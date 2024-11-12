// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "Converter.generated.h"

class AProduct;

USTRUCT(BlueprintType)
struct FConverterRecipeTableRow : public FTableRowBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AInteractableObject>> Input;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AInteractableObject>> Output;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ConversionTime = 5.f;
};

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EConversionEndResult : uint8 {
	None							= 0,
	Manual							= 1 << 0,
	UseStorage						= 1 << 1,
	UseEQS							= 1 << 2,	
};
ENUM_CLASS_FLAGS(EConversionEndResult);

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EConversionStartResult : uint8 {
	None							= 0,
	Manual							= 1 << 0,
	StartOnMatchesRecipe			= 1 << 1,
	StartOnFullInput				= 1 << 2,
};
ENUM_CLASS_FLAGS(EConversionStartResult);


// DEFAULT LINKER NAME: [SpawnStorage]

UCLASS(Blueprintable)
class FARMIONREM_API AConverter : public AStorageUnit {
	GENERATED_BODY()

public:
	AConverter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* FallbackSpawnLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;

	// Handle clientside aesthetics here.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_IsConverting();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void AddedIngredient(const TScriptInterface<ICarryable>& Carryable, const int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void RemovedIngredient(const TScriptInterface<ICarryable>& Carryable, const int32 Index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* RecipeTable;

	UPROPERTY(EditAnywhere,  meta=(Bitmask, BitmaskEnum = "/Script/FarmionRem.EConversionStartResult"))
	uint8 ConversionStart;

	UPROPERTY(EditAnywhere,  meta=(Bitmask, BitmaskEnum = "/Script/FarmionRem.EConversionEndResult"))
	uint8 ConversionEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FConverterRecipeTableRow CurrentConvertedRecipeRow;

	UFUNCTION(BlueprintCallable)
	virtual void ManipulateSpawnedObject(AInteractableObject* SpawnedObject);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnvQuery* EQS_Template = nullptr;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_IsConverting)
	bool bIsConverting = false;

	UFUNCTION(Server, Reliable)
	void Convert(const FName& RecipeName);

	virtual void TryConvert();
	
	virtual bool MatchesAnyRecipe(FName& MatchedRecipe);
	virtual bool MatchesConditions();
	virtual void SpawnOutput();
	virtual void SpawnOutput_Forced();

	FTimerHandle ConversionTimerHandle;
	virtual void Output();

	virtual void OnConversionStartInput(); // AESTHETIC ONLY
	virtual void OnConversionEndInput(); // AESTHETIC ONLY
	
	virtual void ProcessInputAfterConversion();

	virtual bool IsAllowedToPickFromSocket() override;
	virtual void SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual void Link() override;

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
	virtual bool SpawnWithEQS();

	UFUNCTION()
	virtual void SpawnWithEQS_Result(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	TSoftObjectPtr<AStorageUnit> SpawnStorage;
};