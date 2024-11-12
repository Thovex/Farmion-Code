// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CoreMinimal.h"
#include "FarmionRem/Interactables/Tool.h"
#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"
#include "IrrigationSystemWaterContainer.generated.h"

class AIrrigationSystemLever;
class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AIrrigationSystemWaterContainer : public AInteractableObject, public ILiquidTarget {
	GENERATED_BODY()

public:
	AIrrigationSystemWaterContainer();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* WaterInputCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_WaterCharges)
	int32 WaterCharges = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 RequiredWaterCharges = 8;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_WaterCharges();
	
protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual bool Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual ELoadMethod LoadMethod() override;

	virtual FText GetInformationText_Implementation() const override;
};