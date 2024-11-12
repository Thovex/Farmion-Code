// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/Tool.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"
#include "GrowthPad.generated.h"

class AUpgrade;
class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AGrowthPad : public ATool, public ICycleAffected, public ILiquidTarget {
	GENERATED_BODY()

public:
	AGrowthPad();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ContainerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* WaterInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* SoilMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* WetSoilMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Water)
	bool bWatered = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Water();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AInteractableObject>, float> SpawnableActorClasses;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
	virtual bool Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) override;
	virtual void PostPerformCycle() override;

	virtual void Link() override;

private:
	bool bHasContext = false;
	TSoftObjectPtr<AUpgrade> LinkerContext;
	
};