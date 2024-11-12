// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Cheese.h"
#include "FarmionRem/Interactables/Tool.h"
#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"
#include "CurdingTub.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ACurdingTub : public ATool, public ILiquidTarget, public ICycleAffected {
	GENERATED_BODY()

public:
	ACurdingTub();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* CheeseSpawnLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECheeseType> CheeseType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<ECheeseType>, TSubclassOf<ACheese>> CheeseSpawnMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TEnumAsByte<ELiquids> LiquidType;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_MilkCount)
	int32 MilkCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 RequiredMilkCount;	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_MilkCount();

protected:
	virtual bool Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) override;

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual FText GetInformationText_Implementation() const override;
	virtual void PostPerformCycle() override;

};