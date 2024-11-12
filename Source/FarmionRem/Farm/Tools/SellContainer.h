// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "FarmionRem/Interfaces/Interactable/Sellable.h"
#include "SellContainer.generated.h"

class AMoney;

USTRUCT(BlueprintType)
struct FMoneyQueueItem {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MonetaryValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SpawnMoney = true;
};

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ESellMethodCallback : uint8 {
	None = 0,
	Immediate = 1 << 0,
	NextDay = 1 << 1,
	Add = 1 << 2,
	SpawnMoney = 1 << 3,
};
ENUM_CLASS_FLAGS(ESellMethodCallback);

UCLASS(Blueprintable)
class FARMIONREM_API ASellContainer : public AInteractableObject, public ICycleAffected {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMoney> MoneyClass;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UShapeComponent* GetCollisionShape();

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	
	virtual void PerformCycle() override;
	virtual bool Sell(TScriptInterface<ISellable> Sellable);
	
	// If -1: Crash, If 0: Unlimited, Else: Limited.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	int32 GetMaxSoldItemsPerDay();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	int32 GetSellMethod();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	TArray<FTransform> GetSpawnPoints();

	int32 GetCurrentSoldItemsToday() const;

	void IncrementSoldItems(int32 SellValue);
	void ResetSoldItems();

	bool CanSellItem();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnSellItem(int32 SellValue);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnSellItem(int32 SellValue);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SellAvailability)
	bool SellAvailability;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnRep_SellAvailability();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	int32 MaxSoldItemsPerDay = -1; // 0 = unlimited
	int32 CurrentSoldItemsToday = 0;
	
private:
	TQueue<FMoneyQueueItem> MoneyQueue;
	void SpawnMoney(const int32& MonetaryValue);
	void AddMoney(const int32& MonetaryValue);
};