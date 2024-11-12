// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Tool.h"
#include "ForageBag.generated.h"

class AProduct;
class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AForageBag : public ATool {
	GENERATED_BODY()

public:
	AForageBag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BagInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* BagPivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BagMesh;

	UFUNCTION(BlueprintCallable)
	void Extract(TArray<FVector> SpawnLocations);
	
	UFUNCTION(BlueprintCallable)
	void TryExtract();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeTillPop = 10.f;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartExtractionProcess();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Timer)
	float Timer;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Timer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_InputCount)
	int32 InputCount;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_InputCount();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bExplosive;

protected:
	UFUNCTION()
	void OnBagInputBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	TArray<TSharedPtr<FJsonObject>> Products;

	UFUNCTION()
	void UpdateTimer();

	virtual FText GetInformationText_Implementation() const override;

private:
	FTimerHandle TimerUpdaterHandle;
	FTimerHandle TimeTillPopHandle;
};