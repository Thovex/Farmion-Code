// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Product.h"
#include "FarmionRem/Interfaces/Fuel/FuelSource.h"
#include "Fuel.generated.h"

class IFuelTarget;
class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AFuel : public AProduct, public IFuelSource {
	GENERATED_BODY()

public:
	AFuel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* FuelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* FuelCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FuelValue = 1;

	virtual int32 GetFuelValue() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginFuelCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);
};