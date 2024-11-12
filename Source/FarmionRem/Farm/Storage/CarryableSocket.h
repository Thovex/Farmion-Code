// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "CarryableSocket.generated.h"

class ICarryable;

USTRUCT(BlueprintType)
struct FCarryableSocketInfo {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Component = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Pivot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Available = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<ICarryable> Carryable = nullptr;

	void GetSocketBounds(FVector& Origin, FVector& BoundsExtent) const;
};

// This class does not need to be modified.
UINTERFACE()
class UCarryableSocket : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ICarryableSocket {
	GENERATED_BODY()

public:
	void OnBeginPlay();
	AActor* GetActorFromInterface();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	TArray<FCarryableSocketInfo> GetCarryableSocketInfo();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	UShapeComponent* GetSocketDispenserCollision();

	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable);
	virtual bool IsAllowedToPickFromSocket();

	virtual bool GetMatchingSocket(const FVector& OverridePos, FCarryableSocketInfo& OutSocketInfo, int32& OutIndex, const TScriptInterface<ICarryable>& Carryable);
	virtual bool GetMatchingSocket(FCarryableSocketInfo& OutSocketInfo, int32& OutIndex, const TScriptInterface<ICarryable>& Carryable);

	virtual bool TurnSocketedCollisionOff();

	UFUNCTION(NetMulticast, Reliable)
	virtual void SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability);
	
	TSoftObjectPtr<UShapeComponent> Dispenser;
	TArray<FCarryableSocketInfo> AvailableSockets;

	int32 GetSocketsInUse() const;

	virtual bool IgnoreBounds() const;
};


