// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "InteractableObject.h"
#include "FarmionRem/Interfaces/Interactable/Buyable.h"
#include "Tool.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ATool : public AInteractableObject, public IBuyable {
	GENERATED_BODY()
public:
	ATool();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BuyPrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ToolName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Bought)
	bool bBought = false;

	UFUNCTION()
	virtual void OnRep_Bought();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void Buy() override;
	virtual int32 GetBuyPrice_Implementation() const override;
	
	virtual FText GetInformationText_Implementation() const override;
	virtual FText GetMiniInformationText_Implementation() const override;
};