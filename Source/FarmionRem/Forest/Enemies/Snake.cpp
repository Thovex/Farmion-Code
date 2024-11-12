// Copyright 2020-2023: Jesse J. van Vliet


#include "Snake.h"
#include "Components/SphereComponent.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "Kismet/KismetMathLibrary.h"


ASnake::ASnake() {
	PrimaryActorTick.bCanEverTick = true;
	
	WarnAudioSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WarnAudioSphere"));
	WarnAudioSphere->SetupAttachment(RootComponent);

	BiteSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BiteSphere"));
	BiteSphere->SetupAttachment(RootComponent);
}

void ASnake::Server_Bite_Implementation(AFarmionCharacter* Target) {
	Target->Server_Bite();
	Multicast_Bite(Target);
}

void ASnake::Multicast_Bite_Implementation(AFarmionCharacter* Target) {
	DoBite(Target);

	PestMesh->PlayAnimation(AttackAnimation, false);
	AttackAnimTimerHandle.Invalidate();

	GetWorldTimerManager().SetTimer(AttackAnimTimerHandle, [this] {
		if (!this) {
			return;
		}

		this->PestMesh->SetAnimation(this->bDead ? DeathAnimation : AliveAnimation);
	}, 1.f, false);
}

void ASnake::OnWarnAudio(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (AFarmionCharacter* FarmionCharacter = Cast<AFarmionCharacter>(OtherActor)) {
		Targets.AddUnique(FarmionCharacter);

		PlayWarnAudio();

		SetActorTickEnabled(true);
	}
}

void ASnake::OnStopWarnAudio(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (AFarmionCharacter* FarmionCharacter = Cast<AFarmionCharacter>(OtherActor)) {
		Targets.Remove(FarmionCharacter);

		if (Targets.IsEmpty()) {
			SetActorTickEnabled(false);
		}
	}
}

void ASnake::OnBite(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (AFarmionCharacter* FarmionCharacter = Cast<AFarmionCharacter>(OtherActor)) {
		Server_Bite(FarmionCharacter);
	}
}

void ASnake::BeginPlay() {
	Super::BeginPlay();

	WarnAudioSphere->OnComponentBeginOverlap.AddDynamic(this, &ASnake::OnWarnAudio);
	WarnAudioSphere->OnComponentEndOverlap.AddDynamic(this, &ASnake::OnStopWarnAudio);

	if (HasAuthority()) {
		BiteSphere->OnComponentBeginOverlap.AddDynamic(this, &ASnake::OnBite);
	}

	SetActorTickEnabled(false);
}

void ASnake::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!Targets.IsEmpty()) {
		const FVector ThisLocation = GetActorLocation();
		
		FVector TargetActorLocation = Targets[0]->GetActorLocation();
		TargetActorLocation.Z = ThisLocation.Z;
		
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), UKismetMathLibrary::FindLookAtRotation(ThisLocation, TargetActorLocation), GetWorld()->DeltaTimeSeconds, 10.f));
	} else {
		SetActorTickEnabled(false);
	}
}

void ASnake::OnRep_Death() {
	WarnAudioSphere->DestroyComponent();
	BiteSphere->DestroyComponent();

	GetWorldTimerManager().ClearTimer(AttackAnimTimerHandle);
		
	Super::OnRep_Death();
}

void ASnake::PerformCycle() {
	this->Destroy(true);
}