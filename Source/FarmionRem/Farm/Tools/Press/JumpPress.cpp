// Copyright 2020-2023: Jesse J. van Vliet


#include "JumpPress.h"

#include "Components/SphereComponent.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"

AJumpPress::AJumpPress() {
	PrimaryActorTick.bCanEverTick = true;

	PressArea = CreateDefaultSubobject<USphereComponent>(TEXT("PressArea"));
	PressArea->SetupAttachment(RootComponent);
}

void AJumpPress::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA<AFarmionCharacter>()) {
		CharactersInPressArea.Add(Cast<AFarmionCharacter>(OtherActor));
		SetActorTickEnabled(true);
	}
}

void AJumpPress::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA<AFarmionCharacter>()) {
		CharactersInPressArea.Remove(Cast<AFarmionCharacter>(OtherActor));

		if (CharactersInPressArea.IsEmpty()) {
			SetActorTickEnabled(false);
		}
	}
}

void AJumpPress::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		PressArea->OnComponentBeginOverlap.AddDynamic(this, &AJumpPress::OnOverlapBegin);
		PressArea->OnComponentEndOverlap.AddDynamic(this, &AJumpPress::OnOverlapEnd);
	}

	SetActorTickEnabled(false);
	
}

void AJumpPress::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (HasAuthority()) {
		for (AFarmionCharacter* Character : CharactersInPressArea) {
			if (Character->bJustLanded) {
				if (InputCount <= 0) {
					return;
				}
				
				Character->bJustLanded = false;

				GAME_STATE

				bool bHasBuff;
				float BuffPower = GameState->GetBuffValue(1.0f, EBuffType::JumpHeight, bHasBuff);

				// TODO: Test BuffPower :P

				const float AdjustmentFactor = InputCountRange.Y / InputCount;

				Progress += PowerPerJump * AdjustmentFactor;
				Progress = FMath::Clamp(Progress, ProgressRange.X, ProgressRange.Y);
				
				OnRep_Progress();
			}
		}
	}
}
