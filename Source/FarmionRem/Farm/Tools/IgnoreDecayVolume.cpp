// Copyright 2020-2023: Jesse J. van Vliet


#include "IgnoreDecayVolume.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"

AIgnoreDecayVolume::AIgnoreDecayVolume() {
	PrimaryActorTick.bCanEverTick = false;

}

void AIgnoreDecayVolume::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		this->OnActorBeginOverlap.AddDynamic(this, &AIgnoreDecayVolume::ActorBeginOverlap);
		this->OnActorEndOverlap.AddDynamic(this, &AIgnoreDecayVolume::ActorEndOverlap);

		GetWorldTimerManager().SetTimerForNextTick([&]() {
			TArray<AActor*> Overlaps;
			
			GetOverlappingActors(Overlaps);
			
			for (AActor* Overlap : Overlaps) {
				ActorBeginOverlap(this, Overlap);
			}
		});
	}

}

void AIgnoreDecayVolume::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) {
	const TScriptInterface<ICycleAffected> CycleAffectedInt = TScriptInterface<ICycleAffected>(OtherActor);
	if (CycleAffectedInt) {
		CycleAffectedInt->TrySetProtect(static_cast<EDecayFlags>(Flags), true);
	}
}

void AIgnoreDecayVolume::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) {
	const TScriptInterface<ICycleAffected> CycleAffectedInt = TScriptInterface<ICycleAffected>(OtherActor);
	if (CycleAffectedInt) {
		CycleAffectedInt->TrySetProtect(static_cast<EDecayFlags>(Flags), false);
	}
}

