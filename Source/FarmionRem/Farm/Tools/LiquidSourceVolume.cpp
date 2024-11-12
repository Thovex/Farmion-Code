// Copyright 2020-2023: Jesse J. van Vliet


#include "LiquidSourceVolume.h"

#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"

ALiquidSourceVolume::ALiquidSourceVolume() {
	PrimaryActorTick.bCanEverTick = false;
}

void ALiquidSourceVolume::BeginPlay() {
	Super::BeginPlay();

	/* Not sure if we use this or want to use this.
	if (HasAuthority()) {
		OnActorBeginOverlap.AddDynamic(this, &ALiquidSourceVolume::ActorEnteredVolume);
		OnActorEndOverlap.AddDynamic(this, &ALiquidSourceVolume::ActorLeavingVolume);
	}
	*/
}

TEnumAsByte<ELiquids> ALiquidSourceVolume::GetLiquidType() {
	return Liquid;
}

/*
void ALiquidSourceVolume::ActorEnteredVolume(AActor* OverlappedActor, AActor* OtherActor) {
	const TScriptInterface<ILiquidTarget> LiquidTargetInt = TScriptInterface<ILiquidTarget>(OtherActor);
	if (LiquidTargetInt) {
		LiquidTargetInt->Supply(Liquid, 10);
	}
}

void ALiquidSourceVolume::ActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor) {
	
}
*/