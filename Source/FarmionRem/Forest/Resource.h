// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Tools/ToolConverter.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "Resource.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AResource : public AToolConverter {
	GENERATED_BODY()

public:
	AResource();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOneShot = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Chopped)
	bool bChopped;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Chopped();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AvailableSegmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* UnavailableSegmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RespawnTime = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> PotentialDropLocations;

protected:
	virtual void Respawn();
	
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual bool MatchesConditions() override;
	virtual void Output() override;
	virtual void SpawnOutput() override;
	
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FTimerHandle RespawnTimer;

};