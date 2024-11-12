// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Planting/Seeds/InteractableProductSeed.h"
#include "Flower.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AFlower : public AInteractableProductSeed {
	GENERATED_BODY()

protected:
	virtual FText GetInformationText_Implementation() const override;

};