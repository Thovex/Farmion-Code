// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "UObject/Interface.h"
#include "Curio.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UCurio : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ICurio {
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 LearningPoints() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float LearningTime() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FName CurioName();
};
