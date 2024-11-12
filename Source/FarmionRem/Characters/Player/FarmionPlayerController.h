// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "GameFramework/PlayerController.h"
#include "FarmionPlayerController.generated.h"

enum EWeatherEffect : uint8; 

UCLASS(Blueprintable)
class FARMIONREM_API AFarmionPlayerController : public APlayerController {
	GENERATED_BODY()

public:
	AFarmionPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> HUDWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* HUDWidget;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateInformationWidget(const FText& Text);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartWeatherEffect(EWeatherEffect Effect);

protected:
	virtual void AcknowledgePossession(APawn* P) override;
	virtual void OnUnPossess() override;

};