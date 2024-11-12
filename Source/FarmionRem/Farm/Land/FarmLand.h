// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Data/GrowthData.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "FarmionRem/Io/SaveLoad.h"
#include "GameFramework/Actor.h"
#include "FarmLand.generated.h"

class AFarmPest;
class ACropPlant;

USTRUCT()
struct FFarmData {
	GENERATED_BODY()

public:
	UPROPERTY()
	FString TableId;

	UPROPERTY()
	int32 Day = 0;
};

USTRUCT()
struct FFarmEntry {
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY()
	ACropPlant* Plant = nullptr;

	UPROPERTY()
	FFarmData Data = {};

	UPROPERTY()
	FGrowthDataRow Row = {};
};

UCLASS(Blueprintable)
class FARMIONREM_API AFarmLand : public AActor, public ISaveLoad, public ICycleAffected {
	GENERATED_BODY()

public:
	AFarmLand();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AFarmPest>> Pests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* GrowthTable;

	UFUNCTION(Server, Reliable)
	void AddSeed(FVector Location, FRotator Rotation, const FString& TableId);

	void FinishPlant(const ACropPlant* Plant);
	void WaterAll();

protected:
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void PerformCycle() override;

private:
	TArray<FFarmEntry> FarmEntries;

};