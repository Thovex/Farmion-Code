// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "Quality.generated.h"

UENUM(BlueprintType)
enum class EItemQuality : uint8 {
	None	= 0 UMETA(DisplayName = "None"),
	Adept	= 1 UMETA(DisplayName = "Adept"),
	Arcane  = 2 UMETA(DisplayName = "Arcane"),
	Eldar	= 3 UMETA(DisplayName = "Eldar"),
};

USTRUCT(BlueprintType)
struct FQualityDistribution
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	float Adept;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	float Arcane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	float Eldar;

	FQualityDistribution(): Adept(0), Arcane(0), Eldar(0) {}
	FQualityDistribution(const float Adept, const float Arcane, const float Eldar): Adept(Adept), Arcane(Arcane), Eldar(Eldar) {}

	explicit FQualityDistribution(const int32 FarmLevel) {
		const FQualityDistribution StartDistribution(0.97f, 0.02f, 0.01f);
		const FQualityDistribution EndDistribution(0.35f, 0.45f, 0.20f);
		
		this->Adept = FMath::Lerp(StartDistribution.Adept, EndDistribution.Adept, FMath::Clamp(FarmLevel, 1, 100) / 100.0f);
		this->Arcane = FMath::Lerp(StartDistribution.Arcane, EndDistribution.Arcane, FMath::Clamp(FarmLevel, 1, 100) / 100.0f);
		this->Eldar = FMath::Lerp(StartDistribution.Eldar, EndDistribution.Eldar, FMath::Clamp(FarmLevel, 1, 100) / 100.0f);
	}
};

UINTERFACE(Blueprintable)
class UIQuality : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API IIQuality {
	GENERATED_BODY()

public:
	virtual EItemQuality GetQuality();
	virtual bool HasQuality() const;
	
	virtual EItemQuality DetermineQuality(const FQualityDistribution& Distribution) const;
	virtual EItemQuality DetermineQuality(const int32 FarmExperience) const;
	virtual EItemQuality DetermineQualityWithModifier(const int32 FarmExperience, const float Modifier) const;
};