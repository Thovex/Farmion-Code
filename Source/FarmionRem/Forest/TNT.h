// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CoreMinimal.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interfaces/Interactable/Buyable.h"
#include "TNT.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ATNT : public AInteractableObject, public IBuyable {
	GENERATED_BODY()

public:
	ATNT();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ChaosActorToSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Wall;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Pulled)
	bool bPulled = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Pulled();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Bought)
	bool bBought = false;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnRep_Bought();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Explode();

protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;

	virtual void Buy() override;
	virtual int32 GetBuyPrice_Implementation() const override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual ELoadMethod LoadMethod() override;
	
	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;

	virtual FText GetInformationText_Implementation() const override;

};