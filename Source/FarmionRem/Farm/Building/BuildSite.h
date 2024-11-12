// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "BuildSite.generated.h"

class AStageUpgrade;

UCLASS(Blueprintable)
class FARMIONREM_API ABuildSite : public AActor, public ISaveLoad, public ILinked {
	GENERATED_BODY()

public:

	ABuildSite();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_IsBuilt)
	bool bIsBuilt = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_IsBuilt();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void Link() override;

private:
	UFUNCTION()
	void OnBought();
	
	TSoftObjectPtr<AStageUpgrade> StageUpgrade;

};