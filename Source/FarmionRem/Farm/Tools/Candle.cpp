// Copyright 2020-2023: Jesse J. van Vliet


#include "Candle.h"

#include "Components/SphereComponent.h"
#include "FarmionRem/Interfaces/Fuel/FuelTarget.h"

ACandle::ACandle() {
	PrimaryActorTick.bCanEverTick = true;

	UseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("UseSphere"));
	UseSphere->SetupAttachment(RootComponent);
}

void ACandle::Multicast_Light_Implementation() {
	Light_BP();
}


void ACandle::HandleTinderBoxCollision() {
	if (!bBought) {
		return;
	}

	bool bHandledTinderBox = false;
	for (auto OverlappingTargetIt = OverlappingTargets.CreateIterator(); OverlappingTargetIt; ++OverlappingTargetIt) {
		if (OverlappingTargetIt->GetInterface()->Light()) {
			bHandledTinderBox = true;
			OverlappingTargetIt.RemoveCurrent();
		}
	}

	if (bHandledTinderBox) {
		bOnCooldown = true;

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] {
			bOnCooldown = false;
		}, GCandle_Default_Cooldown, false);
		
		Multicast_Light();
	}

	if (OverlappingTargets.IsEmpty()) {
		SetActorTickEnabled(false);
	}
}

void ACandle::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (TScriptInterface<IFuelTarget>(OtherActor)) {
		OverlappingTargets.AddUnique(OtherActor);
	}
	
	if (!OverlappingTargets.IsEmpty()) {
		SetActorTickEnabled(true);
	}
}

void ACandle::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (TScriptInterface<IFuelTarget>(OtherActor)) {
		OverlappingTargets.Remove(OtherActor);
	}

	if (OverlappingTargets.IsEmpty()) {
		SetActorTickEnabled(false);
	}
}

void ACandle::BeginPlay() {
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (HasAuthority()) {
		UseSphere->OnComponentBeginOverlap.AddDynamic(this, &ACandle::OnOverlapBegin);
		UseSphere->OnComponentEndOverlap.AddDynamic(this, &ACandle::OnOverlapEnd);
	}
}

void ACandle::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!bOnCooldown) {
		HandleTinderBoxCollision();
	}
}