// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "StudyBench.generated.h"

class ICurio;

UCLASS(Blueprintable)
class FARMIONREM_API AStudyBench : public AStorageUnit {
	GENERATED_BODY()

public:
	AStudyBench();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) 
	void SetSocketAvailability_BP(const bool bEnabled, const int32 SocketIndex, const TScriptInterface<ICurio>& Curio);

protected:
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual bool IgnoreBounds() const override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;
	virtual void SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) override;

	UFUNCTION()
	void OnCurioCompletion(const int32& SocketIndex, const FName& CurioName, const int32& LearningPoints);

	virtual bool ClearExisting(AActor* ThisAsActor) override;
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

private:
	UPROPERTY()
	TSet<const UClass*> CurrentCurioTypes;

	UPROPERTY()
	TMap<int32, FTimerHandle> CurrentCurios;
};
