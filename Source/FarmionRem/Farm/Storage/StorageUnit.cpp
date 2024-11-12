// Copyright 2020-2023: Jesse J. van Vliet

#include "StorageUnit.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameInstance.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "Net/UnrealNetwork.h"

AStorageUnit::AStorageUnit() {
	ToolName = "Storage";
}

void AStorageUnit::SetSocketAvailability_Implementation(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) {
	if (AvailableSockets.IsValidIndex(Index)) {
		AvailableSockets[Index].Carryable = !Availability ? Carryable : nullptr;
		AvailableSockets[Index].Available = Availability;

		if (const TScriptInterface<ICycleAffected> CycleAffectedInt = TScriptInterface<ICycleAffected>(Carryable->GetActorFromInterface())) {
			CycleAffectedInt->TrySetProtect(static_cast<EDecayFlags>(ProtectionFlags), !Availability);
		}
	}

	SocketsInUse = GetSocketsInUse();
	UE_LOG(LogDebug, Log, TEXT("Socket availability set. Carryable: %s, Index: %d, Availability: %d"), *Carryable->GetActorFromInterface()->GetName(), Index, Availability);
}

bool AStorageUnit::OverrideCanCarry_Implementation() {
	return true;
}

TArray<FCarryableSocketInfo> AStorageUnit::GenerateDefaultSockets(TArray<USceneComponent*> Components) {
	TArray<FCarryableSocketInfo> Sockets;
		
	for (USceneComponent* Component : Components) {
		FCarryableSocketInfo SocketInfo;
		SocketInfo.Component = Component;
		SocketInfo.Available = true;
		Sockets.Add(SocketInfo);
	}

	return Sockets;
}

TArray<FCarryableSocketInfo> AStorageUnit::GenerateFromArray(const TArray<UBoxComponent*>& Components) {
	TArray<FCarryableSocketInfo> Sockets;

	for (UBoxComponent* Component : Components) {
		FCarryableSocketInfo SocketInfo;
		SocketInfo.Component = Cast<USceneComponent>(Component);
		SocketInfo.Available = true;
		Sockets.Add(SocketInfo);
	}
	
	//UE_LOG(LogTemp, Error, TEXT("Count of sockets: %d"), Sockets.Num());
	return Sockets;
}

void AStorageUnit::BeginPlay() {
	Super::BeginPlay();
	OnBeginPlay(); // Important

	UE_LOG(LogStorageUnit, Log, TEXT("StorageUnit BeginPlay called. Available socket count: %d"), AvailableSockets.Num());
}

int32 AStorageUnit::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	int32 Callback = Super::MeetsCarryRequirements(Character);

	if (SocketsInUse > 0 && !bCanCarryWhileFilled) {
		UCommonLibrary::ClearFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED);
	}
	
	if (!OverrideCanCarry()) {
		UCommonLibrary::ClearFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED);
	}
	
	return Callback;
}

void AStorageUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStorageUnit, SocketsInUse);
}

TArray<AActor*> AStorageUnit::ExtraIgnoreActors() const {
	TArray<AActor*> IgnoreActors;
	CollectIgnoreActors(IgnoreActors);

	for (const auto& IgnoreActor : IgnoreActors) {
		UE_LOG(LogStorageUnit, Log, TEXT("Ignored actor: %s"), *IgnoreActor->GetName());
	}

	UE_LOG(LogStorageUnit, Log, TEXT("ExtraIgnoreActors called. Ignored actor count: %d"), IgnoreActors.Num());

	return IgnoreActors;
}

void AStorageUnit::CollectIgnoreActors(TArray<AActor*>& IgnoreActors) const {
	for (const FCarryableSocketInfo& SocketInfo : AvailableSockets) {
		if (SocketInfo.Available) {
			continue;
		}

		AActor* CarryableActor = SocketInfo.Carryable->GetActorFromInterface();
		IgnoreActors.Add(CarryableActor);

		// Check if CarryableActor is an AStorageUnit and if so, call CollectIgnoreActors recursively
		const AStorageUnit* StorageUnit = Cast<AStorageUnit>(CarryableActor);
		if (StorageUnit != nullptr) {
			StorageUnit->CollectIgnoreActors(IgnoreActors);
		}
	}

	UE_LOG(LogStorageUnit, Log, TEXT("CollectIgnoreActors called. Ignored actor count: %d"), IgnoreActors.Num());
}

void AStorageUnit::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	// Is Being Held... DoNotRecreate
	if (CarryableSocket) {
		JsonObject->SetBoolField("AutoRecreate", false);
	}

	TArray<TSharedPtr<FJsonValue>> AvailableSocketsArray;
	for (size_t Index = 0; Index < AvailableSockets.Num(); Index++) {
		const FCarryableSocketInfo& SocketInfo = AvailableSockets[Index];
		if (SocketInfo.Available) {
			continue;
		}

		// Create a JSON object to store the socket data
		TSharedPtr<FJsonObject> JsonSocket = MakeShareable(new FJsonObject());
		JsonSocket->SetNumberField("Index", Index);

		TScriptInterface<ISaveLoad> SaveLoadInterface = TScriptInterface<ISaveLoad>(SocketInfo.Carryable->GetActorFromInterface());
		if (SaveLoadInterface) {
			UE_LOG(LogStorageUnit, Log, TEXT("Saving socket carryable data. Socket index: %llu, Carryable: %s"), Index, *SocketInfo.Carryable->GetActorFromInterface()->GetName());

			SaveLoadInterface->Save(JsonSocket);
		}
		
		TSharedPtr<FJsonValueObject> SocketValue = MakeShareable(new FJsonValueObject(JsonSocket));
		AvailableSocketsArray.Add(SocketValue);
	}
	
	JsonObject->SetArrayField(TEXT("Sockets"), AvailableSocketsArray);
}

void AStorageUnit::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	auto& Sockets = JsonObject->GetArrayField("Sockets");
	for (size_t Index = 0; Index < Sockets.Num(); Index++) {
		const TSharedPtr<FJsonObject> SocketObject = Sockets[Index]->AsObject();
		const int32 SocketIndex = SocketObject->GetIntegerField("Index");
		if (AvailableSockets.IsValidIndex(SocketIndex)) {
			TMap<uint32, AActor*> EmptyMap; // We don't want to use this in this scenario.
			
			AActor* Actor = UFarmionGameInstance::AutoCreateLoadedObject(GetWorld(), SocketObject, EmptyMap);
			Actor->AttachToComponent(AvailableSockets[SocketIndex].Component, FAttachmentTransformRules::KeepWorldTransform);

			TScriptInterface<ICarryable> Carryable = TScriptInterface<ICarryable>(Actor);
			if (Carryable) {
				SetSocketAvailability(Carryable, SocketIndex, false);
				Carryable->CarryableSocket = this;
				Carryable->CarryableSocketIndex = SocketIndex;

				Carryable->SetCollisions(ECollisionEnabled::NoCollision);

				const auto& SocketInfos = Execute_GetCarryableSocketInfo(this);

				FVector Origin, Extent;
				SocketInfos[SocketIndex].GetSocketBounds(Origin, Extent);

				auto& SocketInfo = SocketInfos[SocketIndex];
				if (SocketInfo.Pivot == nullptr) {
					//Actor->AddActorLocalOffset(-FVector(0.f, 0.f, Extent.Z));
				} else {
					Actor->SetActorLocation(SocketInfo.Pivot->GetComponentLocation());
				}

				
				UE_LOG(LogStorageUnit, Log, TEXT("Loaded socket carryable data. Socket index: %d, Carryable: %s"), SocketIndex, *Carryable->GetActorFromInterface()->GetName());
			}
		}
	}

	UE_LOG(LogStorageUnit, Log, TEXT("StorageUnit Load called. Loaded socket count: %d"), Sockets.Num());
}

bool AStorageUnit::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (bBought) {
		const AActor* Actor = Carryable->GetActorFromInterface();
		const TSubclassOf<AInteractableObject> GetClass = Actor->GetClass();
		
		if (ClassToSocketMapper.IsEmpty() || !ClassToSocketMapper.Contains(GetClass)) {
			return ICarryableSocket::MatchesSocketRequirements(Info, Carryable);
		}
		
		for (const TSoftObjectPtr<USceneComponent> TestComponent : ClassToSocketMapper.Find(GetClass)->Components) {
			if (TestComponent == Info.Component) {
				return ICarryableSocket::MatchesSocketRequirements(Info, Carryable);
			}
		}
	}

	return false;
}

FText AStorageUnit::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();	
	}
	
	if (SocketsInUse == 0) {
		return FText::FromString(FString::Printf(TEXT("%s\n<Sub>Empty</>"), *ToolName.ToString()));
	}
	
	if (SocketsInUse == AvailableSockets.Num()) {
		return FText::FromString(FString::Printf(TEXT("%s\n<Sub>At Capacity</>"), *ToolName.ToString()));
	}
	
	return FText::FromString(FString::Printf(TEXT("%s\n<Sub>Capacity (%d/%d)</>"), *ToolName.ToString(), SocketsInUse, AvailableSockets.Num()));
}

int32 AStorageUnit::GetPriority() const {
	return -1;
}
	