// Copyright 2020-2023: Jesse J. van Vliet

#include "CookingPot.h"

#include "FarmionRem/Interfaces/Interactable/Ingredient.h"
#include "Net/UnrealNetwork.h"

void ACookingPot::OnRep_Water_Implementation() {}
void ACookingPot::OnRep_Fuel_Implementation() {}
void ACookingPot::OnRep_Lit_Implementation() {}

void ACookingPot::BeginPlay() {
	Super::BeginPlay();

	OnRep_Water();
	OnRep_Fuel();
	OnRep_Lit();
}

bool ACookingPot::MatchesConditions() {
	return bWater && bFuel && bLit;
}

void ACookingPot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACookingPot, bWater);
	DOREPLIFETIME(ACookingPot, bFuel);
	DOREPLIFETIME(ACookingPot, bLit);
}

bool ACookingPot::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (const TScriptInterface<IIngredient> Ingredient = TScriptInterface<IIngredient>(Carryable->GetActorFromInterface())) {
		if (Ingredient->Execute_IsBoilable(Ingredient->_getUObject())) {
			return Super::MatchesSocketRequirements(Info, Carryable);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Ingredient %s is not boilable"), *Carryable->GetActorFromInterface()->GetName());
	}

	return false;
}

void ACookingPot::Output() {
	Super::Output();

	bWater = false;
	OnRep_Water();

	bFuel = false;
	OnRep_Fuel();

	bLit = false;
	OnRep_Lit();
}

void ACookingPot::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("bWater", bWater);
	JsonObject->SetBoolField("bFuel", bFuel);
	JsonObject->SetBoolField("bLit", bLit);
}

void ACookingPot::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bWater = JsonObject->GetBoolField("bWater");
	bFuel = JsonObject->GetBoolField("bFuel");
	bLit = JsonObject->GetBoolField("bLit");

	OnRep_Water();
	OnRep_Fuel();
	OnRep_Lit();
}

FText ACookingPot::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	if (bIsConverting) {
		FString Text = FString::Printf(TEXT("<Prod>%s</>"), *ToolName.ToString());
		Text += "\n<Sub>Boiling...</>";
		return FText::FromString(Text);
	}
	
	FString Text = Super::GetInformationText_Implementation().ToString();

	if (!bWater) { Text += "\n<Sub>Needs Water</>"; }
	if (!bFuel) { Text += "\n<Sub>Needs Fuel</>"; }
	if (!bLit) { Text += "\n<Sub>Needs to be Lit</>"; }

	return FText::FromString(Text);
}

bool ACookingPot::Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) {
	if (Liquid != ELiquids::ELiquids_Water) {
		return false;
	}

	bWater = true;
	OnRep_Water();

	TryConvert();
	
	return true;
}

bool ACookingPot::Supply(int32 FuelValue) {
	if (bFuel) {
		return false;
	}
	
	bFuel = true;
	OnRep_Fuel();

	TryConvert();
	return true;
}

bool ACookingPot::Light() {
	if (!bFuel && !bLit) {
		return false;
	}

	bLit = true;
	OnRep_Lit();
	
	TryConvert();
	return true;
}

int32 ACookingPot::GetPriority() const {
	return -1;
}