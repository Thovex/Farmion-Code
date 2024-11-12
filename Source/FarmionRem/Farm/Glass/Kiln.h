// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Tools/Converter.h"
#include "Kiln.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FARMIONREM_API AKiln : public AConverter {
	GENERATED_BODY()

public:
	AKiln();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DropLocation;
	
protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;

	virtual bool MatchesAnyRecipe(FName& MatchedRecipe) override;
	virtual void ProcessInputAfterConversion() override;

	virtual void ManipulateSpawnedObject(AInteractableObject* SpawnedObject) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
};
