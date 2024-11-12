// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "Information.generated.h"

enum class EItemQuality : uint8;

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UInformation : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API IInformation {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Information")
	FText GetInformationText() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Information")
	FText GetMiniInformationText() const;

	virtual bool IsActive();
protected:
	static FText QualityTransformText(const EItemQuality Quality, FText& Text);
};