// Copyright 2020-2023: Jesse J. van Vliet


#include "CurioMagnifyer.h"

#include "Curio.h"

int32 ACurioMagnifyer::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return 0;
}

int32 ACurioMagnifyer::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return 0;
}

bool ACurioMagnifyer::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	const UClass* CurioClass = Carryable->GetActorFromInterface()->GetClass();
	
	if (CurioClass->ImplementsInterface(UCurio::StaticClass())) {
		TScriptInterface<ICurio> Curio (Carryable->GetActorFromInterface());

		UE_LOG(LogTemp, Warning, TEXT("LP: %d"), Curio->LearningPoints());
		
		return Super::MatchesSocketRequirements(Info, Carryable);
	}

	return false;
}
