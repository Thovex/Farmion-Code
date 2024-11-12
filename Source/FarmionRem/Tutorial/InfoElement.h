// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "InfoElement.generated.h"

class USphereComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AInfoElement : public AInteractableObject {
	GENERATED_BODY()

public:
	AInfoElement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* CollisionTrigger;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetActive();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetInactive();

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void DetermineFollowCharacter();

private:

	UPROPERTY()
	AFarmionCharacter* FollowCharacter;
	
	UPROPERTY()
	TArray<AFarmionCharacter*> OverlappingCharacters;

};