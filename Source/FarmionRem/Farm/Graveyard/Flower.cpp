// Copyright 2020-2023: Jesse J. van Vliet

#include "Flower.h"

FText AFlower::GetInformationText_Implementation() const {
	FString Text = Super::GetInformationText_Implementation().ToString();
	Text += "\n<Sub>Plant at Active Graves</>";
	return FText::FromString(Text);
}