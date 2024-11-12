// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "InteractableObject.h"
#include "Money.generated.h"

class USpringArmComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AMoney : public AInteractableObject {
	GENERATED_BODY()

public:
	AMoney();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* MoneyText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Money)
	int32 Money;

	UFUNCTION()
	void OnRep_Money();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Server_Consume() override;
	virtual void Multicast_Consume() override;

	virtual int32 GetPriority() const override;

	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};