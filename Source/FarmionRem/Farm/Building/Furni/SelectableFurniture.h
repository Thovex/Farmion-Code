// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "SelectableFurniture.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EFurniEasing : uint8 {
	Default = 0,
	RoofSpawn = 1,
	ThinObject = 2,
	SideSlam = 3,
	Emerge = 4,
};

UCLASS(Blueprintable)
class FARMIONREM_API ASelectableFurniture : public AInteractableObject {
	GENERATED_BODY()

public:
	ASelectableFurniture();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Collision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DisplayMesh;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void CutePopOut(EFurniEasing FurniEasing, AActor* OldFurniture);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void CutePopIn(EFurniEasing FurniEasing, AActor* NewFurniture);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> Furniture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, EFurniEasing> IndexToEase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SetIndex)
	int32 Index = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRemoveFurniture = true;

protected:
	UFUNCTION()
	void OnRep_SetIndex();

	virtual void BeginPlay() override;

	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	UFUNCTION()
	void RenovateModeChanged(bool bRenovateMode);

	virtual bool IsActive() override;
	virtual FText GetInformationText_Implementation() const override;

private:
	UFUNCTION()
	void DestroyCreated(AActor* DestroyedActor);
	
	TSoftObjectPtr<AActor> FurniturePtr;

	int32 PreviousIndex = -1;

	FTimerHandle CooldownTimerHandle;

};