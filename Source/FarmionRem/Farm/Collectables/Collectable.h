// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/InteractableObject.h"
#include "Collectable.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ACollectable : public AInteractableObject {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CollectableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CollectableValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Collected)
	bool bCollected;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnRep_Collected();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Collected_BP();

protected:
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	
	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;
	virtual void Multicast_Use(bool bUse) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FText GetInformationText_Implementation() const override;
};