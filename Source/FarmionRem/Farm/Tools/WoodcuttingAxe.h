// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Tool.h"
#include "WoodcuttingAxe.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AWoodcuttingAxe : public ATool {
	GENERATED_BODY()

public:
	AWoodcuttingAxe();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AxeMesh;
};