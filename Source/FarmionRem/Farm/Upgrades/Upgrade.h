// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "Upgrade.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpgradePurchased);

UCLASS(Blueprintable)
class FARMIONREM_API AUpgrade : public AActor, public IInformation, public IIQuality, public ISaveLoad, public ILinked {
	GENERATED_BODY()

public:
	AUpgrade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UpgradeName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Bought)
	bool bBought = false;

	UFUNCTION()
	virtual void OnRep_Bought();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Generic_Networked_Call();

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Generic_Networked_Call();

	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade")
	void OnUpgradePurchased();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EItemQuality Quality;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnUpgradePurchased OnUpgradePurchasedDelegate;

protected:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual EItemQuality GetQuality() override;
	virtual bool HasQuality() const override;
};