// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Product.h"
#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"
#include "GameFramework/Actor.h"
#include "Flask.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AFlask : public AProduct, public ILiquidTarget {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Charges)
	int32 Charges = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Charges();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 MaxCharges = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Liquid)
	TEnumAsByte<ELiquids> Liquid = ELiquids::ELiquids_None;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Liquid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Color)
	TEnumAsByte<EProductColor> Color = EProductColor::EProductColor_White;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Color();

protected:
	virtual void BeginPlay() override;
	
	virtual bool Supply(TEnumAsByte<ELiquids> SuppliedLiquid, TEnumAsByte<EProductColor> SuppliedColor, int32 SuppliedCharges) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};