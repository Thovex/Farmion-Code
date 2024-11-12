// Copyright 2020-2023: Jesse J. van Vliet

#include "StudyBench.h"

#include "Curio.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Interactables/Product.h"

AStudyBench::AStudyBench() {
	
}

int32 AStudyBench::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return 0;
}

int32 AStudyBench::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return 0;
}

bool AStudyBench::IgnoreBounds() const {
	return true;
}

bool AStudyBench::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	const UClass* CurioClass = Carryable->GetActorFromInterface()->GetClass();

	if (CurioClass->ImplementsInterface(UCurio::StaticClass()) && !CurrentCurioTypes.Contains(CurioClass)) {
		Carryable->SetCollisions(ECollisionEnabled::Type::NoCollision);

		const int32 LearningPoints = ICurio::Execute_LearningPoints(Carryable->GetActorFromInterface());
		const float LearningTime = ICurio::Execute_LearningTime(Carryable->GetActorFromInterface());
		
		UE_LOG(LogTemp, Warning, TEXT("LP: %d, Time: %f"), LearningPoints, LearningTime);
		return Super::MatchesSocketRequirements(Info, Carryable);
	}
	return false;
}

void AStudyBench::SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) {
	const UClass* CurioType = Carryable->GetActorFromInterface()->GetClass();
	
	if (!Availability) {
		AActor* CarryableAsActor = Carryable->GetActorFromInterface();
		
		const float TimeTillCompletion = ICurio::Execute_LearningTime(CarryableAsActor);
		const int32 LearningPoints = ICurio::Execute_LearningPoints(CarryableAsActor);

		FName Name;
		if (const AProduct* Product = Cast<AProduct>(CarryableAsActor)) {
			Name = Product->ProductName;
		} else {
			Name = ICurio::Execute_CurioName(CarryableAsActor);
		}

		FTimerHandle CurioCompletionHandle;
		GetWorldTimerManager().SetTimer(CurioCompletionHandle, [=] () {
			OnCurioCompletion(Index, Name, LearningPoints);
		}, TimeTillCompletion, false);

		CurrentCurios.Add(Index, CurioCompletionHandle);
		CurrentCurioTypes.Add(CurioType);
	} else {
		CurrentCurios.Remove(Index);
		CurrentCurioTypes.Remove(CurioType);
	}

	// TODO: Multicasterino.
	const TScriptInterface<ICurio>& Curio = TScriptInterface<ICurio>(Carryable->GetActorFromInterface());
	SetSocketAvailability_BP(!Availability, Index, Curio);

	UE_LOG(LogTemp, Warning, TEXT("Set socket availability: %d"), Availability);
	Super::SetSocketAvailability(Carryable, Index, Availability);
}

void AStudyBench::OnCurioCompletion(const int32& SocketIndex, const FName& CurioName, const int32& LearningPoints) {
	UE_LOG(LogTemp, Warning, TEXT("Curio completed! Gained %d learning points!"), LearningPoints);

	GAME_STATE
	GameState->ReceiveSystemMessage_Tunnel(FText::FromString(FString::Printf(TEXT("%s Curio completed! Gained %d Learning Points!"), *CurioName.ToString(), LearningPoints)));

	const int32 PreviousLevel = GameState->CalculateFarmLevel();
	
	GameState->FarmExperience += LearningPoints;
	GameState->OnRep_FarmExperience();

	const int32 CurrentLevel = GameState->CalculateFarmLevel();

	if (CurrentLevel > PreviousLevel) {
		GameState->ReceiveEventMessage_Tunnel(FText::FromString(FString::Printf(TEXT("Farm level up! You are now level %d!"), CurrentLevel)));
	}

	AActor* Target = AvailableSockets[SocketIndex].Carryable->GetActorFromInterface();
	SetSocketAvailability(AvailableSockets[SocketIndex].Carryable, SocketIndex, true);
	Target->Destroy(true);
}

bool AStudyBench::ClearExisting(AActor* ThisAsActor) {
	for (auto&& [SocketIndex, TimerHandle] : CurrentCurios) {
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
	
	return Super::ClearExisting(ThisAsActor);
}

void AStudyBench::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	TArray<TSharedPtr<FJsonValue>> AvailableSocketsArray = JsonObject->GetArrayField(TEXT("Sockets"));
	for (const TSharedPtr<FJsonValue> SocketElement : AvailableSocketsArray) { // Expects "UnAvailable" Sockets
		const TSharedPtr<FJsonObject> SocketObject = SocketElement->AsObject();

		const int32 SocketIndex = SocketObject->GetIntegerField("Index");
		
		SocketObject->SetNumberField("RemainingDuration", GetWorldTimerManager().GetTimerRemaining(CurrentCurios[SocketIndex]));
	}
	
	JsonObject->SetArrayField(TEXT("Sockets"), AvailableSocketsArray);

	DEBUG_JSON
	
}

void AStudyBench::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	FTimerManager& TimerManager = GetWorldTimerManager();
	
	TArray<TSharedPtr<FJsonValue>> AvailableSocketsArray = JsonObject->GetArrayField(TEXT("Sockets"));
	for (const TSharedPtr<FJsonValue> SocketElement : AvailableSocketsArray) {
		const TSharedPtr<FJsonObject> SocketObject = SocketElement->AsObject();

		const int32 SocketIndex = SocketObject->GetIntegerField("Index");
		const float RemainingDuration = SocketObject->GetNumberField("RemainingDuration");

		AActor* CarryableAsActor = AvailableSockets[SocketIndex].Carryable->GetActorFromInterface();

		// Clear Timer Created in Super::Load -> We want to replace it with a new one, with the correct times, with copying as little code as possible.
		TimerManager.ClearTimer(CurrentCurios[SocketIndex]);

		const int32 LearningPoints = ICurio::Execute_LearningPoints(CarryableAsActor);

		FName Name;
		if (const AProduct* Product = Cast<AProduct>(CarryableAsActor)) {
			Name = Product->ProductName;
		} else {
			Name = ICurio::Execute_CurioName(CarryableAsActor);
		}
        
		FTimerHandle CurioCompletionHandle;
		GetWorldTimerManager().SetTimer(CurioCompletionHandle, [=] () {
			OnCurioCompletion(SocketIndex, Name, LearningPoints);
		}, RemainingDuration, false);
        
		// Update CurrentCurios with the newly set timer.
		CurrentCurios.Add(SocketIndex, CurioCompletionHandle);
		//CurrentCurioTypes.Add(Class); // Already added at this point :-)
	}
}