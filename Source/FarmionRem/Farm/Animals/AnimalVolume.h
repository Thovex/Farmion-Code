// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Engine/TriggerVolume.h"
#include "AnimalVolume.generated.h"

class AAnimal;
class AAnimalPawn;

UCLASS(Blueprintable)
class FARMIONREM_API AAnimalVolume : public ATriggerVolume {
	GENERATED_BODY()

public:
	AAnimalVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AAnimalPawn> AnimalPawnClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinMoveDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxMoveDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinDistanceBetweenAnimals = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 MaxLocationAttempts = 10;

protected:
	UFUNCTION()
	void ActorEnteredVolume(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void ActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor);

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TMap<AAnimal*, AAnimalPawn*> AnimalToPawnMap;

	FTimerHandle MoveAnimalsTimerHandle;
	void MoveAnimals();
	void MoveAnimal(AAnimal* Animal, AAnimalPawn* AnimalPawn);
};