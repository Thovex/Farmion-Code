// Copyright 2020-2023: Jesse J. van Vliet


#include "Tutorial.h"

#include "FarmionRem/Callbacks.h"
#include "JsonUtils/JsonPointer.h"
#include "Net/UnrealNetwork.h"

ATutorial::ATutorial() {
	bAlwaysRelevant = true;
}

void ATutorial::OnRep_SegmentIndex_Implementation() {}

int32 ATutorial::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return 0;
}

int32 ATutorial::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return (int32)EInteractCallback::START_USING_OBJECT;
}

void ATutorial::Server_Use(AFarmionCharacter* User, bool bUse) {
	if (bUse) {
		if (PassesRequirements()) {
			
			SegmentIndex++;
			OnRep_SegmentIndex();
		}
	}
}

void ATutorial::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATutorial, SegmentIndex);
}

FText ATutorial::GetInformationText_Implementation() const {
	return InfoText;
}

FText ATutorial::GetMiniInformationText_Implementation() const {
	return FText::FromString("Tutorial");
}

void ATutorial::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("SegmentIndex", SegmentIndex);
	JsonObject->SetBoolField("bCompleted", bCompleted);
}

void ATutorial::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bCompleted = JsonObject->GetBoolField("bCompleted");
	SegmentIndex = JsonObject->GetIntegerField("SegmentIndex");
	OnRep_SegmentIndex();
}
