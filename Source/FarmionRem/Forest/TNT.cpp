#include "TNT.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "Net/UnrealNetwork.h"

ATNT::ATNT() {
	Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall"));
	Wall->SetupAttachment(Transform);;
}

void ATNT::OnRep_Pulled_Implementation() {
	if (bPulled) {
		AActor* DestroyActor = GetWorld()->SpawnActor<AActor>(ChaosActorToSpawn, Wall->GetComponentLocation(), Wall->GetComponentRotation());
		DestroyActor->SetLifeSpan(5.f);

		Explode();

		Wall->SetVisibility(false);
		Wall->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	} else {
		Wall->SetVisibility(true);
		Wall->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	}
}

int32 ATNT::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

int32 ATNT::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	if (!bBought) {
		return (int32)EInteractCallback::INTERACTABLE_CAN_BE_BOUGHT;
	}

	// NOTE: returns NO CALLBACK if pulled prior.
	return bPulled ? (int32)EInteractCallback::NO_CALLBACK : (int32)EInteractCallback::START_USING_OBJECT;
}

void ATNT::Buy() {
	bBought = true;
	OnRep_Bought();
}

int32 ATNT::GetBuyPrice_Implementation() const {
	return 250;
}

void ATNT::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("Pulled", bPulled);
	JsonObject->SetBoolField("Bought", bBought);
}

void ATNT::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);
	
	bBought = JsonObject->GetBoolField("Bought");
	OnRep_Bought();

	bPulled = JsonObject->GetBoolField("Pulled");
	OnRep_Pulled();
}

ELoadMethod ATNT::LoadMethod() {
	return ELoadMethod::Recreate;
}

void ATNT::Server_Use(AFarmionCharacter* User, const bool bUse) {
	if (bPulled) {
		return;
	}

	bPulled = true;
	OnRep_Pulled();
}

FText ATNT::GetInformationText_Implementation() const {
	if (!bBought) {
		return FText::FromString("Buy TNT?");
	}
	return FText::FromString("Use TNT");
}


void ATNT::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATNT, bBought);
	DOREPLIFETIME(ATNT, bPulled);
}
