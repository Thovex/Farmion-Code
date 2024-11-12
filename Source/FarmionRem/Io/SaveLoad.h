// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "SaveLoad.generated.h"

UENUM(BlueprintType)
enum class ELoadMethod : uint8 {
	Recreate,
	ManualReset
};

// This class does not need to be modified.
UINTERFACE()
class USaveLoad : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ISaveLoad {
	GENERATED_BODY()

public:
	static void SaveTransform(const TSharedPtr<FJsonObject>& JsonObject, const AActor* Actor);
	static void SaveClass(const TSharedPtr<FJsonObject>& JsonObject, const UObject* Object);

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject);
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject);
	virtual void PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap);
	virtual bool ClearExisting(AActor* ThisAsActor);

	virtual bool Parser(const TSharedPtr<FJsonObject> JsonObject, FString& JsonData);
	virtual ELoadMethod LoadMethod();
	virtual uint32 LoadPriority(); // Higher -> More load priority.

	static void SaveClassPath(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, const UObject* Object);
	static UClass* LoadClassPath(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName);


	uint32 Id = 0;
	uint32 GetId();
};