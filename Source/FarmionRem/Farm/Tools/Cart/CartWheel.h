// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Tool.h"
#include "CartWheel.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ACartWheel : public ATool {
	GENERATED_BODY()

public:
	ACartWheel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WheelMesh;

};