// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Buffs.h"
#include "UObject/Interface.h"
#include "Ingredient.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UIngredient : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API IIngredient {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsBoilable() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsSpittable() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsFryable() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FBuff GetBuff() const;
};