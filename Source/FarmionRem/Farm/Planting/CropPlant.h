// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interfaces/Hover/Information.h"
#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"
#include "FarmionRem/Io/SaveLoad.h"
#include "GameFramework/Actor.h"
#include "CropPlant.generated.h"

class ABeehive;
class ASingleCropPlot;
class UBoxComponent;
class AFarmLand;
class AInteractableProductSeed;

enum class ESeedStage : uint8;

UCLASS(Blueprintable)
class FARMIONREM_API ACropPlant : public AActor, public ILiquidTarget, public IInformation, public ISaveLoad {
	GENERATED_BODY()

public:
	ACropPlant();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PestProbability = 0; // 0 - 100 % per Cycle
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* WaterCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SoilMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ProductsRemaining = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_ProductsTaken)
	int32 ProductsTaken = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_ProductsTaken();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInteractableProductSeed> Product;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* SoilMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* WetSoilMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SurvivesDaysWithoutWater = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Water)
	bool bWatered = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Water();

	UFUNCTION(BlueprintCallable)
	AInteractableProductSeed* CreateProduct(USceneComponent* TransformComp);

	UFUNCTION(Server, Reliable)
	void PickupProduct(const AInteractableProductSeed* Seed); 

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnSeedStage();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnBlossomStage();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnHarvestStage();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnDeathStage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_KillPlant();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnKillPlant();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 TotalHealth = 10; // 1 attack = 1 health 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SeedStage)
	ESeedStage SeedStage;

	UFUNCTION()
	void OnRep_SeedStage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Days)
	int32 Days = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Days();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HoneyDays = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalDays = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSurvivesWinter = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_BeeAffected)
	bool bBeeAffected;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_BeeAffected();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_CollisionToggle)
	bool bCollisionToggle = false;

	UFUNCTION()
	void OnRep_CollisionToggle();

	UFUNCTION(BlueprintCallable)
	void SetActivesByStage(const ESeedStage InStage,
		USceneComponent* SeedParent,
		USceneComponent* BlossomParent,
		USceneComponent* DeathParent);

	TSoftObjectPtr<AFarmLand> FarmLandPtr;
	TSoftObjectPtr<ASingleCropPlot> SingleCropPlotPtr;
	TSoftObjectPtr<ABeehive> BeehivePtr;

	TArray<AInteractableProductSeed*> Products;

	void Attack(const int32 Damage);
	void DestroyAllRemainingProducts();
	
	bool bPendingKill = false;
protected:
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual bool Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) override;

};