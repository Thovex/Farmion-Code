// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Components/Button.h"
#include "FarmionButton.generated.h"

class UFarmionButton;
class FReply;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonAdvancedClickedEvent, UFarmionButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonAdvancedPressedEvent, UFarmionButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonAdvancedReleasedEvent, UFarmionButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonAdvancedHoverEvent, UFarmionButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonAdvancedUnhoverEvent, UFarmionButton*, Button);

/**
 * 
 */
UCLASS(Blueprintable)
class FARMIONREM_API UFarmionButton : public UButton {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonAdvancedClickedEvent OnClickedAdvanced;

	/** Called when the button is pressed */
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonAdvancedPressedEvent OnPressedAdvanced;

	/** Called when the button is released */
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonAdvancedReleasedEvent OnReleasedAdvanced;

	UPROPERTY( BlueprintAssignable, Category = "Button|Event" )
	FOnButtonAdvancedHoverEvent OnHoveredAdvanced;

	UPROPERTY( BlueprintAssignable, Category = "Button|Event" )
	FOnButtonAdvancedUnhoverEvent OnUnhoveredAdvanced;
	
protected:
	virtual void SynchronizeProperties() override;
	
	UFUNCTION()
	void AdvancedSlateHandleClicked();

	UFUNCTION()
	void AdvancedSlateHandlePressed();

	UFUNCTION()
	void AdvancedSlateHandleReleased();

	UFUNCTION()
	void AdvancedSlateHandleHovered();

	UFUNCTION()
	void AdvancedSlateHandleUnhovered();

	UFUNCTION(BlueprintCallable)
	void FarmionButton_Unhover();

	UFUNCTION(BlueprintCallable)
	void FarmionButton_Hover();
	
};
