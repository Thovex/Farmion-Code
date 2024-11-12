// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "InteractableObject.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "Seed.generated.h"

class USphereComponent;
class AFarmLand;
class ACropPlant;
class AProduct;

enum class EDecayFlags : uint8;

UENUM(BlueprintType)
enum class ESeed : uint8 {
	None = 0,
	DO_NOT_USE_CROP_START = 1,
	Carrot = 2,
	Potato = 3,
	Pumpkin = 4,
	Lettuce = 5,
	Barley = 6,
	Watermelon = 7,
	DO_NOT_USE_CROP_END = 9,
	DO_NOT_USE_FLOWER_START = 10,
	Rose = 11,
	DO_NOT_USE_FLOWER_END = 19,
	DO_NOT_USE_BUSH_START = 20,
	Blueberry = 21,
	Strawberry = 22,
	DO_NOT_USE_BUSH_END = 29,
	DO_NOT_USE_TRELLIS_START = 30,
	Grape = 31,
	DO_NOT_USE_TRELLIS_END = 39
};

static FORCEINLINE FString GetSeedDisplayName(ESeed Seed) {
	switch (Seed) {
		case ESeed::None:
			return "None";
		case ESeed::Carrot:
			return "Magic-Rooty";
		case ESeed::Potato:
			return "Earthy-Tuboo";
		case ESeed::Pumpkin:
			return "Charm-O-Lantern";
		case ESeed::Lettuce:
			return "Leafy-Wizleaf";
		case ESeed::Barley:
			return "Mystic-Grain";
		case ESeed::Watermelon:
			return "Moize-Maloon";
		case ESeed::Rose:
			return "Enchanti-Bloom";
		case ESeed::Blueberry:
			return "Mystic-BerryBoo";
		case ESeed::Strawberry:
			return "Charm-Fruity";
		case ESeed::Grape:
			return "Magi-Magi";
		default:
			return "Unknown";
	}
}

UENUM(BlueprintType)
enum class ESeedStage : uint8 {
	None = 0,
	SeedStage = 1 UMETA(DisplayName = "Seed-Sproutik"),
	BlossomStage = 2 UMETA(DisplayName = "Bloomi-Magica"),
	HarvestStage = 3 UMETA(DisplayName = "Reap-Ready"),
	DeathStage = 4 UMETA(DisplayName = "Not-Reapth"),
};

static FORCEINLINE FString GetSeedStageDisplayName(ESeedStage Stage) {
	switch (Stage) {
	case ESeedStage::None:
		return "None";
	case ESeedStage::SeedStage:
		return "Seed-Sproutik";
	case ESeedStage::BlossomStage:
		return "Bloomi-Magica";
	case ESeedStage::HarvestStage:
		return "Reap-Ready";
	case ESeedStage::DeathStage:
		return "Not-Reapth";
	default:
		return "Unknown";
	}
}

UCLASS(Blueprintable)
class FARMIONREM_API ASeed : public AInteractableObject, public ICycleAffected {
	GENERATED_BODY()

public:
	ASeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Pivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SeedBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* SeedCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GrowthTableId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProduct> Product;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESeed SeedType;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SeedError();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Multicast_SeedError_BP();

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void PerformCycle() override;

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 MeetsDropRequirements(const AFarmionCharacter* Character) override;

	virtual EDecayFlags GetDecayFlag() override;

	virtual FText GetInformationText_Implementation() const override;
	virtual FText GetMiniInformationText_Implementation() const override;

private:
	void ResetFarmLandCollisionCheck();
	bool FarmLandCollisionCheck();

	FVector CurrentFarmCollidingPos;
	TSoftObjectPtr<AFarmLand> FarmLandPtr;

	TScriptInterface<ICarryable> ThisAsCarryable;
};