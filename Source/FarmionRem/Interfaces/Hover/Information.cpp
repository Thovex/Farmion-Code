// Copyright 2020-2023: Jesse J. van Vliet


#include "Information.h"

#include "FarmionRem/Interfaces/Interactable/Quality.h"


bool IInformation::IsActive() {
	return true;
}

FText IInformation::QualityTransformText(const EItemQuality Quality, FText& Text) {
	FString NewSubString;

	switch (Quality) {
		case EItemQuality::Adept:
		{
			NewSubString = "<ProdAdept>";
		}
		break;
		case EItemQuality::Arcane:
		{
			NewSubString = "<ProdArcane>Arcane ";
		}
		break;
		case EItemQuality::Eldar:
		{
			NewSubString = "<ProdEldar>Eldar ";
		}
		break;
		default:
		{
			NewSubString = "<Prod>";
		}
		break;
	}

	Text = FText::FromString(Text.ToString().Replace(TEXT("<Prod>"), *NewSubString));
	return Text;
}