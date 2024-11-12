// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "BuffMuncher.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API ABuffMuncher : public AUpgrade {
	GENERATED_BODY()

public:
	ABuffMuncher();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* MunchInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Mouth;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Chomp();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Chomp_BP();
	
protected:
	UFUNCTION()
	void OnMunchInputOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	virtual FText GetInformationText_Implementation() const override;
	virtual FText GetMiniInformationText_Implementation() const override;
};