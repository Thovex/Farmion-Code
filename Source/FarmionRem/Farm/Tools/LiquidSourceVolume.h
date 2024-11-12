// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Engine/TriggerVolume.h"
#include "FarmionRem/Farm/Liquids.h"
#include "FarmionRem/Interfaces/Liquid/LiquidSource.h"
#include "LiquidSourceVolume.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ALiquidSourceVolume : public ATriggerVolume, public ILiquidSource {
	GENERATED_BODY()

public:
	ALiquidSourceVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ELiquids> Liquid;

protected:
	//UFUNCTION()
	//void ActorEnteredVolume(AActor* OverlappedActor, AActor* OtherActor);

	//UFUNCTION()
	//void ActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor);
	
	virtual void BeginPlay() override;
	virtual TEnumAsByte<ELiquids> GetLiquidType() override;
};