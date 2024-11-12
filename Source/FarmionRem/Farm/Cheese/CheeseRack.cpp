// Copyright 2020-2023: Jesse J. van Vliet

#include "CheeseRack.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Cheese/Cheese.h"

ACheeseRack::ACheeseRack() {
	ToolName = "Cheese Rack";
}

int32 ACheeseRack::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	if (SocketsInUse != 0) {
		return (int32)EInteractCallback::NO_CALLBACK;
	}
	
	return Super::MeetsCarryRequirements(Character);
}

void ACheeseRack::PerformCycle() {
	for (const auto& Socket : AvailableSockets) {
		if (!Socket.Available) {
			ACheese* Cheese = Cast<ACheese>(Socket.Carryable->GetActorFromInterface());
			Cheese->UpdateAge(Quality, this->AreaType);
			UE_LOG(LogTemp, Warning, TEXT("Cheese aged!"));
		}
	}
}

void ACheeseRack::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("AreaType", static_cast<uint8>(AreaType));
}

void ACheeseRack::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	AreaType = static_cast<ECheeseAreaType>(JsonObject->GetNumberField("AreaType"));
}

FText ACheeseRack::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();	
	}
	
	FString Text = Super::GetInformationText_Implementation().ToString();
	Text = Text.Replace(*ToolName.ToString(), *FString::Printf(TEXT("<Prod>%s</>"), *ToolName.ToString()));

	if (bBought) {
		switch (AreaType) {
		case ECheeseAreaType::ECheeseAreaType_Outside: {
			Text += "\n<Sub>Outside</>";
		} break;
		case ECheeseAreaType::ECheeseAreaType_Inside: {
			Text += "\n<Sub>Inside</>";
		} break;
		case ECheeseAreaType::ECheeseAreaType_Cellar: {
			Text += "\n<Sub>Cellar</>";
		} break;
		}
	}

	return FText::FromString(Text);
}

bool ACheeseRack::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {

	if (Carryable->GetActorFromInterface()->IsA<ACheese>()) {
		return Super::MatchesSocketRequirements(Info, Carryable);
	}

	return false;
}
