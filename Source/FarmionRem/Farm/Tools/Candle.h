// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Tool.h"
#include "Candle.generated.h"

class IFuelTarget;
class USphereComponent;

UCLASS(Blueprintable)
class FARMIONREM_API ACandle : public ATool {
	GENERATED_BODY()

public:
	ACandle();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* UseSphere;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Light();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Light_BP();

	
protected:
	UFUNCTION()
	void HandleTinderBoxCollision();
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	TArray<TScriptInterface<IFuelTarget>> OverlappingTargets;
	bool bOnCooldown = false;
};