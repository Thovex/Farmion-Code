#include "IrrigationSystemWaterContainer.h"

#include "IrrigationSystemLever.h"
#include "Components/BoxComponent.h"
#include "FarmionRem/Callbacks.h"
#include "Net/UnrealNetwork.h"


AIrrigationSystemWaterContainer::AIrrigationSystemWaterContainer() {
	WaterInputCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterInputCollision"));
	WaterInputCollision->SetupAttachment(RootComponent);
}

void AIrrigationSystemWaterContainer::OnRep_WaterCharges_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("[WaterCharges] RepNotify: [%d]"), WaterCharges);
}

int32 AIrrigationSystemWaterContainer::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

bool AIrrigationSystemWaterContainer::Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) {
	if (Liquid != ELiquids::ELiquids_Water) {
		UE_LOG(LogTemp, Log, TEXT("Trying to Supply with wrong Liquid: [%d]"), (int32)Liquid);
		return false;
	}
	
	UE_LOG(LogTemp, Log, TEXT("[WaterCharges] Supply: [%d]"), WaterCharges);
	
	WaterCharges++;
	OnRep_WaterCharges();

	if (WaterCharges >= RequiredWaterCharges) {
		UE_LOG(LogTemp, Log, TEXT("[WaterCharges] Enough WaterCharges"));
	}

	return true;
}

void AIrrigationSystemWaterContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIrrigationSystemWaterContainer, WaterCharges);
	DOREPLIFETIME(AIrrigationSystemWaterContainer, RequiredWaterCharges);
}

void AIrrigationSystemWaterContainer::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);
	
	JsonObject->SetNumberField("WaterCharges", WaterCharges);
	JsonObject->SetNumberField("RequiredWaterCharges", RequiredWaterCharges);
}

void AIrrigationSystemWaterContainer::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	WaterCharges = JsonObject->GetNumberField("WaterCharges");
	OnRep_WaterCharges();
	
	RequiredWaterCharges = JsonObject->GetNumberField("RequiredWaterCharges");
}

ELoadMethod AIrrigationSystemWaterContainer::LoadMethod() {
	return ELoadMethod::Recreate;
}

FText AIrrigationSystemWaterContainer::GetInformationText_Implementation() const {
	if (WaterCharges == RequiredWaterCharges) {
		return FText::FromString("Capacity\n<Sub>Full</>");
	}
	
	return FText::FromString(FString::Printf(TEXT("Capacity\n<Sub>%d/%d</>"), WaterCharges, RequiredWaterCharges));
}

