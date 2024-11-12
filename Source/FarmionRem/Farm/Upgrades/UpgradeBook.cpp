// Copyright 2020-2023: Jesse J. van Vliet


#include "UpgradeBook.h"

#include "Upgrade.h"
#include "Components/TextRenderComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

AUpgradeBook::AUpgradeBook() {
	PrimaryActorTick.bCanEverTick = false;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("MoneyText"));
	Text->SetupAttachment(SpringArm);

	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));

	bAlwaysRelevant = true;
}

void AUpgradeBook::OnRep_Bought() {
	if (!Upgrade.IsValid()) {
		return;
	}
	
	if (bBought) {
		this->Text->SetText(FText::FromString("Sell " +Upgrade->UpgradeName.ToString() + ": " + FString::FromInt(this->GetFullSellPrice())));
	} else {
		this->Text->SetText(FText::FromString("Buy " +Upgrade->UpgradeName.ToString() + ": " + FString::FromInt(this->IBuyable::Execute_GetBuyPrice(this))));
	}
}

bool AUpgradeBook::CanBuy(const int32 Money) {
	if (Upgrade->bBought) {
		return false;
	}

	return IBuyable::CanBuy(Money);
}

void AUpgradeBook::Buy_Implementation() {
	GAME_STATE
	Upgrade->Quality = DetermineQuality(GameState->CalculateFarmLevel());
	
	Upgrade->bBought = true;
	Upgrade->OnRep_Bought();

	this->bBought = Upgrade->bBought;
	this->OnRep_Bought();
}

void AUpgradeBook::Sell_Implementation() {
	Upgrade->bBought = false;
	Upgrade->OnRep_Bought();

	this->bBought = Upgrade->bBought;
	this->OnRep_Bought();
}

int32 AUpgradeBook::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	GAME_STATE
	
	if (CanBuy(GameState->Money)) {
		return (int32)EInteractCallback::INTERACTABLE_CAN_BE_BOUGHT;
	}

	if (this->bBought && CanSell()) {
		return (int32)EInteractCallback::INTERACTABLE_CAN_BE_SOLD;
	}
	
	return (int32)EInteractCallback::NO_CALLBACK;
}

void AUpgradeBook::PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap) {
	Super::PostLoaded(JsonObject, UniqueIdToActorMap);
}

void AUpgradeBook::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField(TEXT("BuyPrice"), IBuyable::Execute_GetBuyPrice(this));
	JsonObject->SetNumberField(TEXT("SellPrice"), GetFullSellPrice());
}

void AUpgradeBook::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	BuyPrice = JsonObject->GetNumberField(TEXT("BuyPrice"));
	SellPrice = JsonObject->GetNumberField(TEXT("SellPrice"));
}

FText AUpgradeBook::GetInformationText_Implementation() const {
	return Text->Text;
}

int32 AUpgradeBook::GetBuyPrice_Implementation() const {
	return BuyPrice;
}

int32 AUpgradeBook::GetFullSellPrice() const {
	return SellPrice;
}

void AUpgradeBook::Link() {
	Upgrade = Linker->GetLinkedActor(TEXT("Upgrade"));

	this->bBought = Upgrade->bBought;
	this->OnRep_Bought();
}

void AUpgradeBook::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUpgradeBook, bBought);
	DOREPLIFETIME(AUpgradeBook, BuyPrice);
	DOREPLIFETIME(AUpgradeBook, SellPrice);
}
