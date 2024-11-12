// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Farm/Tools/SellContainer.h"
#include "FarmionRem/Farm/Buffs.h"
#include "GameFramework/GameState.h"
#include "FarmionGameState.generated.h"

class AFarmionPlayerState;
class AFarmionCharacter;

class ISellable;
class ISellContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateNameplates);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRenovateHouseMode, bool, Toggled);

/**
 * 
 */
UCLASS(Blueprintable)
class FARMIONREM_API AFarmionGameState : public AGameState {
	GENERATED_BODY()

public:
	AFarmionGameState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Day = 0;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite)
	FUpdateNameplates OnUpdateNameplates;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_FarmExperience)
	int32 FarmExperience = 1;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_FarmExperience();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_BonusFarmLevel)
	int32 BonusFarmLevel = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_BonusFarmLevel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Buffs)
	TArray<FBuff> Buffs;
	TArray<float> BuffsDurations;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Buffs();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Money)
	int32 Money;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Money();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 CycleDeathCount = 0;

	void Sell(const TScriptInterface<ISellable> Sellable, TSoftObjectPtr<ASellContainer> SellContainer);

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_LoadScreen();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void LoadScreenBP();

	UFUNCTION(BlueprintCallable)
	AFarmionCharacter* FindCharacterFromPlayerId(int32 PlayerID);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ReceiveMessage_Tunnel(AFarmionCharacter* Character, const FText& Sender, const FText& Msg);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ReceiveSystemMessage_Tunnel(const FText& Msg);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ReceiveNarrator_Tunnel(const FText& Msg, const float& Duration);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ReceiveEventMessage_Tunnel(const FText& Msg);

	void AddBuff(FBuff& Buff);

	float GetBuffValue(const float BaseValue, const EBuffType& BuffType, bool& HasTypedBuff) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float FlatMovementSpeedBonus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float PercentageMovementSpeedBonus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_RenovateHouseMode)
	bool bRenovateHouseMode = false;

	UFUNCTION()
	void OnRep_RenovateHouseMode();
	
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite)
	FRenovateHouseMode OnRenovateHouseMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* LevelCurve;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 CalculateFarmLevel();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

private:
	int32 BuffIdGen = 0;
	void EvaluateBuff(const FBuff& Buff, bool bAdditive);

};
