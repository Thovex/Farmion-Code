// Copyright 2020-2023: Jesse J. van Vliet

#include "Cycle.h"

#include "EngineUtils.h"
#include "Components/DirectionalLightComponent.h"
#include "FarmionRem/FarmionGameInstance.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Characters/Player/FarmionPlayerController.h"
#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Interactables/Seed.h"
#include "Net/UnrealNetwork.h"

ACycle::ACycle() {
	PrimaryActorTick.bCanEverTick = true;

	Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	Sun->SetupAttachment(RootComponent);
}

void ACycle::OnRep_TimeUntilNight() {
	const float PercentageOfDay = (TimeUntilNight / DayDuration) * 100.f;
	const float RotationAngle = 90.f * (1 - TimeUntilNight / DayDuration);
	SetActorRelativeRotation(FRotator(RotationAngle, 0.f, 0.f));
	
	if (FMath::Abs(PercentageOfDay - LastPercentage) >= 1.f) {
		UpdateSkySphere(SkySphere.Get());
		UpdateChameleon(Chameleon.Get(), PercentageOfDay);
		LastPercentage = PercentageOfDay;
	}
}

void ACycle::Server_ForceTime_Implementation(float PercentageOfDay) {
	this->SetActorTickEnabled(false);
	
	TimeUntilNight = DayDuration * (1.0f - PercentageOfDay / 100.0f);
	OnRep_TimeUntilNight();
}

void ACycle::Server_ResumeTime_Implementation() {
	this->SetActorTickEnabled(true);
}

void ACycle::BeginPlay() {
	Super::BeginPlay();

	LastPercentage = 100.f;
}

void ACycle::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	GAME_STATE

	bool bHasSlowdownBuff;
	const float SlowdownPercentage = FMath::Clamp(GameState->GetBuffValue(100.f, EBuffType::SlowdownDay, bHasSlowdownBuff) - 100.f, 0.f, 100.f);

	if (bHasSlowdownBuff) {
		if (SlowdownPercentage < 100.f && SlowdownPercentage > 0.f) {
			TimeUntilNight -= DeltaSeconds * SlowdownPercentage / 100.f;
		} else {
			TimeUntilNight -= 0.f; // Multiply by 0 when the SlowdownPercentage is 100
		}
	} else {
		TimeUntilNight -= DeltaSeconds;
	}
	
	if (TimeUntilNight <= 0) {
		if (HasAuthority()) {
			UFarmionGameInstance* GameInst = Cast<UFarmionGameInstance>(GetGameInstance());
			check(GameInst);
			GameInst->UpdateCycle();
		}

		TimeUntilNight = DayDuration;
		LastPercentage = 100.f;
	}

	OnRep_TimeUntilNight();
}

void ACycle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACycle, TimeUntilNight);
	DOREPLIFETIME(ACycle, DayDuration);
}

void ACycle::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("DayDuration", DayDuration);
	JsonObject->SetNumberField("TimeUntilNight", TimeUntilNight);
	
	JsonObject->SetStringField("SkySphere", SkySphere.Get()->GetFName().ToString());
	JsonObject->SetStringField("Chameleon", Chameleon.Get()->GetFName().ToString());
}

void ACycle::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	TimeUntilNight = JsonObject->GetNumberField("TimeUntilNight");
	DayDuration = JsonObject->GetNumberField("DayDuration");

	const FString SkySphereName = JsonObject->GetStringField("SkySphere");
	const FString ChameleonName = JsonObject->GetStringField("Chameleon");
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		const AActor* Actor = *ActorItr;
		if (Actor->GetFName().ToString() == SkySphereName) {
			SkySphere = Actor;
			UE_LOG(LogTemp, Warning, TEXT("Found SkySphere: %s"), *Actor->GetName());
			FoundSkySphere(Actor);
		}

		if (Actor->GetFName().ToString() == ChameleonName) {
			Chameleon = Actor;
			UE_LOG(LogTemp, Warning, TEXT("Found Chameleon: %s"), *Actor->GetName());
		}

		if (SkySphere && Chameleon) {
			break;
		}
	}

	LastPercentage = 100.f;
}

ELoadMethod ACycle::LoadMethod() {
	return ELoadMethod::ManualReset;
}

void ACycle::StartWeatherEffect_Implementation(EWeatherEffect Effect) {
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		AFarmionPlayerController* FarmionPlayerController = Cast<AFarmionPlayerController>(It->Get());
		if (FarmionPlayerController && FarmionPlayerController->IsLocalController()) {
			FarmionPlayerController->StartWeatherEffect(Effect);
		}
	}
}

void ACycle::PostPerformCycle() {
	const int32 TypeOfWeather = FMath::RandRange(0, 100);

	GAME_STATE

	GameState->Day++;

	if (TypeOfWeather < GChance_For_WinterStorm) {
		bool bAnyPlantDied = false;
		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			AActor* Actor = *ActorItr;
			if (Actor->IsA<ACropPlant>()) {
				ACropPlant* Plant = Cast<ACropPlant>(Actor);
				if (!Plant->bSurvivesWinter) {
					Plant->SeedStage = ESeedStage::DeathStage;
					Plant->OnRep_SeedStage();

					bAnyPlantDied = true;
				}
			}
		}

		StartWeatherEffect(EWeatherEffect::Winter);
		GameState->ReceiveEventMessage_Tunnel(bAnyPlantDied ? FText::FromString(FString::Printf(TEXT("Day %d\nWinter storm! Some crops have died!"), GameState->Day)) : FText::FromString(FString::Printf(TEXT("Day %d\nWinter storm!"), GameState->Day)));
	} else if (TypeOfWeather < (GChance_For_WinterStorm + GChance_For_Rain)) {
		bool bAnyPlantWatered = false;
		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			AActor* Actor = *ActorItr;
			if (Actor->IsA<ACropPlant>()) {
				ACropPlant* Plant = Cast<ACropPlant>(Actor);
				Plant->bWatered = true;
				Plant->OnRep_Water();

				bAnyPlantWatered = true;
			}
		}

		GameState->ReceiveEventMessage_Tunnel(bAnyPlantWatered ? FText::FromString(FString::Printf(TEXT("Day %d\nIt has started to rain. Some crops have been watered!"), GameState->Day)) : FText::FromString(FString::Printf(TEXT("Day %d\nIt has started to rain."), GameState->Day)));
		StartWeatherEffect(EWeatherEffect::Rain);
	}
	else {
		GameState->ReceiveEventMessage_Tunnel(FText::FromString(FString::Printf(TEXT("Day %d\nIt's a sunny day."), GameState->Day)));
		StartWeatherEffect(EWeatherEffect::Day);
	}
}