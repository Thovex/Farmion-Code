// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "GameFramework/PhysicsVolume.h"
#include "CheeseAreaVolume.generated.h"

UENUM(BlueprintType)
enum class ECheeseAreaType : uint8 {
	ECheeseAreaType_Outside = 0,
	ECheeseAreaType_Inside = 1,
	ECheeseAreaType_Cellar = 2,
};

static FORCEINLINE FString GetCheeseAreaTypeDisplayName(ECheeseAreaType AreaType) {
	switch (AreaType) {
	case ECheeseAreaType::ECheeseAreaType_Outside:
		return "Outside";
	case ECheeseAreaType::ECheeseAreaType_Inside:
		return "Inside";
	case ECheeseAreaType::ECheeseAreaType_Cellar:
		return "Cellar";
	default:
		return "Unknown";
	}
}

UCLASS(Blueprintable)
class FARMIONREM_API ACheeseAreaVolume : public APhysicsVolume {
	GENERATED_BODY()

public:
	ACheeseAreaVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECheeseAreaType AreaType;

protected:
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	virtual void BeginPlay() override;
};