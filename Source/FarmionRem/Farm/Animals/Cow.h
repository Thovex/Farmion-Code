// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Animal.h"
#include "FarmionRem/Interfaces/Liquid/LiquidSource.h"
#include "Cow.generated.h"

UCLASS(BlueprintType)
class FARMIONREM_API ACow : public AAnimal, public ILiquidSource {
	GENERATED_BODY()

protected:
	virtual void UseSource() override;
	virtual bool IsSourceAvailable() override;

	virtual TEnumAsByte<ELiquids> GetLiquidType() override;
	virtual TEnumAsByte<EProductColor> GetLiquidColor() override;

	virtual FText GetInformationText_Implementation() const override;

};