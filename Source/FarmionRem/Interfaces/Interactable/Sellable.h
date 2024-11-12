// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interfaces/Interactable/Quality.h"

#include "UObject/Interface.h"
#include "Sellable.generated.h"

UINTERFACE(Blueprintable)
class USellable : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ISellable {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 BaseSellPrice();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TMap<EItemQuality, float> GetQualitySellPriceModifier();

	virtual int32 GetFullSellPrice() const;

	virtual void Sell();
	virtual bool CanSell();

};