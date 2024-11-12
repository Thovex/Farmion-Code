// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Farm/Upgrades/StageUpgrade.h"
#include "TrellisUpgrade.generated.h"

class ATrellis;

UCLASS(Blueprintable)
class FARMIONREM_API ATrellisUpgrade : public AStageUpgrade {
	GENERATED_BODY()

public:
	ATrellisUpgrade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;

	UPROPERTY()
	TArray<ATrellis*> Trellises;

protected:
	virtual void Build() override;
	virtual void HandleBuildTrellises();

	virtual void PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap) override;

	virtual void Link() override;

private:
	bool bEnsure = false;

};