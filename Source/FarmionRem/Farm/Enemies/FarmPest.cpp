// Copyright 2020-2023: Jesse J. van Vliet


#include "FarmPest.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Util/CommonLibrary.h"

void AFarmPest::Attack() {
	if (Target.IsValid()) {
		Target->Attack(Damage);
	} else {
		GetWorld()->GetTimerManager().ClearTimer(AttackTimer);

		bDead = true;
		OnRep_Death();
	}
}

int32 AFarmPest::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	const int32 Callback = Super::Interact(InteractInstigator, InteractionHit);
	if (UCommonLibrary::HasFlag(Callback, EInteractCallback::CARRYABLE_CAN_BE_CARRIED)) {
		Target = nullptr;
	}

	return Callback;
}

void AFarmPest::ApplyPestTo(ACropPlant* Plant) {
	TArray<UActorComponent*> PestSockets = Plant->GetComponentsByTag(USceneComponent::StaticClass(), GPest_Socket_Tag_Name);
	this->AttachToComponent(static_cast<USceneComponent*>(PestSockets[FMath::RandRange(0, PestSockets.Num() - 1)]), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Target = Plant;

	GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AFarmPest::Attack, AttackRateInSeconds, true, AttackRateInSeconds);
}