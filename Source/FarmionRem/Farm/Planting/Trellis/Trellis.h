// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Planting/SingleCropPlot.h"
#include "Trellis.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ATrellis : public ASingleCropPlot
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetTrellisActive(bool bActive);

	virtual void OnRep_Bought() override;

protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual void BeginPlay() override;
};
