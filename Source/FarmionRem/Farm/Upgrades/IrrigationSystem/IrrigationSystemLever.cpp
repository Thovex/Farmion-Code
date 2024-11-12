#include "IrrigationSystemLever.h"

#include "IrrigationSystemUpgrade.h"
#include "IrrigationSystemWaterContainer.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "Net/UnrealNetwork.h"

AIrrigationSystemLever::AIrrigationSystemLever() {
	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

int32 AIrrigationSystemLever::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

int32 AIrrigationSystemLever::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return bPulled ? (int32)EInteractCallback::STOP_USING_OBJECT : (int32)EInteractCallback::START_USING_OBJECT;
}

void AIrrigationSystemLever::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("Pulled", bPulled);
}

void AIrrigationSystemLever::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bPulled = JsonObject->GetBoolField("Pulled");
	OnRep_Pulled();
}

ELoadMethod AIrrigationSystemLever::LoadMethod() {
	return ELoadMethod::Recreate;
}

void AIrrigationSystemLever::Server_Use(AFarmionCharacter* User, const bool bUse) {
	bPulled = !bPulled;
	OnRep_Pulled();

	if (!WaterContainer.IsValid() || !Upgrade.IsValid()) {
		return;
	}

	if (WaterContainer->WaterCharges >= WaterContainer->RequiredWaterCharges) {
		WaterContainer->WaterCharges = 0;
		WaterContainer->OnRep_WaterCharges();
		Upgrade->Multicast_Generic_Networked_Call();
	}
}

FText AIrrigationSystemLever::GetInformationText_Implementation() const {
	return FText::FromString("Lever");
}

void AIrrigationSystemLever::Link() {
	WaterContainer = Cast<AIrrigationSystemWaterContainer>(Linker->GetLinkedActor(TEXT("IrrigationSystem_WaterContainer")));
	Upgrade = Cast<AIrrigationSystemUpgrade>(Linker->GetLinkedActor(TEXT("IrrigationSystem_Upgrade")));
}


void AIrrigationSystemLever::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIrrigationSystemLever, bPulled);
}
