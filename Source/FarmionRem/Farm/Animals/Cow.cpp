// Copyright 2020-2023: Jesse J. van Vliet

#include "Cow.h"

#include "FarmionRem/Interactables/Product.h"

void ACow::UseSource() {
	CurrentTimeInField = 0;
	bHarvestable = false;
}

bool ACow::IsSourceAvailable() {
	return bHarvestable;
}

TEnumAsByte<ELiquids> ACow::GetLiquidType() {
	return ELiquids::ELiquids_Milk;
}

TEnumAsByte<EProductColor> ACow::GetLiquidColor() {
	if (!ProductFed) {
		return EProductColor::EProductColor_White;
	}

	const AProduct* DefaultProduct = ProductFed.GetDefaultObject();
	return DefaultProduct->ProductColor;
}

FText ACow::GetInformationText_Implementation() const {
	return FText::FromString(FString::Printf(TEXT("%s\n<Sub>%hs, %hs</>"),
		*AnimalName.ToString(),
		ProductFed != nullptr ? "Fed" : "Hungry",
		bHarvestable ? "Harvestable" : "Moo...")
	);
}