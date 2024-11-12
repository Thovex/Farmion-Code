// Copyright 2020-2023: Jesse J. van Vliet

#include "Tool.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Interfaces/Interactable/Sellable.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "Net/UnrealNetwork.h"

ATool::ATool()
	: AInteractableObject() {
}

void ATool::OnRep_Bought() {
	
}

void ATool::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATool, bBought);
}

int32 ATool::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	int32 Callback = Super::Interact(InteractInstigator, InteractionHit);

	if (!bBought) {
		UCommonLibrary::SetFlag(Callback, (int32)EInteractCallback::INTERACTABLE_CAN_BE_BOUGHT);
	}

	return Callback;
}

int32 ATool::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	if (bBought) {
		return Super::MeetsCarryRequirements(Character);
	}
	return (int32)EInteractCallback::NO_CALLBACK;
}

void ATool::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("Bought", bBought);
}

void ATool::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bBought = JsonObject->GetBoolField("Bought");
	OnRep_Bought();
}

FText ATool::GetInformationText_Implementation() const {
	if (!bBought) {
		return FText::FromString(FString::Printf(TEXT("Buy %s?\n<Sub>$"), *ToolName.ToString()) + FString::FromInt(IBuyable::Execute_GetBuyPrice((UObject*)this)) + "</>");
	}
	
	return FText::FromString(FString::Printf(TEXT("<Prod>%s</>"), *ToolName.ToString()));
}

FText ATool::GetMiniInformationText_Implementation() const {
	return FText::FromString(ToolName.ToString());
}

void ATool::Buy() {
	GAME_STATE
	
	Quality = DetermineQuality(GameState->CalculateFarmLevel());
	
	bBought = true;
	OnRep_Bought();
}

int32 ATool::GetBuyPrice_Implementation() const {
	return BuyPrice;
}