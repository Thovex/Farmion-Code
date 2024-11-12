// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Engine/DataTable.h"
#include "GrowthData.generated.h"

class ACropPlant;

enum class ESeedStage : uint8;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGrowthDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACropPlant> Plant;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ESeedStage, int32> DaysPerStage;

	FORCEINLINE int32 GetTotalDays() {
		int32 Days = 0;
		for (const auto& Val : DaysPerStage) {
			Days += Val.Value;
		}
		return Days;
	}
};