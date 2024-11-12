// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Pest.h"
#include "FarmPest.generated.h"

UCLASS()
class FARMIONREM_API AFarmPest : public APest {
	GENERATED_BODY()

public:
	void ApplyPestTo(ACropPlant* Plant);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRateInSeconds = 1.f;

protected:
	virtual void Attack();
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	
private:
	FTimerHandle AttackTimer;
	TWeakObjectPtr<ACropPlant> Target;
};