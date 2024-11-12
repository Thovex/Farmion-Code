// Copyright 2020-2023: Jesse J. van Vliet

#include "InteractableProductSeed.h"

#include "Net/UnrealNetwork.h"

AInteractableProductSeed::AInteractableProductSeed() {
	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(Transform);
	
	SeedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SeedMesh"));
	SeedMesh->SetupAttachment(Pivot);
}

void AInteractableProductSeed::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		
	DOREPLIFETIME(AInteractableProductSeed, bPicked);
}

void AInteractableProductSeed::OnRep_Picked_Implementation() {}

void AInteractableProductSeed::Save(const TSharedPtr<FJsonObject>& JsonObject) {

	// The FarmLand will save the CropPlant, the CropPlant will save this product.
	if (!CropPtr) {
		Super::Save(JsonObject);

		JsonObject->SetBoolField("Picked", bPicked);
	}
}

void AInteractableProductSeed::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bPicked = JsonObject->GetBoolField("Picked");
	OnRep_Picked();
}
