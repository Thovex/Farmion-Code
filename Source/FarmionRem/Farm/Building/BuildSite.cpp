// Copyright 2020-2023: Jesse J. van Vliet


#include "BuildSite.h"

#include "FarmionRem/Farm/Upgrades/StageUpgrade.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "Net/UnrealNetwork.h"

void ABuildSite::OnRep_IsBuilt_Implementation() {
	
}

ABuildSite::ABuildSite() {
	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

void ABuildSite::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuildSite, bIsBuilt);
}

void ABuildSite::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	ISaveLoad::Save(JsonObject);
}

void ABuildSite::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	ISaveLoad::Load(JsonObject);
}

void ABuildSite::Link() {
	StageUpgrade = Linker->GetLinkedActor("BuildSite");
	check(StageUpgrade.IsValid());

	if (!StageUpgrade->bBought) {
		StageUpgrade->OnUpgradePurchasedDelegate.AddDynamic(this, &ABuildSite::OnBought);
	} else {
		OnBought();
	}
}

void ABuildSite::OnBought() {
	UE_LOG(LogTemp, Warning, TEXT("OnBought"));
	
	bIsBuilt = true;
	OnRep_IsBuilt();
}