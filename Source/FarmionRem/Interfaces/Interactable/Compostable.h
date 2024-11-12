// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "Compostable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCompostable : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ICompostable {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetCompostValue() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanCompost() const;

	virtual bool CanCompost_Implementation() const { return true; }
	virtual int32 GetCompostValue_Implementation() const { return 1; } 

};