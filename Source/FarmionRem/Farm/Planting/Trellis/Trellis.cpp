// Copyright 2020-2023: Jesse J. van Vliet


#include "Trellis.h"

#include "FarmionRem/Callbacks.h"


void ATrellis::SetTrellisActive_Implementation(bool bActive) {}

int32 ATrellis::MeetsCarryRequirements(const AFarmionCharacter* Character)
{
	return static_cast<int32>(EInteractCallback::NO_CALLBACK);
}

void ATrellis::BeginPlay() {
	Super::BeginPlay();

	OnRep_Bought();
}

void ATrellis::OnRep_Bought() {
	Super::OnRep_Bought();

	SetTrellisActive(bBought);
}

