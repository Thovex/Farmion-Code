// Copyright 2020-2023: Jesse J. van Vliet


#include "CookingSpit.h"

#include "FarmionRem/Interfaces/Interactable/Ingredient.h"
#include "Net/UnrealNetwork.h"

ACookingSpit::ACookingSpit() {}

void ACookingSpit::OnRep_Fuel_Implementation() {}
void ACookingSpit::OnRep_Lit_Implementation() {}

void ACookingSpit::BeginPlay() {
	Super::BeginPlay();

	OnRep_Fuel();
	OnRep_Lit();
}

bool ACookingSpit::MatchesConditions() {
	return bFuel && bLit;
}

bool ACookingSpit::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (const TScriptInterface<IIngredient> Ingredient = TScriptInterface<IIngredient>(Carryable->GetActorFromInterface())) {
		if (Ingredient->Execute_IsSpittable(Ingredient->_getUObject())) {
			return Super::MatchesSocketRequirements(Info, Carryable);
		}

		UE_LOG(LogTemp, Warning, TEXT("Ingredient %s is not spittable"), *Carryable->GetActorFromInterface()->GetName());
	}

	return false;
}

void ACookingSpit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACookingSpit, bFuel);
	DOREPLIFETIME(ACookingSpit, bLit);
}

void ACookingSpit::ManipulateSpawnedObject(AInteractableObject* SpawnedObject) {
	this->SetSocketAvailability(SpawnedObject, 0, false);
	
	FVector SocketOrigin, SocketBoundsExtent;
	AvailableSockets[0].GetSocketBounds(SocketOrigin, SocketBoundsExtent);

	SpawnedObject->CarryableSocket = this;
	SpawnedObject->CarryableSocketIndex = 0;
	SpawnedObject->CarryTarget = AvailableSockets[0].Component;
	SpawnedObject->bMustWeld = true;

	SpawnedObject->OffsetZ = SocketBoundsExtent.Z;
	
	FTransform EndTransform = AvailableSockets[0].Component->GetComponentTransform();
	EndTransform.SetLocation(EndTransform.GetLocation() - (FVector( 0, 0, SocketBoundsExtent.Z)));

	SpawnedObject->SetActorTransform(EndTransform);
}

void ACookingSpit::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("bFuel", bFuel);
	JsonObject->SetBoolField("bLit", bLit);
}

void ACookingSpit::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bFuel = JsonObject->GetBoolField("bFuel");
	bLit = JsonObject->GetBoolField("bLit");

	OnRep_Fuel();
	OnRep_Lit();
}

bool ACookingSpit::Supply(int32 FuelValue) {
	if (bFuel) {
		return false;
	}
	
	bFuel = true;
	OnRep_Fuel();

	TryConvert();
	return true;
}

bool ACookingSpit::Light() {
	if (!bFuel && !bLit) {
		return false;
	}

	bLit = true;
	OnRep_Lit();
	
	TryConvert();
	return true;
}

int32 ACookingSpit::GetPriority() const {
	return -1;
}

void ACookingSpit::Output() {
	// Do not call Super::()
	
	bIsConverting = false;
	OnRep_IsConverting();

	ProcessInputAfterConversion();

	SpawnOutput();

	SocketsInUse = GetSocketsInUse();
	
	bFuel = false;
	OnRep_Fuel();

	bLit = false;
	OnRep_Lit();
}