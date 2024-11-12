// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "StorageUnit.h"
#include "Stockpile.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AStockpile : public AStorageUnit {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInteractableObject> StockPileClass;
protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;

	virtual bool GetMatchingSocket(FCarryableSocketInfo& OutSocketInfo, int32& OutIndex, const TScriptInterface<ICarryable>& Carryable) override;

	virtual void SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) override;
};