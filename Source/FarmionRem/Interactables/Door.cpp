// Copyright 2020-2023: Jesse J. van Vliet

#include "Door.h"

#include "LinkerComponent.h"
#include "FarmionRem/Callbacks.h"
#include "Net/UnrealNetwork.h"

ADoor::ADoor() {
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

void ADoor::BeginPlay() {
	Super::BeginPlay();

	Link();
}

int32 ADoor::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

int32 ADoor::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return bOpen ? (int32)EInteractCallback::START_USING_OBJECT : (int32)EInteractCallback::STOP_USING_OBJECT;
}

void ADoor::Server_Use(AFarmionCharacter* User, bool bUse) {
	bOpen = !bOpen;
	OnRep_Open();

	if (ConnectedDoor) {
		ConnectedDoor->bOpen = bOpen;
		ConnectedDoor->OnRep_Open();
	}
}

void ADoor::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("Open", bOpen);
	JsonObject->SetNumberField("OpenAngle", OpenAngle);
}

void ADoor::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bOpen = JsonObject->GetBoolField("Open");
	OpenAngle = JsonObject->GetNumberField("OpenAngle");
	OnRep_Open();
}

void ADoor::Link() {
	if (Linker->Links.IsEmpty()) {
		return;
	}

	ConnectedDoor = Linker->GetLinkedActor("ConnectedDoor");
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, bOpen);
	DOREPLIFETIME(ADoor, OpenAngle);
}
