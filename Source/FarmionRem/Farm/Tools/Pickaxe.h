// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Tool.h"
#include "Pickaxe.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API APickaxe : public ATool {
	GENERATED_BODY()

public:
	APickaxe();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PickaxeMesh;
};