// SaveGameData.h
#pragma once

#include "CoreMinimal.h"
#include "FarmionSaveGameData.generated.h"

USTRUCT()
struct FFarmionSaveGameData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere)
	int32 UserIndex = 0;

	UPROPERTY(VisibleAnywhere)
	int32 Money = 0;

	UPROPERTY(VisibleAnywhere)
	int32 FarmExperience = 1;

	UPROPERTY(VisibleAnywhere)
	int32 Version = 0;

	UPROPERTY(VisibleAnywhere)
	int32 Day = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> ObjectData;
};