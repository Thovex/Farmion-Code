// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "NoCollisionStorage.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "SpecificNoCollisionStorage.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ASpecificNoCollisionStorage : public ANoCollisionStorage {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AInteractableObject>> AllowedTypes;

protected:
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;
};