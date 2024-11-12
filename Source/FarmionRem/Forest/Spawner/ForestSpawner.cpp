// Copyright 2020-2023: Jesse J. van Vliet


#include "ForestSpawner.h"

#include "Mushroom.h"

void AForestSpawner::PostPerformCycle() {
	const bool bCappedSpawns = !FMath::IsNearlyEqual(MaxSpawns, -1.f);

	float TotalWeight = 0.0f;
	for (const auto& Pair : SpawnableActorClasses) {
		TotalWeight += Pair.Value;
	}

	int32 CurrentSpawns = 0;
	for (const FTransform& SpawnTransform : SpawnTransforms) {
		if (FMath::FRand() > SpawnChance) {
			continue;
		}

		const float RandomValue = FMath::FRand() * TotalWeight;

		float AccumulatedWeight = 0.0f;
		for (const auto& Pair : SpawnableActorClasses) {
			if ((AccumulatedWeight += Pair.Value) >= RandomValue) {
				GetWorld()->SpawnActor<AInteractableObject>(Pair.Key, SpawnTransform.GetLocation(), SpawnTransform.Rotator());
				break;
			}
		}

		if (bCappedSpawns && ++CurrentSpawns >= MaxSpawns) {
			break;
		}
	}
}