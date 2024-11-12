// Copyright 2020-2023: Jesse J. van Vliet

#include "Stockpile.h"

#include "FarmionRem/Callbacks.h"

int32 AStockpile::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

bool AStockpile::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (Carryable->GetActorFromInterface()->IsA(StockPileClass)) {
		return Super::MatchesSocketRequirements(Info, Carryable);
	}

	return false;
}

bool AStockpile::GetMatchingSocket(FCarryableSocketInfo& OutSocketInfo, int32& OutIndex, const TScriptInterface<ICarryable>& Carryable) {
	check(!AvailableSockets.IsEmpty());

	for (auto It = AvailableSockets.CreateConstIterator(); It; ++It) {
		const FCarryableSocketInfo& SocketInfo = *It;

		if (SocketInfo.Available && MatchesSocketRequirements(SocketInfo, Carryable)) {
			OutSocketInfo = SocketInfo;
			OutIndex = It.GetIndex();
			return true;
		}
	}

	return false;
}

void AStockpile::SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) {
	Super::SetSocketAvailability(Carryable, Index, Availability);

	if (!Availability) {
		Carryable->GetActorFromInterface()->Tags.Add(GProtected_Tag_Name);
	} else {
		Carryable->GetActorFromInterface()->Tags.Remove(GProtected_Tag_Name);
	}
}
