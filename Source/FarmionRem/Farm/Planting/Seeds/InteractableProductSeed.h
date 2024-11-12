// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Interactables/Product.h"
#include "InteractableProductSeed.generated.h"

enum class ESeedStage : uint8;

class AFarmLand;

UCLASS(Blueprintable)
class FARMIONREM_API AInteractableProductSeed : public AProduct {
	GENERATED_BODY()

public:
	AInteractableProductSeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Pivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SeedMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationAsSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* PickedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* UnPickedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Picked)
	bool bPicked;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Picked();

	TSoftObjectPtr<ACropPlant> CropPtr;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
};