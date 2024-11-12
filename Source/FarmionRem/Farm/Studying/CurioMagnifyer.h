// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "CurioMagnifyer.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ACurioMagnifyer : public AStorageUnit {
	GENERATED_BODY()

protected:
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;
};
