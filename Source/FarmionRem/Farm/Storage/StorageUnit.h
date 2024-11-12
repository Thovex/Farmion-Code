// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CarryableSocket.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interactables/Tool.h"
#include "StorageUnit.generated.h"

class UBoxComponent;
enum class EDecayFlags : uint8;

USTRUCT(BlueprintType)
struct FClassToSocketMapperData {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<USceneComponent>> Components;
};


UCLASS(Blueprintable)
class FARMIONREM_API AStorageUnit : public ATool, public ICarryableSocket {
	GENERATED_BODY()

public:
	AStorageUnit();
	
	UFUNCTION(Reliable, Server)
	virtual void SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool OverrideCanCarry();

	UPROPERTY(EditAnywhere,  meta=(Bitmask, BitmaskEnum = "/Script/FarmionRem.EDecayFlags"))
	int8 ProtectionFlags;
	
	UPROPERTY(Replicated)
	int32 SocketsInUse;

	UFUNCTION(BlueprintCallable)
	TArray<FCarryableSocketInfo> GenerateDefaultSockets(TArray<USceneComponent*> Components);

	UFUNCTION(BlueprintCallable)
	TArray<FCarryableSocketInfo> GenerateFromArray(const TArray<UBoxComponent*>& Components);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AInteractableObject>, FClassToSocketMapperData> ClassToSocketMapper; // Not necessary to be used.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanCarryWhileFilled = false;

protected:
	virtual void BeginPlay() override;

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CollectIgnoreActors(TArray<AActor*>& IgnoreActors) const;
	virtual TArray<AActor*> ExtraIgnoreActors() const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual int32 GetPriority() const override;

};