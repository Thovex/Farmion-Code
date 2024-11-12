// Copyright 2020-2023: Jesse J. van Vliet

#include "PaperElement.h"

APaperElement::APaperElement() {
	
}

void APaperElement::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	const FPaperElementSaveData& Data = Save_BP();
	JsonObject->SetStringField("Text", Data.Text.ToString());
	JsonObject->SetNumberField("TextSize", Data.TextSize);
}

void APaperElement::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	FPaperElementSaveData Data;
	Data.Text = FText::FromString(JsonObject->GetStringField("Text"));
	Data.TextSize = JsonObject->GetIntegerField("TextSize");
	
	Load_BP(Data);
	
}
