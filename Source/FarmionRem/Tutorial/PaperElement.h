// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Tutorial/InfoElement.h"
#include "PaperElement.generated.h"

USTRUCT(Blueprintable)
struct FPaperElementSaveData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TextSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;
	
};

UCLASS(Blueprintable)
class FARMIONREM_API APaperElement : public AInfoElement {
	GENERATED_BODY()

public:
	APaperElement();

protected:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FPaperElementSaveData Save_BP();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Load_BP(FPaperElementSaveData SaveData);

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
};
