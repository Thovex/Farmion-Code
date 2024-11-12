// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "ToolUpgrader.generated.h"

enum class ECanUpgradeToolCallback : int32 {
	NO_CALLBACK = 0,
	PLACE_TOOL_IN_SOCKET = 1 << 0,
	TOOL_AT_MAX_QUALITY = 1 << 1,
	CAN_UPGRADE = 1 << 2,
	UPGRADE_IN_PROGRESS = 1 << 3,
};

UENUM(BlueprintType)
enum class EUpgradeToolCallback : uint8 {
	SAME_QUALITY = 0,
	WORSE_QUALITY = 1,
	BETTER_QUALITY = 2,
};

UCLASS(Blueprintable)
class FARMIONREM_API AToolUpgrader : public AStorageUnit {
	GENERATED_BODY()

public:
	virtual int32 CanUpgradeTool();
	virtual void StartUpgrade(bool bCanDowngrade);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FName UpgradeToolName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EItemQuality UpgradeToolQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_IsUpgrading)
	bool bIsUpgrading;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_IsUpgrading();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpgradeTime = 3.f;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpgradeTool(EUpgradeToolCallback Callback);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Multicast_UpgradeTool_BP(EUpgradeToolCallback Callback);

protected:
	virtual void UpgradeTool();

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual void SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FText GetInformationText_Implementation() const override;

private:
	bool bSavedCanDowngrade;
	FTimerHandle UpgradeTimerHandle;

};