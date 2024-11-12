// Copyright 2020-2023: Jesse J. van Vliet


#include "Quality.h"

EItemQuality IIQuality::GetQuality() {
	UE_LOG(LogTemp, Error, TEXT("GetQuality() called on (%s)."), *this->_getUObject()->GetName());
	return EItemQuality::None;
}

bool IIQuality::HasQuality() const {
	UE_LOG(LogTemp, Error, TEXT("HasQuality() called on (%s)."), *this->_getUObject()->GetName());
	return false;
}


EItemQuality IIQuality::DetermineQuality(const FQualityDistribution& Distribution) const {
	//UE_LOG(LogTemp, Log, TEXT("Generated Quality for (%s). Distribution: ([Ad: %f], [Ar %f], [El: %f])"), *this->_getUObject()->GetName(), Distribution.Adept, Distribution.Arcane, Distribution.Eldar);
	const float RandomValue = FMath::RandRange(0.f, 1.f);

	if (RandomValue < Distribution.Adept) {
		//UE_LOG(LogTemp, Log, TEXT("Generated Quality for (%s). Quality: Adept"), *this->_getUObject()->GetName());
		return EItemQuality::Adept;
	}

	if (RandomValue < Distribution.Adept + Distribution.Arcane) {
		//UE_LOG(LogTemp, Log, TEXT("Generated Quality for (%s). Quality: Arcane"), *this->_getUObject()->GetName());
		return EItemQuality::Arcane;
	}

	//UE_LOG(LogTemp, Log, TEXT("Generated Quality for (%s). Quality: Eldar"), *this->_getUObject()->GetName());
	return EItemQuality::Eldar;
}

EItemQuality IIQuality::DetermineQuality(const int32 FarmLevel) const {
	return DetermineQuality(FQualityDistribution(FarmLevel));
}

EItemQuality IIQuality::DetermineQualityWithModifier(const int32 FarmLevel, const float Modifier) const {
	return DetermineQuality(FQualityDistribution(FMath::CeilToInt(FarmLevel * Modifier)));
}