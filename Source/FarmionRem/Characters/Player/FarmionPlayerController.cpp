// Copyright 2020-2023: Jesse J. van Vliet

#include "FarmionPlayerController.h"

#include "Blueprint/UserWidget.h"

AFarmionPlayerController::AFarmionPlayerController() {
	bReplicates = true;
}

void AFarmionPlayerController::AcknowledgePossession(APawn* P) {
	Super::AcknowledgePossession(P);

	if (IsLocalPlayerController()) {
		HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);

		if (HUDWidget) {
			HUDWidget->AddToViewport();
		}
	}
}

void AFarmionPlayerController::OnUnPossess() {
	Super::OnUnPossess();

	if (IsLocalPlayerController()) {
		if (HUDWidget) {
			HUDWidget->RemoveFromParent();
		}
	
		HUDWidget = nullptr;
	}
}