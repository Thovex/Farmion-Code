// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Blueprint/UserWidget.h"
#include "NameplateWidget.generated.h"

class AFarmionPlayerController;

/**
 * 
 */
UCLASS(BlueprintType)
class FARMIONREM_API UNameplateWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetName(AFarmionPlayerController* Controller, FName Name);
};
