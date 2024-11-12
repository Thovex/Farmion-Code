// Copyright 2020-2023: Jesse J. van Vliet


#include "Ping.h"

#include "FarmionRem/GlobalValues.h"
#include "Net/UnrealNetwork.h"

APing::APing() {
	PrimaryActorTick.bCanEverTick = false;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	PingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PingMesh"));
	PingMesh->SetupAttachment(RootComponent);
	PingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;
}

void APing::OnRep_PrimaryColor_Implementation() {
	
}

void APing::Multicast_TransformToLocation_Implementation(const FTransform& T) {
	TargetTransform = T;
	GetWorldTimerManager().ClearTimer(AttachTimerHandle);
	GetWorldTimerManager().SetTimer(AttachTimerHandle, this, &APing::AttachCoroutine, GetWorld()->GetDeltaSeconds(), true);
}

void APing::AttachCoroutine() {
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	AttachElapsedTime += DeltaTime;
	const float Alpha = FMath::Clamp(AttachElapsedTime / GCarry_Grab_Interpolation_Duration, 0.0f, 1.0f);
	const float CurveValue = LerpCurve->GetFloatValue(Alpha);

	const FTransform& CurrentTransform = this->GetActorTransform();
	FTransform NewTransform;

	NewTransform.Blend(CurrentTransform, TargetTransform, CurveValue);

	this->SetActorTransform(NewTransform);

	if (NewTransform.Equals(TargetTransform, GCarry_Snap_Threshold)
		|| AttachElapsedTime >= GCarry_Grab_Interpolation_Duration) {
		GetWorldTimerManager().ClearTimer(AttachTimerHandle);
		AttachElapsedTime = 0.0f;
	}
}

void APing::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APing, PrimaryColor);
}