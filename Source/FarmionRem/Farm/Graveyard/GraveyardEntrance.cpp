// Copyright 2020-2023: Jesse J. van Vliet


#include "GraveyardEntrance.h"

#include "Grave.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AGraveyardEntrance::OnRep_CurrentGraves_Implementation() {
	
}

void AGraveyardEntrance::OnRep_GraveyardEverOpened_Implementation() {
	
}

void AGraveyardEntrance::OnGraveActiveChanged(const bool bActive) {
	CurrentGraves += bActive ? 1 : -1;
	OnRep_CurrentGraves();

	if (!bGraveyardEverOpened && bActive) {
		bGraveyardEverOpened = true;
		OnRep_GraveyardEverOpened();
	}
}

void AGraveyardEntrance::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(this, AGrave::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors) {
			AGrave* Grave = Cast<AGrave>(Actor);
			Grave->OnGraveActiveChanged.AddDynamic(this, &AGraveyardEntrance::OnGraveActiveChanged);
		}
	}
}

void AGraveyardEntrance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGraveyardEntrance, CurrentGraves);
	DOREPLIFETIME(AGraveyardEntrance, bGraveyardEverOpened);
}

void AGraveyardEntrance::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("CurrentGraves", CurrentGraves);
	JsonObject->SetBoolField("GraveyardEverOpened", bGraveyardEverOpened);
}

void AGraveyardEntrance::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	CurrentGraves = JsonObject->GetIntegerField("CurrentGraves");
	bGraveyardEverOpened = JsonObject->GetBoolField("GraveyardEverOpened");

	OnRep_CurrentGraves();
	OnRep_GraveyardEverOpened();
}
