// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interfaces/Hover/Information.h"
#include "FarmionRem/Interfaces/Interactable/Carryable.h"
#include "FarmionRem/Interfaces/Interactable/Compostable.h"
#include "FarmionRem/Interfaces/Interactable/Interactable.h"
#include "FarmionRem/Interfaces/Interactable/Quality.h"

#include "FarmionRem/Io/Linked.h"
#include "FarmionRem/Io/SaveLoad.h"

#include "GameFramework/Actor.h"
#include "InteractableObject.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AInteractableObject : public AActor,
	public IInteractable, public ICarryable, public IIQuality, public IInformation, public ILinked, public ISaveLoad, public ICompostable {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Transform;
	
	AInteractableObject();

	// Handle only once.
	UFUNCTION(Server, Reliable)
	virtual void Server_Consume();

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Consume();

	UFUNCTION(Server, Reliable)
	virtual void Server_Use(AFarmionCharacter* User, bool bUse);

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Use(bool bUse);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SellVfx = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ConsumeVfx = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SpawnVfx = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EItemQuality Quality;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OneTimeMove(const TArray<FTransform>& Path);

	UPROPERTY(Replicated)
	bool bNetworkKill;
	
	bool bConsumed = false;

protected:
	virtual void BeginPlay() override;
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
 
	virtual void Destroyed() override; 

	virtual FText GetInformationText_Implementation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual EItemQuality GetQuality() override;
	virtual bool HasQuality() const override;
	
};

