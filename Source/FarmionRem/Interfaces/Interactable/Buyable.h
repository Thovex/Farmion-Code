// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "Buyable.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UBuyable : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API IBuyable {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetBuyPrice() const;
	
	virtual bool CanBuy(int32 Money);
	virtual void Buy();

};