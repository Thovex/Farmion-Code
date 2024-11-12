// Copyright 2020-2023: Jesse J. van Vliet


#include "ToolConverter.h"

AToolConverter::AToolConverter() {
	ToolDropLocation = CreateDefaultSubobject<USceneComponent>(TEXT("DropLocation"));
	ToolDropLocation->SetupAttachment(Transform);
}

bool AToolConverter::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	const AActor* Actor = Carryable->GetActorFromInterface();
	
	if (Actor->IsA(HarvestToolClass)) {
		return Super::MatchesSocketRequirements(Info, Carryable);
	}

	const FString ContextStr;

	bool bIsAnyInput = false;

	TArray<FConverterRecipeTableRow*> Rows;
	RecipeTable->GetAllRows<FConverterRecipeTableRow>(ContextStr, Rows);
	for (const FConverterRecipeTableRow* Row : Rows) {
		if (bIsAnyInput) {
			break;
		}
		
		for (const TSubclassOf<AInteractableObject> InputClass : Row->Input) {
			if (Actor->IsA(InputClass)) {
				bIsAnyInput = true;
				break;
			}
		}
	}

	if (bIsAnyInput) {
		return Super::MatchesSocketRequirements(Info, Carryable);
	}
	
	return false;
}

void AToolConverter::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	const FVector& Loc = ToolDropLocation->GetComponentLocation();
	JsonObject->SetNumberField("DropLocationX", Loc.X);
	JsonObject->SetNumberField("DropLocationY", Loc.Y);
	JsonObject->SetNumberField("DropLocationZ", Loc.Z);
}

void AToolConverter::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);
			
	ToolDropLocation->SetWorldLocation(FVector(
		JsonObject->GetNumberField("DropLocationX"),
		JsonObject->GetNumberField("DropLocationY"),
		JsonObject->GetNumberField("DropLocationZ")
	));
}

void AToolConverter::ProcessInputAfterConversion() {
	// Ignore Super::ProcessInputAfterConversion()

	for (auto& Socket : AvailableSockets) {
		AActor* Actor = Socket.Carryable->GetActorFromInterface();
		if (Actor->IsA(HarvestToolClass)) {
			Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Actor->SetActorLocationAndRotation(ToolDropLocation->GetComponentLocation(), ToolDropLocation->GetComponentRotation());
	
			Socket.Carryable->CarryableSocket = nullptr;
			Socket.Carryable->CarryableSocketIndex = 0;
		}

		Socket.Carryable = nullptr;
		Socket.Available = true;
	}

	// Mandatory
	SocketsInUse = GetSocketsInUse();

}

void AToolConverter::OnConversionStartInput() {
	Super::OnConversionStartInput();

	if (!bHideRest && !bHideTool) {
		return;
	}
	
	for (const auto& Socket : AvailableSockets) {
		if (!Socket.Available && Socket.Carryable) {
			AActor* Actor = Socket.Carryable->GetActorFromInterface();
			if (Actor->IsA(HarvestToolClass)) {
				if (bHideTool) {
					Actor->SetActorHiddenInGame(true);
				}
			} else {
				if (bHideRest) {
					Actor->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

void AToolConverter::OnConversionEndInput() {
	Super::OnConversionEndInput();

	if (!bHideRest && !bHideTool) {
		return;
	}

	for (const auto& Socket : AvailableSockets) {
		if (!Socket.Available && Socket.Carryable) {
			AActor* Actor = Socket.Carryable->GetActorFromInterface();
			if (Actor->IsA(HarvestToolClass)) {
				if (bHideTool) {
					Actor->SetActorHiddenInGame(false);
				}
			} else {
				if (bHideRest) {
					Actor->SetActorHiddenInGame(false);
				}
			}
		}
	}
}