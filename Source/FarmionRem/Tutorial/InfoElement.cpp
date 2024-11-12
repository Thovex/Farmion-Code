// Copyright 2020-2023: Jesse J. van Vliet


#include "InfoElement.h"

#include "Components/SphereComponent.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AInfoElement::AInfoElement() {
	PrimaryActorTick.bCanEverTick = true;

	CollisionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionTrigger"));
	CollisionTrigger->SetupAttachment(Transform);
}

void AInfoElement::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (AFarmionCharacter* Character = Cast<AFarmionCharacter>(OtherActor)) {
		OverlappingCharacters.AddUnique(Character);
		DetermineFollowCharacter();

		SetActorTickEnabled(true);
	}
}

void AInfoElement::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (AFarmionCharacter* Character = Cast<AFarmionCharacter>(OtherActor)) {
		OverlappingCharacters.Remove(Character);
		DetermineFollowCharacter();

		if (OverlappingCharacters.IsEmpty()) {
			SetActorTickEnabled(false);
		}
	}
}

void AInfoElement::BeginPlay() {
	Super::BeginPlay();

	if (CollisionTrigger) {
		CollisionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AInfoElement::OnOverlapBegin);
		CollisionTrigger->OnComponentEndOverlap.AddDynamic(this, &AInfoElement::OnOverlapEnd);
	} else {
		UE_LOG(LogTemp, Error, TEXT("AInfoElement::BeginPlay: CollisionTrigger is nullptr!"));
	}

	SetActorTickEnabled(false);
}

void AInfoElement::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (FollowCharacter) {
		const FVector& ActorLocation = GetActorLocation();
		const FVector& TargetLocation = FollowCharacter->GetActorLocation() + FVector(0.0f, 0.0f, 176.0f / 2); // 1/2 HeightOf(Jesse).Y;

		// Find Look At Rotation
		const FRotator CurrentRotation = GetActorRotation();
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
		
		SetActorRotation(FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaSeconds, 5.0f));
	}
}

int32 AInfoElement::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return 0;
}

int32 AInfoElement::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return 0;
}

void AInfoElement::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("Radius", CollisionTrigger->GetScaledSphereRadius());
}

void AInfoElement::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	CollisionTrigger->SetSphereRadius(JsonObject->GetNumberField("Radius"));
}

void AInfoElement::DetermineFollowCharacter() {
	FollowCharacter = nullptr;

	if (!OverlappingCharacters.IsEmpty()) {
		const APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		for (AFarmionCharacter* Character : OverlappingCharacters) {
			if (LocalPawn == Character) {
				FollowCharacter = Character;
				break;
			}
		}

		if (!FollowCharacter) {
			FollowCharacter = OverlappingCharacters[FMath::RandRange(0, OverlappingCharacters.Num() - 1)];
		}

		SetActive();
	} else {
		SetInactive();
	}
}