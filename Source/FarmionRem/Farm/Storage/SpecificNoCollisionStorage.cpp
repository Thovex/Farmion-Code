// Copyright 2020-2023: Jesse J. van Vliet


#include "SpecificNoCollisionStorage.h"

bool ASpecificNoCollisionStorage::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	const AActor* ActorToCheck = Carryable->GetActorFromInterface();
	for (auto&& AllowedType : AllowedTypes) {
		if (ActorToCheck->IsA(AllowedType)) {
			return Super::MatchesSocketRequirements(Info, Carryable);
		}
	}

	return false;
}