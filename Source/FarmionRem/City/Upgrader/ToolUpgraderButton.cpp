// Copyright 2020-2023: Jesse J. van Vliet


#include "ToolUpgraderButton.h"

#include "ToolUpgrader.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "FarmionRem/Util/CommonLibrary.h"

AToolUpgraderButton::AToolUpgraderButton() {
	PrimaryActorTick.bCanEverTick = false;
	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

int32 AToolUpgraderButton::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	if (Upgrader && UCommonLibrary::HasFlag(Upgrader->CanUpgradeTool(), ECanUpgradeToolCallback::CAN_UPGRADE)) {
		return (int32)EInteractCallback::INTERACTABLE_CAN_BE_BOUGHT;
	}
	
	return (int32)EInteractCallback::NO_CALLBACK;
}

void AToolUpgraderButton::Buy() {
	if (Upgrader) {
		Upgrader->StartUpgrade(bCanDowngrade);
	}
	
	Multicast_Purchased();
}

FText AToolUpgraderButton::GetInformationText_Implementation() const {
	if (!Upgrader) {
		return FText::FromString("Unavailable");
	}

	const int32 UpgradeCallback = Upgrader->CanUpgradeTool();
	if (UCommonLibrary::HasFlag(UpgradeCallback, ECanUpgradeToolCallback::UPGRADE_IN_PROGRESS)) {
		return FText::FromString("Upgrade in Progress\n<Sub>Please Wait...</>");
	}
	
	if (UCommonLibrary::HasFlag(UpgradeCallback, ECanUpgradeToolCallback::PLACE_TOOL_IN_SOCKET)) {
		return FText::FromString("Place Tool on Upgrader");
	}

	if (UCommonLibrary::HasFlag(UpgradeCallback, ECanUpgradeToolCallback::TOOL_AT_MAX_QUALITY)) {
		return FText::FromString("Tool is of Highest Quality");
	}

	if (UCommonLibrary::HasFlag(UpgradeCallback, ECanUpgradeToolCallback::CAN_UPGRADE)) {
		return FText::FromString(FString::Printf(TEXT("Reroll Tool Quality?\n<Sub>%hs, $%d</>"), bCanDowngrade ? "Can downgrade" : "Can't downgrade", IBuyable::Execute_GetBuyPrice(this)));
	}

	return FText::FromString("Unavailable");
}

void AToolUpgraderButton::Multicast_Purchased_Implementation() {
	
}

void AToolUpgraderButton::Link() {
	Upgrader = Cast<AToolUpgrader>(Linker->GetLinkedActor("Upgrader"));
}

