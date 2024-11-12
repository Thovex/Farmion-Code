// Copyright 2020-2023: Jesse J. van Vliet


#include "CheeseAreaVolume.h"

#include "CheeseRack.h"

ACheeseAreaVolume::ACheeseAreaVolume() {
	PrimaryActorTick.bCanEverTick = false;
}

void ACheeseAreaVolume::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		OnActorBeginOverlap.AddDynamic(this, &ACheeseAreaVolume::ActorBeginOverlap);
		OnActorEndOverlap.AddDynamic(this, &ACheeseAreaVolume::ActorEndOverlap);

		GetWorldTimerManager().SetTimerForNextTick([&]() {
			TArray<AActor*> Overlaps;
			
			GetOverlappingActors(Overlaps);
			
			for (AActor* Overlap : Overlaps) {
				ActorBeginOverlap(this, Overlap);
			}
		});
	}
}

void ACheeseAreaVolume::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) {
	UE_LOG(LogTemp, Warning, TEXT("ActorBeginOverlap with %s"), *OtherActor->GetName());
	if (Cast<ACheeseRack>(OtherActor)) {
		ACheeseRack* Rack = Cast<ACheeseRack>(OtherActor);
		Rack->AreaType = this->AreaType;
	}
}

void ACheeseAreaVolume::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) {
	if (Cast<ACheeseRack>(OtherActor)) {
		ACheeseRack* Rack = Cast<ACheeseRack>(OtherActor);
		Rack->AreaType = ECheeseAreaType::ECheeseAreaType_Outside;
	}
}