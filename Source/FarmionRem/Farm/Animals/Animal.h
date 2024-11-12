// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "FarmionRem/Interfaces/Interactable/Ingredient.h"
#include "Animal.generated.h"

class AProduct;

USTRUCT(BlueprintType)
struct FAnimalAnimationData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequence* Sequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLooping = false;
	
};

UCLASS(Blueprintable)
class FARMIONREM_API AAnimal : public AInteractableObject, public IIngredient, public ICycleAffected {
	GENERATED_BODY()

public:
	AAnimal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AnimalName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* MouthTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* AnimalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FAnimalAnimationData> Animations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DurationPerUnitDistance = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UCurveFloat* MovementCurve;

	void SetNewTargetLocation(FVector NewTargetLocation);

	UFUNCTION(NetMulticast, Reliable)
	void TransitionState(FName AnimationName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeInFieldTillHarvestable = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bHarvestable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TSubclassOf<AProduct> ProductFed;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Chomp();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Chomp_BP();

	virtual void PerformCycle() override;

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual bool IsBoilable_Implementation() const override;
	virtual bool IsFryable_Implementation() const override;
	virtual bool IsSpittable_Implementation() const override;

	virtual EDecayFlags GetDecayFlag() override;

	float CurrentTimeInField = 0;
private:
	FVector CurrentTargetLocation;

	float StartTime;
	float Duration;
	FVector StartLocation;
	
	FTimerHandle MovementTimerHandle;
	void UpdateTransform();

};