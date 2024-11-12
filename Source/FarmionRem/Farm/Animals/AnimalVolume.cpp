// Copyright 2020-2023: Jesse J. van Vliet


#include "AnimalVolume.h"

#include "AIController.h"
#include "Animal.h"
#include "AnimalPawn.h"
#include "NavigationSystem.h"

AAnimalVolume::AAnimalVolume() {
	PrimaryActorTick.bCanEverTick = false;
}

void AAnimalVolume::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		OnActorBeginOverlap.AddDynamic(this, &AAnimalVolume::ActorEnteredVolume);
		OnActorEndOverlap.AddDynamic(this, &AAnimalVolume::ActorLeavingVolume);
	}

	GetWorld()->GetTimerManager().SetTimer(MoveAnimalsTimerHandle, this, &AAnimalVolume::MoveAnimals, 5.f, true, 5.f);
}

void AAnimalVolume::MoveAnimals() {
	if (AnimalToPawnMap.IsEmpty()) {
		return;
	}

	for (auto& [Animal, AnimalPawn] : AnimalToPawnMap) {
		MoveAnimal(Animal, AnimalPawn);
	}
}

void AAnimalVolume::MoveAnimal(AAnimal* Animal, AAnimalPawn* AnimalPawn) {
	FNavLocation NavLocation;
	bool bFoundValidLocation = false;
	int32 Attempts = 0;

	while (!bFoundValidLocation && Attempts < MaxLocationAttempts) {
		if (UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomPointInNavigableRadius(AnimalPawn->GetActorLocation(), 1000.f, NavLocation)) {
			bool bLocationValid = true;

			for (auto& [OtherAnimal, OtherAnimalPawn] : AnimalToPawnMap) {
				if (Animal != OtherAnimal && FVector::Dist(NavLocation.Location, OtherAnimalPawn->GetActorLocation()) < MinDistanceBetweenAnimals) {
					bLocationValid = false;
					break;
				}
			}

			if (bLocationValid) {
				bFoundValidLocation = true;
				AnimalPawn->SetActorLocation(NavLocation.Location);
				Animal->SetNewTargetLocation(NavLocation.Location);
			}
		}

		Attempts++;
	}

	if (!bFoundValidLocation) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to find a valid random point after %d attempts."), MaxLocationAttempts);
	}
}

void AAnimalVolume::ActorEnteredVolume(AActor* OverlappedActor, AActor* OtherActor) {
	if (Cast<AAnimal>(OtherActor)) {
		AAnimal* Animal = Cast<AAnimal>(OtherActor);
		AAnimalPawn* AnimalPawn = GetWorld()->SpawnActor<AAnimalPawn>(AnimalPawnClass, Animal->GetActorLocation(), Animal->GetActorRotation());
		AAIController* AIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass());
		AIController->Possess(AnimalPawn);
		
		if (AnimalToPawnMap.Contains(Animal)) {
			AnimalToPawnMap[Animal]->Destroy(true);
		}

		AnimalToPawnMap.Add(Animal, AnimalPawn);
	}
}

void AAnimalVolume::ActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor) {
	if (Cast<AAnimal>(OtherActor)) {
		const AAnimal* Animal = Cast<AAnimal>(OtherActor);

		if (AnimalToPawnMap.Contains(Animal)) {
			AnimalToPawnMap[Animal]->GetController()->Destroy(true);
			AnimalToPawnMap[Animal]->Destroy(true);
			
			AnimalToPawnMap.Remove(Animal);
		}
	}
}