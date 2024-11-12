// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "Petto.generated.h"

class USpringArmComponent;

UCLASS(Blueprintable)
class FARMIONREM_API APetto : public AInteractableObject {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CarryOffset = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CarryRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CarryLength = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BindSocket = NAME_None;

	UPROPERTY(ReplicatedUsing=OnRep_NewOwner)
	AFarmionCharacter* OwningCharacter;

	UFUNCTION()
	void OnRep_NewOwner();

	UPROPERTY()
	FTransform SpawnTransform;

protected:
	virtual void BeginPlay() override;
	
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	
	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;
	
	virtual bool CanCompost_Implementation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
