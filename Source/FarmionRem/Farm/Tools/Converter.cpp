// Copyright 2020-2023: Jesse J. van Vliet


#include "Converter.h"

#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "FarmionRem/Interactables/Product.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Net/UnrealNetwork.h"

AConverter::AConverter() {
	FallbackSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("FallbackSpawnLocation"));
	FallbackSpawnLocation->SetupAttachment(RootComponent);

	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

bool CheckRequiredInputsForRecipe(const TArray<TSubclassOf<AInteractableObject>>& RequiredInputs, const TArray<AActor*>& AvailableIngredients) {
	TMap<TSubclassOf<AInteractableObject>, int32> RequiredInputCounts;

	for (const TSubclassOf<AInteractableObject>& RequiredInput : RequiredInputs) {
		if (!RequiredInputCounts.Contains(RequiredInput)) {
			RequiredInputCounts.Add(RequiredInput, 1);
		} else {
			RequiredInputCounts[RequiredInput]++;
		}
	}

	for (const AActor* Ingredient : AvailableIngredients) {
		TSubclassOf<AInteractableObject> IngredientClass = Ingredient->GetClass();

		if (RequiredInputCounts.Contains(IngredientClass) && RequiredInputCounts[IngredientClass] > 0) {
			RequiredInputCounts[IngredientClass]--;
		}
	}

	for (const auto& Entry : RequiredInputCounts) {
		if (Entry.Value > 0) {
			UE_LOG(LogTemp, Log, TEXT("Recipe does not have required inputs."));
			return false;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Recipe has required inputs."));
	return true;
}

void AConverter::Convert_Implementation(const FName& RecipeName) {
	bIsConverting = true;
	OnRep_IsConverting();

	UE_LOG(LogTemp, Log, TEXT("Converting... Server"));

	FString ContextString;
	CurrentConvertedRecipeRow = *RecipeTable->FindRow<FConverterRecipeTableRow>(RecipeName, ContextString, true);
	GetWorld()->GetTimerManager().SetTimer(ConversionTimerHandle, this, &AConverter::Output, CurrentConvertedRecipeRow.ConversionTime, false);

	UE_LOG(LogTemp, Log, TEXT("Conversion time: %f"), CurrentConvertedRecipeRow.ConversionTime);
}

void AConverter::TryConvert() {
	FName MatchedRecipe = NAME_None;

	//UE_LOG(LogTemp, Warning, TEXT("%s"), MatchesAnyRecipe(MatchedRecipe) ? TEXT("true") : TEXT("false"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), MatchesConditions() ? TEXT("true") : TEXT("false"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), bIsConverting ? TEXT("true") : TEXT("false"));

	if (MatchesAnyRecipe(MatchedRecipe) && MatchesConditions() && !bIsConverting) {
		UE_LOG(LogTemp, Log, TEXT("Starting conversion..."));
		Convert(MatchedRecipe);
	}
}

bool AConverter::MatchesAnyRecipe(FName& MatchedRecipe) {
	TArray<AActor*> AvailableIngredients;
	for (const FCarryableSocketInfo& Info : AvailableSockets) {
		if (!Info.Available) {
			AvailableIngredients.Add(Info.Carryable->GetActorFromInterface());
			UE_LOG(LogTemp, Log, TEXT("Found ingredient %s."), *Info.Carryable->GetActorFromInterface()->GetName());
		}
	}

	bool bHasRequiredInput = false;
	static const FString ContextString(TEXT("Checking Required Inputs"));
	for (const auto& Row : RecipeTable->GetRowMap()) {
		const FConverterRecipeTableRow* RecipeRow = RecipeTable->FindRow<FConverterRecipeTableRow>(Row.Key, ContextString, true);

		if (CheckRequiredInputsForRecipe(RecipeRow->Input, AvailableIngredients)) {
			UE_LOG(LogTemp, Log, TEXT("Recipe %s has required inputs."), *Row.Key.ToString());
			
			bHasRequiredInput = true;
			MatchedRecipe = Row.Key;
			break;
		}
	}

	return bHasRequiredInput;
}

bool AConverter::MatchesConditions() {
	return true;
}

void AConverter::SpawnOutput_Forced() {
	
	for (TSubclassOf<AInteractableObject> OutputClass : CurrentConvertedRecipeRow.Output) {
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(FallbackSpawnLocation->GetComponentLocation());
		SpawnTransform.SetRotation((FallbackSpawnLocation->GetComponentRotation() + FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f)).Quaternion());

		AInteractableObject* SpawnedObject = GetWorld()->SpawnActorDeferred<AInteractableObject>(OutputClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		GAME_STATE
		const float Modifier = GET_DEFAULT_QUALITY_MODIFIER(Quality);
		SpawnedObject->Quality = DetermineQualityWithModifier(GameState->CalculateFarmLevel(), Modifier);

		if (UCommonLibrary::HasFlag(ConversionEnd, EConversionEndResult::UseStorage)) {
			if (SpawnStorage) {
				FCarryableSocketInfo Info;
				int32 Index;
				if (SpawnStorage->GetMatchingSocket(Info, Index, SpawnedObject)) {
					SpawnStorage->SetSocketAvailability(SpawnedObject, Index, false);

					FVector SocketOrigin, SocketBoundsExtent;
					Info.GetSocketBounds(SocketOrigin, SocketBoundsExtent);

					UGameplayStatics::FinishSpawningActor(SpawnedObject, SpawnTransform);
					SpawnedObject->AttachToActor(SpawnStorage.Get(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					SpawnedObject->SetActorLocation(Info.Component->GetComponentLocation() - FVector(0, 0, SocketBoundsExtent.Z));
					SpawnedObject->SetActorRotation(Info.Component->GetComponentRotation());

					const TScriptInterface<ICarryable> CarryableInt = TScriptInterface<ICarryable>(SpawnedObject);
					const TScriptInterface<ICarryableSocket> CarryableSocketInt = TScriptInterface<ICarryableSocket>(SpawnStorage.Get());
					CarryableInt->CarryableSocket = CarryableSocketInt;
					CarryableInt->CarryableSocketIndex = Index;

					UE_LOG(LogTemp, Log, TEXT("Spawned %s into Storage %s"), *SpawnedObject->GetName(), *SpawnStorage->GetName());
					continue;
				}
			}
		}

		UGameplayStatics::FinishSpawningActor(SpawnedObject, SpawnTransform);
		ManipulateSpawnedObject(SpawnedObject);

		if (IsValid(SpawnedObject)) {
			UE_LOG(LogTemp, Log, TEXT("Spawned %s into Fallback"), *SpawnedObject->GetName());
		} else {
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn %s"), *OutputClass->GetName())
		}
	}
}

void AConverter::SpawnOutput() {
	if (UCommonLibrary::HasFlag(ConversionEnd, EConversionEndResult::UseEQS)) {
		if (SpawnWithEQS()) {
			// Remember to call ManipulateSpawnedObject on the spawned objects.
			return;
		}
	}

	SpawnOutput_Forced();
}

void AConverter::Output() {
	if (UCommonLibrary::HasFlag(ConversionEnd, EConversionEndResult::Manual)) {
		UE_LOG(LogTemp, Error, TEXT("Handle Conversion End Manually by Overriding Output! Returning..."));
		return;
	}
	
	bIsConverting = false;
	OnRep_IsConverting();
	
	SpawnOutput();

	ProcessInputAfterConversion();
}

void AConverter::ManipulateSpawnedObject(AInteractableObject* SpawnedObject) {}

bool AConverter::SpawnWithEQS() {
	const UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(GetWorld());
	if (!EQSManager) {
		UE_LOG(LogTemp, Error, TEXT("No EQS Manager found!"));
		return false;
	}

	if (EQS_Template == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No EQS Template found!"));
	}

	if (auto* Instance = EQSManager->RunEQSQuery(GetWorld(), EQS_Template, this, EEnvQueryRunMode::AllMatching, nullptr)) {
		Instance->GetOnQueryFinishedEvent().AddDynamic(this, &AConverter::AConverter::SpawnWithEQS_Result);
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to run EQS Query!"));
	return false;
}

void AConverter::OnConversionStartInput() {
	//
}

void AConverter::OnConversionEndInput() {
	//
}

void AConverter::ProcessInputAfterConversion() {
	for (auto& Socket : AvailableSockets) {
		Socket.Carryable->GetActorFromInterface()->Destroy();
		Socket.Available = true;
	}

	// Mandatory
	SocketsInUse = GetSocketsInUse();
}

bool AConverter::IsAllowedToPickFromSocket() {
	return !bIsConverting;
}

void AConverter::SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) {
	Super::SetSocketAvailability(Carryable, Index, Availability);

	if (!Availability) {
		AddedIngredient(Carryable, Index);
	} else {
		RemovedIngredient(Carryable, Index);
	}

	if (UCommonLibrary::HasFlag(ConversionStart, EConversionStartResult::StartOnFullInput)) {
		if (SocketsInUse == AvailableSockets.Num()) {
			TryConvert();
		}
	}

	if (UCommonLibrary::HasFlag(ConversionStart, EConversionStartResult::StartOnMatchesRecipe)) {
		FName MatchedRecipe;
		if (MatchesAnyRecipe(MatchedRecipe)) {
			TryConvert();
		}
	}
}

void AConverter::OnRep_IsConverting_Implementation() {
	if (bIsConverting) {
		OnConversionStartInput();
	} else {
		OnConversionEndInput();
	}
}

void AConverter::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	const FVector& Loc = FallbackSpawnLocation->GetComponentLocation();
	JsonObject->SetNumberField("FallbackSpawnX", Loc.X);
	JsonObject->SetNumberField("FallbackSpawnY", Loc.Y);
	JsonObject->SetNumberField("FallbackSpawnZ", Loc.Z);
}

void AConverter::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	FallbackSpawnLocation->SetWorldLocation(FVector(
		JsonObject->GetNumberField("FallbackSpawnX"),
		JsonObject->GetNumberField("FallbackSpawnY"),
		JsonObject->GetNumberField("FallbackSpawnZ")
	));
}

FText AConverter::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	FString Text = Super::GetInformationText_Implementation().ToString();
	Text = Text.Replace(*ToolName.ToString(), *FString::Printf(TEXT("<Prod>%s</>"), *ToolName.ToString()));

	return FText::FromString(Text);
	
}

void AConverter::Link() {
	if (Linker->Links.IsEmpty()) {
		return;
	}

	SpawnStorage = Linker->GetLinkedActor(FName(TEXT("SpawnStorage")));
}

void AConverter::BeginPlay() {
	Super::BeginPlay();

	OnRep_IsConverting();
	
	Link();
}

void AConverter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConverter, bIsConverting);
}

void AConverter::SpawnWithEQS_Result(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus) {
	if (!QueryInstance) {
		SpawnOutput_Forced();
		return;
	}
	
	TArray<FVector> Locations;
	QueryInstance->GetQueryResultsAsLocations(Locations);

	UCommonLibrary::Shuffle<FVector>(Locations);
	for (int32 i = 0; i < CurrentConvertedRecipeRow.Output.Num(); i++) {
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(FallbackSpawnLocation->GetComponentLocation());
		SpawnTransform.SetRotation((FallbackSpawnLocation->GetComponentRotation() + FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f)).Quaternion());

		AInteractableObject* SpawnedObject = GetWorld()->SpawnActorDeferred<AInteractableObject>(CurrentConvertedRecipeRow.Output[i], SpawnTransform);

		GAME_STATE
		const float Modifier = GET_DEFAULT_QUALITY_MODIFIER(Quality);
		SpawnedObject->Quality = DetermineQualityWithModifier(GameState->CalculateFarmLevel(), Modifier);

		SpawnTransform.SetLocation(Locations[i % Locations.Num()]);
		
		UGameplayStatics::FinishSpawningActor(SpawnedObject, SpawnTransform);
		ManipulateSpawnedObject(SpawnedObject);
	}
}
