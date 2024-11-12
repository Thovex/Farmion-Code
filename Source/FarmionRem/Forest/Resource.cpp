// Copyright 2020-2023: Jesse J. van Vliet

#include "Resource.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Callbacks.h"
#include "Net/UnrealNetwork.h"

AResource::AResource() {
	UnavailableSegmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrunkMesh"));
	UnavailableSegmentMesh->SetupAttachment(Transform);
	
	AvailableSegmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeMesh"));
	AvailableSegmentMesh->SetupAttachment(UnavailableSegmentMesh);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ChopCollision"));
	CollisionBox->SetupAttachment(Transform);
}

void AResource::OnRep_Chopped_Implementation() {
	
}

void AResource::Respawn() {
	bChopped = false;
	OnRep_Chopped();

	TryConvert();
}

int32 AResource::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

bool AResource::MatchesConditions() {
	return !bChopped;
}

void AResource::Output() {
	Super::Output();

	bChopped = true;
	OnRep_Chopped();

	if (!bOneShot) {
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AResource::Respawn, RespawnTime);
	}
}

void AResource::SpawnOutput() {
	int32 SpawnCountOverride = 1;

	const AInteractableObject* InteractableObject = Cast<AInteractableObject>(AvailableSockets[0].Carryable->GetActorFromInterface());

	// ReSharper disable once CppIncompleteSwitchStatement
	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	switch (InteractableObject->Quality) {
		case EItemQuality::Arcane:
		{
			SpawnCountOverride += FMath::RandBool() ? 1 : 0;
		} break;
		case EItemQuality::Eldar: {
			SpawnCountOverride += FMath::RandBool() ? 2 : 1;
		} break;
	}

	for (size_t i = 0; i < SpawnCountOverride; i++) {
		// TODO: What if this Super() spawns more.. lel ffs
		FallbackSpawnLocation->SetWorldLocation(PotentialDropLocations[FMath::RandRange(0, PotentialDropLocations.Num() - 1)]);
		Super::SpawnOutput();
	}
}

void AResource::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("Chopped", bChopped);
	JsonObject->SetNumberField("RespawnTimeRemaining", GetWorld()->GetTimerManager().GetTimerRemaining(RespawnTimer));
}

void AResource::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bChopped = JsonObject->GetBoolField("Chopped");
	OnRep_Chopped();

	if (bChopped) {
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AResource::Respawn, JsonObject->GetNumberField("RespawnTimeRemaining"));

		if (bIsConverting) {
			bIsConverting = false;
			OnRep_IsConverting();

			ConversionTimerHandle.Invalidate();
		}
	}
}


FText AResource::GetInformationText_Implementation() const {
	FString Text = ToolName.ToString();

	if (bIsConverting) {
		Text += "\n<Sub>Harvesting!</>";
	}
	
	return FText::FromString(Text);
}

void AResource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AResource, bChopped);
}



