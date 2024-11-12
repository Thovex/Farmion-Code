// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CheeseAreaVolume.h"
#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "CheeseRack.generated.h"

enum class ECheeseAreaType : uint8;

UCLASS(Blueprintable)
class FARMIONREM_API ACheeseRack : public AStorageUnit, public ICycleAffected {
	GENERATED_BODY()

public:
	ECheeseAreaType AreaType;

	ACheeseRack();

protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;

	virtual void PerformCycle() override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;
};