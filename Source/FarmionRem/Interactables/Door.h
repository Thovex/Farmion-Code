// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "InteractableObject.h"
#include "Door.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ADoor : public AInteractableObject {
	GENERATED_BODY()

public:
	ADoor();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Open)
	bool bOpen = false;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnRep_Open();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float OpenAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;

	TSoftObjectPtr<ADoor> ConnectedDoor;

protected:
	virtual void BeginPlay() override;
	
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Link() override;
};