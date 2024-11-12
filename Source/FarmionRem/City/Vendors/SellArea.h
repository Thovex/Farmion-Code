// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "SellArea.generated.h"

class AMoney;

class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API ASellArea : public AInteractableObject {
	GENERATED_BODY()

public:
	ASellArea();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AInteractableObject>> SellableClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* SellArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMoney> MoneyClass;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	TArray<FTransform> GetSpawnPoints();

protected:
	UFUNCTION()
	void SellAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SpawnMoney(const int32& MonetaryValue);

	virtual void BeginPlay() override;
};