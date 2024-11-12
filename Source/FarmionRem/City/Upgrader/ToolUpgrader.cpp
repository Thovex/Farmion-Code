// Copyright 2020-2023: Jesse J. van Vliet

#include "ToolUpgrader.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Interactables/Tool.h"
#include "Net/UnrealNetwork.h"

int32 AToolUpgrader::CanUpgradeTool() {
	if (bIsUpgrading) {
		return (int32)ECanUpgradeToolCallback::UPGRADE_IN_PROGRESS;
	}
	
	if (SocketsInUse == 0) {
		return (int32)ECanUpgradeToolCallback::PLACE_TOOL_IN_SOCKET;
	}

	if (UpgradeToolQuality == EItemQuality::Eldar) {
		return (int32)ECanUpgradeToolCallback::TOOL_AT_MAX_QUALITY;
	}

	return (int32)ECanUpgradeToolCallback::CAN_UPGRADE;
}

void AToolUpgrader::StartUpgrade(bool bCanDowngrade) {
	bIsUpgrading = true;
	OnRep_IsUpgrading();
	
	bSavedCanDowngrade = bCanDowngrade;
	GetWorld()->GetTimerManager().SetTimer(UpgradeTimerHandle, this, &AToolUpgrader::UpgradeTool, UpgradeTime, false);
}

void AToolUpgrader::OnRep_IsUpgrading_Implementation() {}

void AToolUpgrader::UpgradeTool() {
	GAME_STATE
	
	AInteractableObject* InteractableObject = Cast<AInteractableObject>(AvailableSockets[0].Carryable->GetActorFromInterface());
	const EItemQuality NewQuality = DetermineQuality(GameState->CalculateFarmLevel());
	
	if (bSavedCanDowngrade) {
		const bool bIsBetter = NewQuality > InteractableObject->Quality;
		const bool bIsWorse = NewQuality < InteractableObject->Quality;

		InteractableObject->Quality = NewQuality;
		UpgradeToolQuality = NewQuality;

		if (bIsBetter) {
			Multicast_UpgradeTool(EUpgradeToolCallback::BETTER_QUALITY);
		}

		if (bIsWorse) {
			Multicast_UpgradeTool(EUpgradeToolCallback::WORSE_QUALITY);
		}

		if (!bIsBetter && !bIsWorse) {
			Multicast_UpgradeTool(EUpgradeToolCallback::SAME_QUALITY);
		}
	} else {
		if (NewQuality > InteractableObject->Quality) {
			InteractableObject->Quality = NewQuality;
			UpgradeToolQuality = NewQuality;

			Multicast_UpgradeTool(EUpgradeToolCallback::BETTER_QUALITY);
		} else {
			Multicast_UpgradeTool(EUpgradeToolCallback::SAME_QUALITY);
		}
	}

	bIsUpgrading = false;
	OnRep_IsUpgrading();
}

void AToolUpgrader::Multicast_UpgradeTool_Implementation(EUpgradeToolCallback Callback) {
	Multicast_UpgradeTool_BP(Callback);
}

int32 AToolUpgrader::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

void AToolUpgrader::SetSocketAvailability(const TScriptInterface<ICarryable>& Carryable, const int32 Index, const bool Availability) {
	Super::SetSocketAvailability(Carryable, Index, Availability);

	if (!Availability) {
		const AInteractableObject* InteractableObject = Cast<AInteractableObject>(Carryable->GetActorFromInterface());
		const ATool* Tool = Cast<ATool>(InteractableObject);

		UpgradeToolQuality = InteractableObject->Quality;
		UpgradeToolName = Tool->ToolName;
	} else {
		UpgradeToolQuality = EItemQuality::None;
		UpgradeToolName = NAME_None;
	}
}

bool AToolUpgrader::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (Carryable->GetActorFromInterface()->IsA<ATool>()) {
		return Super::MatchesSocketRequirements(Info, Carryable);
	}

	return false;
}

void AToolUpgrader::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField(TEXT("IsUpgrading"), bIsUpgrading);
	JsonObject->SetBoolField(TEXT("CanDowngrade"), bSavedCanDowngrade);

	JsonObject->SetNumberField(TEXT("UpgradeTimeRemaining"), GetWorld()->GetTimerManager().GetTimerRemaining(UpgradeTimerHandle));
	
	JsonObject->SetNumberField(TEXT("UpgradeToolQuality"), (int32)UpgradeToolQuality);
	JsonObject->SetStringField(TEXT("UpgradeToolName"), *UpgradeToolName.ToString());
}

void AToolUpgrader::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bIsUpgrading = JsonObject->GetBoolField(TEXT("IsUpgrading"));
	OnRep_IsUpgrading();
	
	bSavedCanDowngrade = JsonObject->GetBoolField(TEXT("CanDowngrade"));
	
	UpgradeToolQuality = (EItemQuality)JsonObject->GetNumberField(TEXT("UpgradeToolQuality"));
	UpgradeToolName = *JsonObject->GetStringField(TEXT("UpgradeToolName"));
	
	GetWorld()->GetTimerManager().SetTimer(UpgradeTimerHandle, this, &AToolUpgrader::UpgradeTool, JsonObject->GetNumberField(TEXT("UpgradeTimeRemaining")), false);
}

void AToolUpgrader::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AToolUpgrader, bIsUpgrading);
	DOREPLIFETIME(AToolUpgrader, UpgradeToolQuality);
	DOREPLIFETIME(AToolUpgrader, UpgradeToolName);
}

FText AToolUpgrader::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();	
	}

	FString Text = *ToolName.ToString();
	if (SocketsInUse != 0 && UpgradeToolName != NAME_None) {
		Text += FString::Printf(TEXT("\n<Sub>Target: %s</>"), *UpgradeToolName.ToString());
	}

	return FText::FromString(Text);
}