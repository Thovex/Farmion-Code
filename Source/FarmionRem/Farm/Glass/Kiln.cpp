// Copyright 2020-2023: Jesse J. van Vliet


#include "Kiln.h"

#include "Mould.h"
#include "FarmionRem/Callbacks.h"

AKiln::AKiln() {
	DropLocation = CreateDefaultSubobject<USceneComponent>(TEXT("DropLocation"));
	DropLocation->SetupAttachment(Transform);
}

int32 AKiln::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

bool AKiln::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (Carryable->GetActorFromInterface()->IsA<AMould>()) {
		return Super::MatchesSocketRequirements(Info, Carryable);
	}

	return false;
}

bool AKiln::MatchesAnyRecipe(FName& MatchedRecipe) {
	if (SocketsInUse > 0) {
		if (AvailableSockets[0].Carryable->GetActorFromInterface()->IsA<AMould>()) {
			const AMould* MouldObject = Cast<AMould>(AvailableSockets[0].Carryable->GetActorFromInterface());
			if (MouldObject->bFilled) {
				return Super::MatchesAnyRecipe(MatchedRecipe);
			}
		}
	}

	return false;
}

void AKiln::ProcessInputAfterConversion() {
	// Ignore Super::ProcessInputAfterConversion()
	
	for (auto& Socket : AvailableSockets) {
		AActor* Actor = Socket.Carryable->GetActorFromInterface();

		if (Actor->IsA<AMould>()) {
			AMould* MouldObject = Cast<AMould>(Actor);
			MouldObject->bFilled = false;
			MouldObject->OnRep_Filled();
		}
		
		Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Actor->SetActorLocationAndRotation(DropLocation->GetComponentLocation(), DropLocation->GetComponentRotation());
		
		Socket.Carryable->CarryableSocket = nullptr;
		Socket.Carryable->CarryableSocketIndex = 0;

		Socket.Carryable = nullptr;
		Socket.Available = true;
	}

	// Mandatory
	SocketsInUse = GetSocketsInUse();
}

void AKiln::ManipulateSpawnedObject(AInteractableObject* SpawnedObject) {
	Super::ManipulateSpawnedObject(SpawnedObject);

	if (SpawnedObject->IsA<ATool>()) {
		ATool* SpawnedObjectTool = Cast<ATool>(SpawnedObject);

		SpawnedObjectTool->bBought = true;
		SpawnedObjectTool->OnRep_Bought();
	}
}

void AKiln::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	const FVector& Loc = DropLocation->GetComponentLocation();
	JsonObject->SetNumberField("DropLocationX", Loc.X);
	JsonObject->SetNumberField("DropLocationY", Loc.Y);
	JsonObject->SetNumberField("DropLocationZ", Loc.Z);
}

void AKiln::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	DropLocation->SetWorldLocation(FVector(
		JsonObject->GetNumberField("DropLocationX"),
		JsonObject->GetNumberField("DropLocationY"),
		JsonObject->GetNumberField("DropLocationZ")
	));
}