// Copyright 2020-2023: Jesse J. van Vliet


#include "Cheese.h"

#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"

ACheese::ACheese() {
	PrimaryActorTick.bCanEverTick = false;

	CheeseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheeseMesh"));
	CheeseMesh->SetupAttachment(RootComponent);
}

void ACheese::OnRep_CheeseName_Implementation() {
	
	CheeseMesh->SetStaticMesh(CheeseVariants.FindByPredicate([this](const FCheeseVariant& CheeseVariant) {
		return CheeseVariant.Name == CurrentCheeseName;
	})->Mesh);
}

void ACheese::UpdateAge(const EItemQuality RackQuality, const ECheeseAreaType CurrentAreaType) {
	CurrentCheeseAge++;
	
	TArray<FCheeseVariant> PotentialVariants;
	for (const FCheeseVariant& CheeseVariant : CheeseVariants) {
		if (CheeseVariant.PreviousName == CurrentCheeseName
			&& CheeseVariant.AreaType == CurrentAreaType
			&& CheeseVariant.RequiredCycles >= CurrentCheeseAge) {
			PotentialVariants.Add(CheeseVariant);
			UE_LOG(LogTemp, Log, TEXT("Potential Variant: %s"), *CheeseVariant.Name.ToString());
		}
	}

	GAME_STATE
	const float Modifier = GET_DEFAULT_QUALITY_MODIFIER(RackQuality);

	for (const FCheeseVariant& CheeseVariant : PotentialVariants) {
		UE_LOG(LogTemp, Log, TEXT("Age: %d. Needs: %d to Transform to: %s"), CurrentCheeseAge, CheeseVariant.RequiredCycles, *CheeseVariant.Name.ToString());
		if (CheeseVariant.RequiredCycles == CurrentCheeseAge) {
			CurrentCheeseName = CheeseVariant.Name;
			OnRep_CheeseName();

			CurrentCheeseAge = 0;
			UE_LOG(LogTemp, Log, TEXT("Transformed to: %s. Quality Mod: %f"), *CheeseVariant.Name.ToString(), Modifier);

			// Acquire a new Quality on Transformation.
			Quality = DetermineQualityWithModifier(GameState->CalculateFarmLevel(), Modifier);
			return;
		}
	}

	if (PotentialVariants.IsEmpty() && CurrentCheeseName != FName("Generic Gouda")) {
		CurrentCheeseName = FName("Generic Gouda");
		OnRep_CheeseName();

		// Acquire a new Quality on Transformation.
		Quality = DetermineQualityWithModifier(GameState->CalculateFarmLevel(), Modifier);
		UE_LOG(LogTemp, Log, TEXT("Transformed to: %s. Quality Mod: %f"), *CurrentCheeseName.ToString(), Modifier);
	}
}

void ACheese::BeginPlay() {
	Super::BeginPlay();

	OnRep_CheeseName();
}

void ACheese::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetStringField(TEXT("CurrentCheeseName"), CurrentCheeseName.ToString());
	JsonObject->SetNumberField(TEXT("CurrentCheeseAge"), CurrentCheeseAge);
}

void ACheese::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	CurrentCheeseName = FName(JsonObject->GetStringField(TEXT("CurrentCheeseName")));
	OnRep_CheeseName();
	
	CurrentCheeseAge = JsonObject->GetNumberField(TEXT("CurrentCheeseAge"));
}

void ACheese::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACheese, CurrentCheeseAge);
	DOREPLIFETIME(ACheese, CurrentCheeseName);
}

FText ACheese::GetInformationText_Implementation() const {
	return FText::FromString(FString::Printf(TEXT("<Prod>%s</>\n<Sub>%hs</>"), *CurrentCheeseName.ToString(), CurrentCheeseAge == 0 ? "Young" : "Old"));
}



