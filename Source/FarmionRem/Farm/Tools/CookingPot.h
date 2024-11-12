// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Converter.h"
#include "FarmionRem/Interfaces/Fuel/FuelTarget.h"
#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"
#include "CookingPot.generated.h"

UCLASS(BlueprintType)
class FARMIONREM_API ACookingPot : public AConverter, public ILiquidTarget, public IFuelTarget {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Water();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Water)
	bool bWater;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Fuel();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Fuel)
	bool bFuel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Lit)
	bool bLit;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Lit();

protected:
	virtual void BeginPlay() override;
	virtual bool MatchesConditions() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;
	
	virtual void Output() override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual bool Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) override;
	virtual bool Supply(int32 FuelValue) override;
	virtual bool Light() override;

	virtual int32 GetPriority() const override;
};