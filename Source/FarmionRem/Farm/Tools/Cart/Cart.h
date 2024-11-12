// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "Cart.generated.h"

class UBoxComponent;
class USphereComponent;
class AFarmionCharacter;

UCLASS(Blueprintable)
class FARMIONREM_API ACart : public AStorageUnit {
	GENERATED_BODY()

public:
	ACart();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* CartPivot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WheelR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WheelL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* CartCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* WheelInputCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	AFarmionCharacter* UserPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_InUse)
	bool bInUse;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_InUse();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InterpolationSpeed = 2.0f; // Adjust this to control the speed of the cart's movement

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceThreshold = 300.0f; // The distance threshold for the cart to start following the player

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed = 5.0f; // Adjust this to control the speed of the cart's rotation

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_WheelCount)
	int32 WheelCount = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_WheelCount();

protected:
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;

	virtual int32 GetPriority() const override;

	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;
	virtual void Multicast_Use(bool bUse) override;

	UFUNCTION()
	void CartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void WheelOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;
private:
	
	TArray<FVector> PastPositions;
	const int32 DelaySteps = 15; // How many steps behind the player the cart should be
};