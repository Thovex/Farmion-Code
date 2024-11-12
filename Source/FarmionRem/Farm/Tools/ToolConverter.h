// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Converter.h"
#include "ToolConverter.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AToolConverter : public AConverter {
	GENERATED_BODY()


public:
	AToolConverter();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* ToolDropLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ATool> HarvestToolClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideRest = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideTool = true;

protected:
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void ProcessInputAfterConversion() override;

	virtual void OnConversionStartInput() override;
	virtual void OnConversionEndInput() override;

};