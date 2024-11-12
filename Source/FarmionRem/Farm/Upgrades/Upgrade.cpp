// Copyright 2020-2023: Jesse J. van Vliet

#include "Upgrade.h"

#include "UpgradeBook.h"
#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"

AUpgrade::AUpgrade() {
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	
	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;
}

void AUpgrade::OnRep_Bought() {
	OnUpgradePurchased();

	OnUpgradePurchasedDelegate.Broadcast();
}

void AUpgrade::Multicast_Generic_Networked_Call_Implementation() {
	Generic_Networked_Call();
}

void AUpgrade::BeginPlay() {
	Super::BeginPlay();

	GAME_STATE
		
	if (!HasQuality()) {
		Quality = DetermineQuality(GameState->CalculateFarmLevel());
	}
}

void AUpgrade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUpgrade, bBought);
	DOREPLIFETIME(AUpgrade, Quality);
}

void AUpgrade::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	ISaveLoad::Save(JsonObject);
	
	JsonObject->SetBoolField("Bought", bBought);
	JsonObject->SetNumberField("Quality", static_cast<int32>(Quality));
}

void AUpgrade::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	ISaveLoad::Load(JsonObject);

	Quality = static_cast<EItemQuality>(JsonObject->GetIntegerField("Quality"));

	bBought = JsonObject->GetBoolField("Bought");
	OnRep_Bought();
}

EItemQuality AUpgrade::GetQuality() {
	return Quality;
}

bool AUpgrade::HasQuality() const {
	return Quality != EItemQuality::None;
}
