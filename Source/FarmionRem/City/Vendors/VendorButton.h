// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interfaces/Interactable/Buyable.h"
#include "VendorButton.generated.h"

class ULinkerComponent;
class AStorageUnit;

UCLASS(Blueprintable)
class FARMIONREM_API AVendorButton : public AInteractableObject, public IBuyable {
	GENERATED_BODY()

public:
	AVendorButton();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInteractableObject> InteractableToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* FallbackSpawnLocation; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BuyPrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Text;

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multicast_Purchased(); // Aesthetics event

protected:
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;

	virtual void Buy() override;
	virtual int32 GetBuyPrice_Implementation() const override;
	
	virtual FText GetInformationText_Implementation() const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
private:
	virtual void Link() override;

	TSoftObjectPtr<AStorageUnit> SpawnStorage;

};