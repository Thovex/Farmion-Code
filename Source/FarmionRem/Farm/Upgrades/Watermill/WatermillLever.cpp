#include "WatermillLever.h"

#include "Watermill.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "Net/UnrealNetwork.h"

AWatermillLever::AWatermillLever() {
	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

int32 AWatermillLever::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

int32 AWatermillLever::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return bPulled ? (int32)EInteractCallback::STOP_USING_OBJECT : (int32)EInteractCallback::START_USING_OBJECT;
}

void AWatermillLever::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("Pulled", bPulled);
}

void AWatermillLever::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bPulled = JsonObject->GetBoolField("Pulled");
	OnRep_Pulled();
}

ELoadMethod AWatermillLever::LoadMethod() {
	return ELoadMethod::Recreate;
}

void AWatermillLever::Server_Use(AFarmionCharacter* User, const bool bUse) {
	bPulled = !bPulled;
	OnRep_Pulled();

	Watermill->bRotating = !Watermill->bRotating;
	Watermill->OnRep_Rotating();
}

FText AWatermillLever::GetInformationText_Implementation() const {
	return FText::FromString("Lever");
}

void AWatermillLever::Link() {
	Watermill = Cast<AWatermill>(Linker->GetLinkedActor(TEXT("Watermill")));
}

void AWatermillLever::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWatermillLever, bPulled);
}