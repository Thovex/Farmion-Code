// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CropPlant.h"
#include "FarmionRem/Farm/Land/FarmLand.h"
#include "FarmionRem/Interactables/Tool.h"
#include "SingleCropPlot.generated.h"

enum class ESeed : uint8;
class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API ASingleCropPlot : public ATool, public ICycleAffected {
	GENERATED_BODY()

public:
	ASingleCropPlot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PlotInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PlotMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* PlantTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* GrowthTable;

	UPROPERTY(ReplicatedUsing=OnRep_Seed, EditAnywhere, BlueprintReadWrite)
	ESeed Seed;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Seed();
	
	virtual void FinishPlant(const ACropPlant* Plant);
	TSoftObjectPtr<ACropPlant> GetPlant();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESeed StartAcceptedSeedsEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESeed EndAcceptedSeedsEnum;

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PerformCycle() override;
	
	UFUNCTION(Server, Reliable)
	virtual void Server_AddSeed(const ASeed* InSeed);

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
	virtual FText GetInformationText_Implementation() const override;

	UFUNCTION()
	virtual void OverlapPlot(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); 

	TArray<FFarmEntry> FarmEntries;
};