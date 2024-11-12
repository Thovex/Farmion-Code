// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "GameFramework/PhysicsVolume.h"
#include "IgnoreDecayVolume.generated.h"

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EDecayFlags : uint8 {
	None = 0,
	Player = 1 << 0,
	Produce = 1 << 1,
	Seed = 1 << 2,
	Animal = 1 << 3,
	Other = 1 << 4,
};

ENUM_CLASS_FLAGS(EDecayFlags);

UCLASS(Blueprintable)
class FARMIONREM_API AIgnoreDecayVolume : public APhysicsVolume {
	GENERATED_BODY()

public:
	AIgnoreDecayVolume();

	UPROPERTY(EditAnywhere,  meta=(Bitmask, BitmaskEnum = "/Script/FarmionRem.EDecayFlags"))
	uint8 Flags;

protected:
	virtual void BeginPlay() override;
	
	// virtual void ActorEnteredVolume(AActor* Other) override;
	// virtual void ActorLeavingVolume(AActor* Other) override;

	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);


};