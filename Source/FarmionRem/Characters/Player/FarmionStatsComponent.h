// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Components/ActorComponent.h"
#include "FarmionStatsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FARMIONREM_API UFarmionStatsComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UFarmionStatsComponent();

	TMap<FName, int32> Stats;

};