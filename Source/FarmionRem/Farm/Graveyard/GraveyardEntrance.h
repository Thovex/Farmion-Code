// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "GraveyardEntrance.generated.h"

class AGrave;

UCLASS(Blueprintable)
class FARMIONREM_API AGraveyardEntrance : public AUpgrade, public ICycleAffected {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentGraves)
	int32 CurrentGraves;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_CurrentGraves();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_GraveyardEverOpened)
	bool bGraveyardEverOpened;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_GraveyardEverOpened();

protected:
	UFUNCTION()
	void OnGraveActiveChanged(bool bActive);
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;


};