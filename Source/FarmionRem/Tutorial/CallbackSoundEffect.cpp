// Copyright 2020-2023: Jesse J. van Vliet


#include "CallbackSoundEffect.h"

#include "FarmionRem/FarmionGameState.h"
#include "Kismet/GameplayStatics.h"

ACallbackSoundEffect::ACallbackSoundEffect() {
	PrimaryActorTick.bCanEverTick = false;
}

void ACallbackSoundEffect::PlaySound() {
	const float Duration = Sounds[CurrentIndex]->Duration;
	
	if (bPlayAtLocation) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sounds[CurrentIndex],
			GetActorLocation(), GetActorRotation(),
			1.0f, 1.0f, 0.0f,
			nullptr, nullptr,
			this);
	} else {
		UGameplayStatics::PlaySound2D(GetWorld(), Sounds[CurrentIndex],
			1.0f, 1.0f, 0.0f,
			nullptr,
			this);
	}

	if (Narrations.IsValidIndex(CurrentIndex) && !Narrations[CurrentIndex].IsEmpty()) {
		GAME_STATE
		
		GameState->ReceiveNarrator_Tunnel(Narrations[CurrentIndex], Duration);
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [&] {
		Evaluate();
	}, Duration + DelayBetweenSounds, false);

}

void ACallbackSoundEffect::BeginPlay() {
	Super::BeginPlay();
	PlaySound();
}

void ACallbackSoundEffect::Evaluate() {
	if (CurrentIndex + 1 < Sounds.Num()) {
		CurrentIndex++;
		PlaySound();
	} else {
		if (DelayTillCallback > 0.f) {
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [&] {
				Expire();
			}, DelayTillCallback, false);
		} else {
			Expire();
		}
	}
}

void ACallbackSoundEffect::Expire() {
	OnEnded.Broadcast();
	SetLifeSpan(1.f);
}