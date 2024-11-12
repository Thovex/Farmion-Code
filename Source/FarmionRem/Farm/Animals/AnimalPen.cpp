// Copyright 2020-2023: Jesse J. van Vliet


#include "AnimalPen.h"

#include "Animal.h"
#include "FarmionRem/Callbacks.h"

int32 AAnimalPen::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

bool AAnimalPen::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (Carryable->GetActorFromInterface()->IsA<AAnimal>()) {
		return Super::MatchesSocketRequirements(Info, Carryable);
	}

	return false;
}

FText AAnimalPen::GetInformationText_Implementation() const {
	FString Info = "Animal Pen\n";

	if (!bBought) {
		Info += FString::Printf(TEXT("<Sub>Purchase? $%d</>"), IBuyable::Execute_GetBuyPrice(this));
		return FText::FromString(Info);
	}

	if (SocketsInUse == 0) {
		Info += "<Sub>Empty</>";
	} else if (SocketsInUse == AvailableSockets.Num()) {
		Info += "<Sub>At Capacity</>";
	} else {
		Info += FString::Printf(TEXT("<Sub>%d/%d</>"), SocketsInUse, AvailableSockets.Num());
	}
	
	return FText::FromString(Info);
}