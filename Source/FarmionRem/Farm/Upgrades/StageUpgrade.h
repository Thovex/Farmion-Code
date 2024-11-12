// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Upgrade.h"
#include "StageUpgrade.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AStageUpgrade : public AUpgrade {
	GENERATED_BODY()

public:
	AStageUpgrade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_BuildRequirementsMet)
	TArray<TSubclassOf<AInteractableObject>> BuildRequirementsMet;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_BuildRequirementsMet();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_BuildRequirementsNecessary)
	TArray<TSubclassOf<AInteractableObject>> BuildRequirementsNecessary;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_BuildRequirementsNecessary();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BuildInput;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void FinishConstruction();

protected:
	UFUNCTION()
	void OnBuildInputBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	
	virtual bool MatchesBuildRequirements();
	virtual void Build();
	
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void OnRep_Bought() override;

	virtual FText GetInformationText_Implementation() const override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};