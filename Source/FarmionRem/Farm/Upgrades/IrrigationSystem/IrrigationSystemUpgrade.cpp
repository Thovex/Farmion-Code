// Copyright 2020-2023: Jesse J. van Vliet

#include "IrrigationSystemUpgrade.h"

#include "EngineUtils.h"
#include "IrrigationSystemLever.h"
#include "IrrigationSystemWaterContainer.h"
#include "FarmionRem/Farm/Land/FarmLand.h"
#include "FarmionRem/Farm/Tools/IgnoreDecayVolume.h"
#include "FarmionRem/Interactables/LinkerComponent.h"

AIrrigationSystemUpgrade::AIrrigationSystemUpgrade() {
	PrimaryActorTick.bCanEverTick = true;
	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

void AIrrigationSystemUpgrade::Multicast_Generic_Networked_Call() {
	Super::Multicast_Generic_Networked_Call();

	const auto FarmLand = Cast<AFarmLand>(Linker->GetLinkedActor(TEXT("FarmLand")));
	FarmLand->WaterAll();
}

void AIrrigationSystemUpgrade::BeginPlay() {
	Super::BeginPlay();
	SetVisibility();
}

void AIrrigationSystemUpgrade::OnRep_Bought() {
	Super::OnRep_Bought();
	SetVisibility();
}

void AIrrigationSystemUpgrade::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);
	JsonObject->SetStringField("IgnoreDecayVolume", IgnoreDecayVolume.Get()->GetFName().ToString());
}

void AIrrigationSystemUpgrade::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	const FString LoadedActorUniqueName = JsonObject->GetStringField("IgnoreDecayVolume");
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		const AActor* Actor = *ActorItr;
		if (Actor->GetFName().ToString() == LoadedActorUniqueName) {
			IgnoreDecayVolume = Actor;
			UE_LOG(LogTemp, Log, TEXT("Found IgnoreDecayVolume: %s"), *Actor->GetName());
			break;
		}
	}

	// Important, needs to find this actor because bought makes the decay volume active.
	Super::Load(JsonObject);
}

void AIrrigationSystemUpgrade::Link() {
	IgnoreDecayVolume->SetActorEnableCollision(bBought);

	WaterContainer = Cast<AIrrigationSystemWaterContainer>(Linker->GetLinkedActor(TEXT("IrrigationSystem_WaterContainer")));
	Lever = Cast<AIrrigationSystemLever>(Linker->GetLinkedActor(TEXT("IrrigationSystem_Lever")));
	SetVisibility();
}

void AIrrigationSystemUpgrade::SetVisibility() {
	if (!WaterContainer.IsValid() || !Lever.IsValid()) {
		return;
	}
	
	WaterContainer->SetActorEnableCollision(bBought);
	WaterContainer->SetActorHiddenInGame(!bBought);
		
	Lever->SetActorEnableCollision(bBought);
	Lever->SetActorHiddenInGame(!bBought);
}

