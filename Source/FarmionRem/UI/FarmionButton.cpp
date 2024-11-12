// Copyright 2020-2023: Jesse J. van Vliet

#include "FarmionButton.h"

#include "FarmionUILibrary.h"
#include "Components/TextBlock.h"

void UFarmionButton::SynchronizeProperties() {
	Super::SynchronizeProperties();

	// Kinda hacky, maybe we get rid of these later. (If Unused)
	
	OnClicked.RemoveAll(this);
	OnPressed.RemoveAll(this);
	OnReleased.RemoveAll(this);
	OnHovered.RemoveAll(this);
	OnUnhovered.RemoveAll(this);
	
	OnClicked.AddDynamic(this, &UFarmionButton::AdvancedSlateHandleClicked);
	OnPressed.AddDynamic(this, &UFarmionButton::AdvancedSlateHandlePressed);
	OnReleased.AddDynamic(this, &UFarmionButton::AdvancedSlateHandleReleased);
	OnHovered.AddDynamic(this, &UFarmionButton::AdvancedSlateHandleHovered);
	OnUnhovered.AddDynamic(this, &UFarmionButton::AdvancedSlateHandleUnhovered);
}

UTextBlock* FarmionButton_FirstTextChild(UFarmionButton* Button) {
	const UPanelWidget* ButtonAsPanel = Cast<UPanelWidget>(Button);
	if (!ButtonAsPanel) {
		UE_LOG(LogTemp, Error, TEXT("WidgetContainer is not a PanelWidget on [FarmionButton_FirstTextChild]"));
		return nullptr;
	}
	
	if (ButtonAsPanel->GetChildrenCount() == 0) {
		UE_LOG(LogTemp, Error, TEXT("WidgetContainer has no children on [FarmionButton_FirstTextChild]"));
		return nullptr;
	}
	
	UWidget* Child = ButtonAsPanel->GetChildAt(0);
	return Cast<UTextBlock>(Child);
}


void UFarmionButton::FarmionButton_Unhover() {
	UTextBlock* TextBlock = FarmionButton_FirstTextChild(this);
	if (!TextBlock) {
		UE_LOG(LogTemp, Error, TEXT("Child is not a TextBlock on [BindFarmionButton_Unhover]"));
		return;
	}

	//TextBlock->SetColorAndOpacity(FSlateColor { GFarmion_Button_Text_Color });
}

void UFarmionButton::FarmionButton_Hover() {
	UTextBlock* TextBlock = FarmionButton_FirstTextChild(this);
	if (!TextBlock) {
		UE_LOG(LogTemp, Error, TEXT("Child is not a TextBlock on [BindFarmionButton_Hover]"));
		return;
	}

	//TextBlock->SetColorAndOpacity(FSlateColor { GFarmion_Button_Text_Color_Hover });
}

void UFarmionButton::AdvancedSlateHandleClicked() {
	OnClickedAdvanced.Broadcast(this);
}

void UFarmionButton::AdvancedSlateHandlePressed() {
	OnPressedAdvanced.Broadcast(this);
}

void UFarmionButton::AdvancedSlateHandleReleased() {
	OnReleasedAdvanced.Broadcast(this);
}

void UFarmionButton::AdvancedSlateHandleHovered() {
	OnHoveredAdvanced.Broadcast(this);
	FarmionButton_Hover();
}

void UFarmionButton::AdvancedSlateHandleUnhovered() {
	OnUnhoveredAdvanced.Broadcast(this);
	FarmionButton_Unhover();
}