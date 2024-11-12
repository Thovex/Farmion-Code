// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "AdvancedFriendsGameInstance.h"
#include "FarmionRem/Io/FarmionSaveGameData.h"
#include "FarmionGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FLinkData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UniqueId = 0;
};

/**
 * 
 */
UCLASS(Blueprintable)
class FARMIONREM_API UFarmionGameInstance : public UAdvancedFriendsGameInstance {
	GENERATED_BODY()

public:
	static AActor* AutoCreateLoadedObject(UWorld* World, const TSharedPtr<FJsonObject>& JsonObject, TMap<uint32, AActor*>& IdToActor);

	UFUNCTION(BlueprintCallable, Category = "SaveSlotName")
	void SaveGame(FString SaveSlotName);
	//void SaveGame(FString SaveSlotName);
	
	UFUNCTION(BlueprintCallable, Category = "SaveSlotName")
	void LoadGame(FString SaveSlotName);

	UFUNCTION(BlueprintCallable)
	void UpdateCycle();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CheckedMainMenu;

	UFUNCTION(BlueprintCallable)
	TArray<FString> LoadAllSaveGames(const FString& SaveSlotName);

private:
	void UpgradeSaveGame(FFarmionSaveGameData& SaveData);

};
