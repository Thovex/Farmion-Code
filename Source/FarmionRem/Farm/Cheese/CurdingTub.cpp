// Copyright 2020-2023: Jesse J. van Vliet

#include "CurdingTub.h"

#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"

ACurdingTub::ACurdingTub() {
	CheeseSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("CheeseSpawnLocation"));
	CheeseSpawnLocation->SetupAttachment(RootComponent);
}

bool ACurdingTub::Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) {
	if (LiquidType == ELiquids::ELiquids_None || LiquidType == Liquid) {
		if (LiquidType == ELiquids::ELiquids_None) {
			LiquidType = Liquid;
		}
		
		MilkCount += Charges;

		if (MilkCount >= RequiredMilkCount) {
			MilkCount = RequiredMilkCount;
		}

		UE_LOG(LogTemp, Warning, TEXT("MilkCount: %d"), MilkCount);
		OnRep_MilkCount();
		
		return true;
	}
	
	if (Liquid != LiquidType) {
		return false;
	}

	return false;
}

void ACurdingTub::BeginPlay() {
	Super::BeginPlay();

	OnRep_MilkCount();
}


void ACurdingTub::OnRep_MilkCount_Implementation() {
	
}

int32 ACurdingTub::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return Super::MeetsCarryRequirements(Character);
}


void ACurdingTub::PostPerformCycle() {
	if (MilkCount == RequiredMilkCount) {
		check(CheeseSpawnMap[CheeseType]); // Add the other types of cheese dummy.
		
		ACheese* NewCheese = GetWorld()->SpawnActor<ACheese>(CheeseSpawnMap[CheeseType], CheeseSpawnLocation->GetComponentLocation(), CheeseSpawnLocation->GetComponentRotation());
		const float Modifier = GET_DEFAULT_QUALITY_MODIFIER(Quality);

		GAME_STATE
		NewCheese->Quality = DetermineQualityWithModifier(GameState->CalculateFarmLevel(), Modifier);
		
		MilkCount = 0;
		OnRep_MilkCount();
	}
}

void ACurdingTub::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("MilkCount", MilkCount);
	JsonObject->SetNumberField("RequiredMilkCount", RequiredMilkCount);
	JsonObject->SetNumberField("LiquidType", LiquidType);
}

void ACurdingTub::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	MilkCount = JsonObject->GetNumberField("MilkCount");
	RequiredMilkCount = JsonObject->GetNumberField("RequiredMilkCount");
	LiquidType = static_cast<ELiquids>(JsonObject->GetNumberField("LiquidType"));

	OnRep_MilkCount();
}

FText ACurdingTub::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	FString Text = Super::GetInformationText_Implementation().ToString();
	Text += FString::Printf(TEXT("\n<Sub>%s, %hs</>"), *GetCheeseTypeDisplayName(CheeseType), MilkCount == RequiredMilkCount ? "Curding..." : "Needs Milk");
	return FText::FromString(Text);
}

void ACurdingTub::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACurdingTub, MilkCount);
	DOREPLIFETIME(ACurdingTub, RequiredMilkCount);
	DOREPLIFETIME(ACurdingTub, LiquidType);
}