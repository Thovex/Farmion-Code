// Copyright 2020-2023: Jesse J. van Vliet


#include "TrellisUpgrade.h"

#include "FarmionRem/Farm/Planting/Trellis/Trellis.h"
#include "FarmionRem/Interactables/LinkerComponent.h"

ATrellisUpgrade::ATrellisUpgrade() {
	PrimaryActorTick.bCanEverTick = false;

	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

void ATrellisUpgrade::Build() {
	Super::Build();

	HandleBuildTrellises();
}

void ATrellisUpgrade::PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap) {
	Super::PostLoaded(JsonObject, UniqueIdToActorMap);

	if (bBought) {
		HandleBuildTrellises();
	}
}

void ATrellisUpgrade::HandleBuildTrellises() {
	if (bEnsure) {
		UE_LOG(LogTemp, Error, TEXT("TrellisUpgrade::Build: Already ensured!"));
		return;
	}

	for (auto* Trellis : Trellises) {
		if (Trellis) {
			Trellis->bBought = true;
			Trellis->OnRep_Bought();
		}
	}

	bEnsure = true;
}

void ATrellisUpgrade::Link() {
	for (const auto& Link : Linker->Links) {
		if (Link.Actor) {
			ATrellis* Trellis = Cast<ATrellis>(Link.Actor);
			Trellises.Add(Trellis);
		}
	}
}