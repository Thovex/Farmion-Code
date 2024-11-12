// Copyright 2020-2023: Jesse J. van Vliet

#include "FarmionGameInstance.h"
#include "EngineUtils.h"
#include "FarmionGameState.h"
#include "HAL/FileManagerGeneric.h"
#include "Interfaces/Cycle/CycleAffected.h"
#include "Io/FarmionSaveGame.h"
#include "Io/FarmionSaveGameData.h"
#include "Io/SaveLoad.h"
#include "Kismet/GameplayStatics.h"

AActor* UFarmionGameInstance::AutoCreateLoadedObject(UWorld* World, const TSharedPtr<FJsonObject>& JsonObject, TMap<uint32, AActor*>& IdToActor) {
	UClass* LoadedClass = LoadObject<UClass>(nullptr, *JsonObject->GetStringField("Class"));

	if (LoadedClass && World) {
		// Set the spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Define the location and rotation for the new object
		const FVector SpawnLocation = FVector(JsonObject->GetNumberField("LocationX"), JsonObject->GetNumberField("LocationY"), JsonObject->GetNumberField("LocationZ"));
		const FRotator SpawnRotation = FRotator(JsonObject->GetNumberField("RotationPitch"), JsonObject->GetNumberField("RotationYaw"), JsonObject->GetNumberField("RotationRoll"));
		const FVector SpawnScale = FVector(JsonObject->GetNumberField("ScaleX"), JsonObject->GetNumberField("ScaleY"), JsonObject->GetNumberField("ScaleZ"));

		AActor* NewObject = World->SpawnActor<AActor>(LoadedClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (NewObject) {
			NewObject->SetActorScale3D(SpawnScale);

			const TScriptInterface<ISaveLoad> NewObjectInterface(NewObject);
			if (NewObjectInterface) {
				IdToActor.Add(JsonObject->GetNumberField("UniqueId"), NewObject);
				NewObjectInterface->Load(JsonObject);
				UE_LOG(LogTemp, Log, TEXT("Load Called for: %s"), *NewObject->GetFullName());
			}

			NewObject->SetReplicates(true);
		}

		return NewObject;
	}

	return nullptr;
}


void UFarmionGameInstance::SaveGame(FString SaveSlotName) {
	FFarmionSaveGameData SaveGameData;
	SaveGameData.SaveSlotName = SaveSlotName;
	SaveGameData.UserIndex = 0;

	GAME_STATE

	// Save your game-wide state data, like money
	SaveGameData.Money = GameState->Money;
	SaveGameData.FarmExperience = GameState->FarmExperience;
	SaveGameData.Version = GSaveGame_Version;
	SaveGameData.Day = GameState->Day;

	TSet<uint32> UniqueIds;

	// Iterate through all the objects that implement the SaveLoadInterface
	for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
		AActor* Actor = *It;
		TScriptInterface<ISaveLoad> SaveLoadInt(Actor);

		if (SaveLoadInt) {
			const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			SaveLoadInt->Save(JsonObject);

			uint32 UniqueId;
			if (JsonObject->TryGetNumberField("UniqueId", UniqueId)) {
				if (UniqueIds.Contains(UniqueId)) {
					// TODO: Make unique ids be properly unique.

					for (size_t i = 0; i < 1000; i++) {
						UE_LOG(LogTemp, Error, TEXT("DUPLICATE ID: %f"), JsonObject->GetNumberField("UniqueId"));
					}
					FString JsonString;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
					FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
					UE_LOG(LogTemp, Error, TEXT("%s"), *Actor->GetName());
					check(nullptr);
				}

				UniqueIds.Add(SaveLoadInt->Id);
			}

			FString JsonData;
			if (SaveLoadInt->Parser(JsonObject, JsonData)) {
				SaveGameData.ObjectData.Add(JsonData);
				//UE_LOG(LogTemp, Warning, TEXT("Save Called for: %s"), *Actor->GetFullName())
			} else {
				UE_LOG(LogTemp, Log, TEXT("Save Ignored for: %s"), *Actor->GetFullName())
			}
		}
	}

	// Save the game data to a save game object
	UFarmionSaveGame* SaveGameInstance = Cast<UFarmionSaveGame>(UGameplayStatics::CreateSaveGameObject(UFarmionSaveGame::StaticClass()));
	SaveGameInstance->SaveGameData = SaveGameData;

	UE_LOG(LogTemp, Warning, TEXT("Start Saving %d Objects..."), SaveGameData.ObjectData.Num());

	// Save the save game object to a slot
	const bool bIsSaved = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameData.SaveSlotName, SaveGameData.UserIndex);

	UE_LOG(LogTemp, Warning, TEXT("Saved? %hs"), bIsSaved ? "True" : "False");
}

void UFarmionGameInstance::LoadGame(FString SaveSlotName) {
	GAME_STATE

	//GameState->Multicast_LoadScreen();

	FFarmionSaveGameData SaveGameData;
	SaveGameData.SaveSlotName = SaveSlotName;
	SaveGameData.UserIndex = 0;

	USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SaveGameData.SaveSlotName, SaveGameData.UserIndex);
	UE_LOG(LogTemp, Warning, TEXT("Loaded? %hs"), SaveGame != nullptr ? "True" : "False");

	// Load the save game object from a slot
	UFarmionSaveGame* LoadGameInstance = Cast<UFarmionSaveGame>(SaveGame);
	UE_LOG(LogTemp, Warning, TEXT("Loaded Correctly? %hs"), (LoadGameInstance != nullptr) ? "True" : "False");

	if (LoadGameInstance->SaveGameData.Version != GSaveGame_Version) {
		UpgradeSaveGame(LoadGameInstance->SaveGameData);
	}

	GameState->Money = LoadGameInstance->SaveGameData.Money;
	GameState->OnRep_Money();

	GameState->FarmExperience = LoadGameInstance->SaveGameData.FarmExperience;
	GameState->OnRep_FarmExperience();

	GameState->bRenovateHouseMode = false;
	GameState->OnRep_RenovateHouseMode();
	
	GameState->Day = LoadGameInstance->SaveGameData.Day;

	if (LoadGameInstance) {
		// Retrieve the saved game data
		SaveGameData = LoadGameInstance->SaveGameData;

		TMap<uint32, AActor*> UniqueIdToActor;

		// Iterate through all the objects that kill existing SaveLoadInterfaces
		for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
			const TScriptInterface<ISaveLoad> SaveLoadInt(*It);
			if (SaveLoadInt) {
				switch (SaveLoadInt->LoadMethod()) {
					case ELoadMethod::ManualReset: {
						UniqueIdToActor.Add(SaveLoadInt->Id, *It);
					} break;
					case ELoadMethod::Recreate: {
						UE_LOG(LogTemp, Log, TEXT("Recreate: %s %hs"), *It->GetFullName(), SaveLoadInt->ClearExisting(*It) ? "Destroyed" : "Not Destroyed");
					} break;
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Start Loading %d Money..."), SaveGameData.Money);
		UE_LOG(LogTemp, Warning, TEXT("Start Loading %d Objects..."), SaveGameData.ObjectData.Num());

		TArray<TTuple<TSharedPtr<FJsonObject>, TScriptInterface<ISaveLoad>>> RecreatedInterfaces;
		for (auto& JsonData : SaveGameData.ObjectData) {
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonData);

			// TODO: Add LoadPriority.

			if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
				const bool bAutoRecreate = JsonObject->GetBoolField("AutoRecreate");
				const ELoadMethod LoadMethod = static_cast<ELoadMethod>(JsonObject->GetNumberField("LoadMethod"));

				switch (LoadMethod) {
				case ELoadMethod::Recreate:
				{
					if (!bAutoRecreate) {
						UE_LOG(LogTemp, Log, TEXT("LoadMethod::Recreate, but not auto-recreating: %s"), *JsonData);
						continue;
					}

					UE_LOG(LogTemp, Log, TEXT("LoadMethod::Recreate, auto-recreating: %s"), *JsonData);

					TScriptInterface<ISaveLoad> SaveLoadInt = TScriptInterface<ISaveLoad>(AutoCreateLoadedObject(GetWorld(), JsonObject, UniqueIdToActor));
					TTuple<TSharedPtr<FJsonObject>, TScriptInterface<ISaveLoad>> Tuple(JsonObject, SaveLoadInt);
					RecreatedInterfaces.Add(Tuple);
				}
				break;
				case ELoadMethod::ManualReset:
				{
					UE_LOG(LogTemp, Log, TEXT("Manual Load for: %s"), *JsonData);
					if (UniqueIdToActor.Contains(JsonObject->GetNumberField("UniqueId"))) {
						TScriptInterface<ISaveLoad> SaveLoadInt = UniqueIdToActor[JsonObject->GetNumberField("UniqueId")];
						SaveLoadInt->Load(JsonObject);

						TTuple<TSharedPtr<FJsonObject>, TScriptInterface<ISaveLoad>> Tuple(JsonObject, SaveLoadInt);
						RecreatedInterfaces.Add(Tuple);
					} else {
						UE_LOG(LogTemp, Error, TEXT("Manual Load Failed for: %s"), *JsonData);
					}
				}
				break;
				}
			}
		}

		for (auto& [JsonObject, SaveLoadInt] : RecreatedInterfaces) {
			//UE_LOG(LogTemp, Log, TEXT("Post Load Called! %s"), *SaveLoadInt.GetObject()->GetName());
			SaveLoadInt->PostLoaded(JsonObject, UniqueIdToActor);
		}
	}
}

void UFarmionGameInstance::UpdateCycle() {
	UE_LOG(LogTemp, Warning, TEXT("It's a new day!"));
	SaveGame("AUTOSAVE");

	GAME_STATE

	GameState->CycleDeathCount = 0;

	TArray<TScriptInterface<ICycleAffected>> Affected;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
		AActor* Actor = *It;
		TScriptInterface<ICycleAffected> ActorInterface(Actor);

		if (ActorInterface) {
			Affected.Add(ActorInterface);
			if (!ActorInterface->IsProtected()) {
				UE_LOG(LogTemp, Log, TEXT("PerformCycle for: %s"), *Actor->GetFullName());
				ActorInterface->PerformCycle();
			} else {
				UE_LOG(LogTemp, Log, TEXT("Protected from Cycle for: %s"), *Actor->GetFullName());
			}
		}
	}

	for (const auto& Interface : Affected) {
		Interface->PostPerformCycle();
	}

	if (GameState->CycleDeathCount >= 4) {
		UE_LOG(LogTemp, Error, TEXT("Game Over!"));

		GameState->ReceiveEventMessage_Tunnel(FText::FromString("You have no lives remaining."));
	}
}

void Upgrade_V1_To_V2(FFarmionSaveGameData& SaveData) {
	UE_LOG(LogTemp, Warning, TEXT("Starting Upgrade from [%d] to [%d]"), 1, 2);
	SaveData.Version = 2;
	UE_LOG(LogTemp, Warning, TEXT("Upgrade Complete!"));
}

void UFarmionGameInstance::UpgradeSaveGame(FFarmionSaveGameData& SaveData) {
	if (SaveData.Version == 1) {
		Upgrade_V1_To_V2(SaveData);
	}
}

TArray<FString> UFarmionGameInstance::LoadAllSaveGames(const FString& SaveSlotName) {
	TArray<FString> SaveGames;

	// Get the Saved Games directory.
	FString SaveGamesDirectory = FPaths::ProjectSavedDir() + TEXT("SaveGames/");

	// Get a pointer to the file manager.
	IFileManager* FileManager = &IFileManager::Get();

	// FindFiles will hold the results.
	TArray<FString> SaveFileNames;

	// Find all .sav files in the Saved Games directory.
	FileManager->FindFiles(SaveFileNames, *SaveGamesDirectory, TEXT("sav"));

	FFileManagerGeneric fm;
	
	
	for (FString SaveFileName : SaveFileNames) {
		//USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0);
		//SaveGames.Add(Cast<UFarmionSaveGame>(SaveGame));
		FString FilePathP1 = SaveGamesDirectory + SaveFileName;
		const TCHAR* Filepath = *FilePathP1;
		auto FileDateTime = fm.GetTimeStamp(Filepath);
		SaveGames.Add(SaveFileName + TEXT(" ") + FileDateTime.ToString());
	}

	return SaveGames;
    
}