// MySaveGame.h
#pragma once

#include "FarmionRem/Io/FarmionSaveGameData.h"
#include "GameFramework/SaveGame.h"
#include "FarmionSaveGame.generated.h"

UCLASS(Blueprintable)
class UFarmionSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FFarmionSaveGameData SaveGameData;
};