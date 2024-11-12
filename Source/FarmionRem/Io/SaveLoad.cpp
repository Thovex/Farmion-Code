// Copyright 2020-2023: Jesse J. van Vliet

#include "SaveLoad.h"

#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"

void ISaveLoad::SaveTransform(const TSharedPtr<FJsonObject>& JsonObject, const AActor* Actor) {
	const FVector& Location = Actor->GetActorLocation();
	JsonObject->SetNumberField("LocationX", Location.X);
	JsonObject->SetNumberField("LocationY", Location.Y);
	JsonObject->SetNumberField("LocationZ", Location.Z);

	const FRotator& Rotation = Actor->GetActorRotation();
	JsonObject->SetNumberField("RotationPitch", Rotation.Pitch);
	JsonObject->SetNumberField("RotationYaw", Rotation.Yaw);
	JsonObject->SetNumberField("RotationRoll", Rotation.Roll);

	const FVector& Scale = Actor->GetActorScale();
	JsonObject->SetNumberField("ScaleX", Scale.X);
	JsonObject->SetNumberField("ScaleY", Scale.Y);
	JsonObject->SetNumberField("ScaleZ", Scale.Z);
}

void ISaveLoad::SaveClass(const TSharedPtr<FJsonObject>& JsonObject, const UObject* Object) {
	JsonObject->SetStringField("Class", Object->GetClass()->GetPathName());
}

void ISaveLoad::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	JsonObject->SetNumberField("UniqueId", GetId());
	JsonObject->SetNumberField("LoadMethod", static_cast<int32>(LoadMethod()));
	
	UObject* Object = _getUObject();
	if (!Object) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Save Called. Target is not an UObject, Ignoring base prop(s): Class"));
		return;
	}
	
	SaveClass(JsonObject, Object);

	AActor* Actor = static_cast<AActor*>(Object);
	if (!Actor) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Save Called. Target is not an AActor, Ignoring base prop(s): Location, Rotation, Scale"));
		return;
	}
	
	UActorComponent* ActorLinkerComp = Actor->GetComponentByClass(ULinkerComponent::StaticClass());
	if (ActorLinkerComp) {
		ULinkerComponent* LinkerComp = static_cast<ULinkerComponent*>(ActorLinkerComp);

		if (!LinkerComp->Links.IsEmpty()) {

			TArray<TSharedPtr<FJsonValue>> LinksArray;

			for (auto& [Name, InteractablePtr] : LinkerComp->Links) {
				const TSharedPtr<FJsonObject> LinkEntry = MakeShareable(new FJsonObject());
				checkf(InteractablePtr, TEXT("LinkerComponent Link [%s] has a null InteractablePtr!"), *Name.ToString());
				
				const TScriptInterface<ISaveLoad> SaveLoadInt = TScriptInterface<ISaveLoad>(InteractablePtr);
				check(SaveLoadInt);
				
				LinkEntry->SetStringField(TEXT("Name"), Name.ToString());
				LinkEntry->SetNumberField(TEXT("UniqueId"), SaveLoadInt->GetId());

				const TSharedPtr<FJsonValueObject> LinkEntryValue = MakeShareable(new FJsonValueObject(LinkEntry));
				LinksArray.Add(LinkEntryValue);
			}

			JsonObject->SetArrayField(TEXT("Links"), LinksArray);
		}
	}

	const TScriptInterface<ICycleAffected> CycleAffectedInt = TScriptInterface<ICycleAffected>(Actor);
	if (CycleAffectedInt) {
		JsonObject->SetBoolField("IsProtected", CycleAffectedInt->IsProtected());
	}

	SaveTransform(JsonObject, Actor);

	JsonObject->SetBoolField("AutoRecreate", true);
}

void ISaveLoad::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	UObject* Object = _getUObject();
	if (!Object) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Save Called. Target is not an UObject, Ignoring base prop(s): Class"));
		return;
	}

	// Set unique Id. Let's hope its a good unique.
	Id = JsonObject->GetNumberField("UniqueId");

	AActor* Actor = static_cast<AActor*>(Object);
	if (!Actor) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Save Called. Target is not an AActor, Ignoring base prop(s): Location, Rotation, Scale"));
		return;
	}
	
	const TScriptInterface<ICycleAffected> CycleAffectedInt = TScriptInterface<ICycleAffected>(Actor);
	if (CycleAffectedInt) {
		if (JsonObject->GetBoolField("IsProtected")) {
			if (!Actor->Tags.Contains(GProtected_Tag_Name)) {
				Actor->Tags.Add(GProtected_Tag_Name);		
			}
		}
	}
}

void ISaveLoad::PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap) {
	UObject* Object = _getUObject();
	if (!Object) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Save Called. Target is not an UObject, Ignoring base prop(s): Class"));
		return;
	}

	AActor* Actor = static_cast<AActor*>(Object);
	if (!Actor) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Save Called. Target is not an AActor, Ignoring base prop(s): Location, Rotation, Scale"));
		return;
	}
	
	UActorComponent* ActorLinkerComp = Actor->GetComponentByClass(ULinkerComponent::StaticClass());
	if (ActorLinkerComp) {
		ULinkerComponent* LinkerComp = static_cast<ULinkerComponent*>(ActorLinkerComp);
		const TArray<TSharedPtr<FJsonValue>>* LinksArrayPtr = nullptr;
		if (!JsonObject->TryGetArrayField(TEXT("Links"), LinksArrayPtr) || LinksArrayPtr->IsEmpty()) {
			return;
		}
		
		TArray<TSharedPtr<FJsonValue>> LinksArray = *LinksArrayPtr;
		for (const TSharedPtr<FJsonValue>& LinkValue : LinksArray) {
			const TSharedPtr<FJsonObject>& LinkObject = LinkValue->AsObject();

			const uint32 UniqueId = LinkObject->GetNumberField(TEXT("UniqueId"));

			FLinkerData LinkerData;
			LinkerData.Actor = UniqueIdToActorMap[UniqueId];
			LinkerData.Name = FName(LinkObject->GetStringField(TEXT("Name")));
			
			LinkerComp->Links.Add(LinkerData);
			UE_LOG(LogTemp, Log, TEXT("Linking %s to %s"), *Actor->GetName(), *UniqueIdToActorMap[UniqueId]->GetName());
		}

		LinkerComp->OnRep_Links();
	}

}

bool ISaveLoad::ClearExisting(AActor* ThisAsActor) {
	// Clear potential timers..
	ThisAsActor->GetWorldTimerManager().ClearAllTimersForObject(ThisAsActor);
	return ThisAsActor->Destroy(true);
}

bool ISaveLoad::Parser(const TSharedPtr<FJsonObject> JsonObject, FString& JsonData) {
	if (JsonObject->Values.IsEmpty()) {
		return false;
	}
	
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonData);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	bool bPrintJson;
	if (JsonObject->TryGetBoolField("PrintJson", bPrintJson) && bPrintJson) {
		UE_LOG(LogTemp, Warning, TEXT("Parsed JSON: %s"), *JsonData);
	}
	return true;
}

ELoadMethod ISaveLoad::LoadMethod() {
	return ELoadMethod::Recreate;
}

uint32 ISaveLoad::LoadPriority() {
	return 0;
}

uint32 ISaveLoad::GetId() {
	if (Id != 0) {
		return Id;
	}

	Id = _getUObject()->GetUniqueID();
	return Id;
}

void ISaveLoad::SaveClassPath(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, const UObject* Object) {
	JsonObject->SetStringField(FieldName, Object == nullptr ? "NULL" : Object->GetPathName());
}

UClass* ISaveLoad::LoadClassPath(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName) {
	UClass* Class = nullptr;
	
	FString ClassPath;
	if (JsonObject->TryGetStringField(FieldName, ClassPath)) {
		if (ClassPath == "NULL") {
			return Class;
		}
		
		UClass* ClassPtr = StaticLoadClass(AActor::StaticClass(), nullptr, *ClassPath);

		if (ClassPtr) {
			Class = ClassPtr;
		} else {
			UE_LOG(LogTemp, Error, TEXT("Failed to load class from path: %s"), *ClassPath);
		}
	}

	return Class;
}
