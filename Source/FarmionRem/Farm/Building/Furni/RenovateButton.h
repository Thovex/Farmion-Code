// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "RenovateButton.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ARenovateButton : public AInteractableObject {
	GENERATED_BODY()

public:
	ARenovateButton();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_RenovateActive)
	bool bRenovateActive = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_RenovateActive();

protected:
	virtual void BeginPlay() override;
	
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};