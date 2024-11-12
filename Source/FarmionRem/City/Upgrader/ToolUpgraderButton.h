// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interfaces/Interactable/Buyable.h"
#include "ToolUpgraderButton.generated.h"

class AToolUpgrader;

UCLASS(Blueprintable)
class FARMIONREM_API AToolUpgraderButton : public AInteractableObject, public IBuyable {
	GENERATED_BODY()

public:
	AToolUpgraderButton();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Text;

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multicast_Purchased(); // Aesthetics event

protected:
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual void Buy() override;
	
	virtual FText GetInformationText_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanDowngrade = true;

private:
	virtual void Link() override;

	TSoftObjectPtr<AToolUpgrader> Upgrader;

};