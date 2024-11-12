// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "InteractableObject.h"
#include "FarmionRem/Farm/Liquids.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "FarmionRem/Interfaces/Interactable/Ingredient.h"
#include "FarmionRem/Interfaces/Interactable/Sellable.h"
#include "Product.generated.h"

enum class EDecayFlags : uint8;

UCLASS(Blueprintable)
class FARMIONREM_API AProduct : public AInteractableObject, public IIngredient, public ISellable, public ICycleAffected {
	GENERATED_BODY()
public:
	AProduct();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProductName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EProductColor> ProductColor = EProductColor::EProductColor_White;
	
protected:
	virtual void Sell() override;
	
	virtual void PerformCycle() override;
	virtual EDecayFlags GetDecayFlag() override;

	virtual FText GetInformationText_Implementation() const override;
	virtual FText GetMiniInformationText_Implementation() const override;
	
	virtual int32 GetPriority() const override;
};