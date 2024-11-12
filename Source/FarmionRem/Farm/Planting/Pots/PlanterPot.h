// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Planting/SingleCropPlot.h"
#include "PlanterPot.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API APlanterPot : public ASingleCropPlot
{
	GENERATED_BODY()

public:

	APlanterPot();

	
	UPROPERTY(ReplicatedUsing=OnRep_Ground, EditAnywhere, BlueprintReadWrite)
	bool bGround = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Ground();

	virtual void FinishPlant(const ACropPlant* Plant) override;

protected:

	virtual void OverlapPlot(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override; 

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;
	
};
