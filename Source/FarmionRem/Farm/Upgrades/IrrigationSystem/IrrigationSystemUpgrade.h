// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "IrrigationSystemUpgrade.generated.h"

class AIgnoreDecayVolume;

class AIrrigationSystemLever;
class AIrrigationSystemWaterContainer;

class ULinkerComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AIrrigationSystemUpgrade : public AUpgrade {
	GENERATED_BODY()

public:
	AIrrigationSystemUpgrade(); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<AIgnoreDecayVolume> IgnoreDecayVolume;

	virtual void Multicast_Generic_Networked_Call() override;

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_Bought() override;
	
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void Link() override;

private:
	void SetVisibility();
	TSoftObjectPtr<AIrrigationSystemWaterContainer> WaterContainer;
	TSoftObjectPtr<AIrrigationSystemLever> Lever;
};