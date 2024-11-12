// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Tool.h"
#include "Saw.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ASaw : public ATool {
	GENERATED_BODY()

public:
	ASaw();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SawMesh;
};