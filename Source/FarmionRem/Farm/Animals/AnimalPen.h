// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "GameFramework/Actor.h"
#include "AnimalPen.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AAnimalPen : public AStorageUnit {
	GENERATED_BODY()

protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;
	
	virtual FText GetInformationText_Implementation() const override;
};