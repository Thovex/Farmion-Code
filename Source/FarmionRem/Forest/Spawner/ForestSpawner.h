// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "GameFramework/Actor.h"
#include "ForestSpawner.generated.h"

class AInteractableObject;

UCLASS(Blueprintable)
class FARMIONREM_API AForestSpawner : public AActor, public ICycleAffected {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AInteractableObject>, float> SpawnableActorClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTransform> SpawnTransforms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpawns = -1.f;
	
protected:
	virtual void PostPerformCycle() override;

};