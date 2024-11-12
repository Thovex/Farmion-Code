// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interfaces/Interactable/Buyable.h"
#include "FarmionRem/Interfaces/Interactable/Sellable.h"
#include "UpgradeBook.generated.h"

class AUpgrade;

class ULinkerComponent;
class USpringArmComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AUpgradeBook : public AInteractableObject, public IBuyable, public ISellable {
	GENERATED_BODY()

public:
	AUpgradeBook();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* Text;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Bought)
	bool bBought = false;

	TSoftObjectPtr<AUpgrade> Upgrade;

	UFUNCTION()
	virtual void OnRep_Bought();
	
	virtual bool CanBuy(int32 Money) override;

	UFUNCTION(Server, Reliable)
	virtual void Buy() override;

	UFUNCTION(Server, Reliable)
	virtual void Sell() override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 BuyPrice;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 SellPrice;

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual int32 GetBuyPrice_Implementation() const override;
	virtual int32 GetFullSellPrice() const override;
	
private:
	UFUNCTION()
	virtual void Link() override;

};