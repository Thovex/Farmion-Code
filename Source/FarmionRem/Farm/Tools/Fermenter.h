// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/Tool.h"
#include "Fermenter.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AFermenter : public ATool {
	GENERATED_BODY()

public:
	AFermenter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* FermenterInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Percentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentCapacity; // in "Bucket Scoops"
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCapacity; // in "Bucket Scoops"

protected:
	virtual void BeginPlay() override;
};