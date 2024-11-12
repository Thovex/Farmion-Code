// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Product.h"
#include "Pest.generated.h"

class ACropPlant;

UCLASS(Blueprintable)
class FARMIONREM_API APest : public AProduct {
	GENERATED_BODY()

public:
	APest();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* PestMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* AliveAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Death)
	bool bDead = false;

	UFUNCTION()
	virtual void OnRep_Death();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void PerformCycle() override;

	virtual FText GetInformationText_Implementation() const override;


};