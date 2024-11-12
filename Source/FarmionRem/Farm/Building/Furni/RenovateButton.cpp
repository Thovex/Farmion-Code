// Copyright 2020-2023: Jesse J. van Vliet


#include "RenovateButton.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"

ARenovateButton::ARenovateButton() {
	PrimaryActorTick.bCanEverTick = false;
}

void ARenovateButton::OnRep_RenovateActive_Implementation() {}

void ARenovateButton::BeginPlay() {
	Super::BeginPlay();

	OnRep_RenovateActive();
}

int32 ARenovateButton::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return 0;
}

int32 ARenovateButton::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return !bRenovateActive ? (int32)EInteractCallback::START_USING_OBJECT : (int32)EInteractCallback::STOP_USING_OBJECT;
}

void ARenovateButton::Server_Use(AFarmionCharacter* User, bool bUse) {
	bRenovateActive = !bRenovateActive;

	GAME_STATE

	GameState->bRenovateHouseMode = bRenovateActive;
	GameState->OnRep_RenovateHouseMode();

	OnRep_RenovateActive();
}

FText ARenovateButton::GetInformationText_Implementation() const {
	return FText::FromString(bRenovateActive ? "Stop renovating" : "Renovate");
}

void ARenovateButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARenovateButton, bRenovateActive);
}
