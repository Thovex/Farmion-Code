// Copyright 2020-2023: Jesse J. van Vliet


#include "Flask.h"

#include "Net/UnrealNetwork.h"

void AFlask::OnRep_Liquid_Implementation() {}

void AFlask::OnRep_Color_Implementation() {}

void AFlask::OnRep_Charges_Implementation() {}

void AFlask::BeginPlay() {
	Super::BeginPlay();

	OnRep_Color();
	OnRep_Charges();
	OnRep_Liquid();
}

bool AFlask::Supply(TEnumAsByte<ELiquids> SuppliedLiquid, TEnumAsByte<EProductColor> SuppliedColor, int32 SuppliedCharges) {
	if (Charges >= MaxCharges) {
		return false;		
	}
	
	if (SuppliedLiquid == ELiquids_None) {
		return false;
	}

	if (this->Liquid != ELiquids_None) {
		return false;
	}
	
	this->Liquid = SuppliedLiquid;
	this->Color = SuppliedColor;
	this->Charges += SuppliedCharges;

	if (this->Charges > this->MaxCharges) {
		this->Charges = this->MaxCharges;
	}

	OnRep_Charges();
	OnRep_Liquid();
	OnRep_Color();
	
	return true;
}

void AFlask::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("Charges", Charges);
	JsonObject->SetNumberField("MaxCharges", MaxCharges);
	JsonObject->SetNumberField("Liquid", Liquid);
	JsonObject->SetNumberField("Color", Color);
}

void AFlask::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	Charges = JsonObject->GetNumberField("Charges");
	MaxCharges = JsonObject->GetNumberField("MaxCharges");
	Liquid = static_cast<ELiquids>(JsonObject->GetNumberField("Liquid"));
	Color = static_cast<EProductColor>(JsonObject->GetNumberField("Color"));

	OnRep_Charges();
	OnRep_Liquid();
	OnRep_Color();
}

void AFlask::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlask, Charges);
	DOREPLIFETIME(AFlask, MaxCharges);
	DOREPLIFETIME(AFlask, Liquid);
	DOREPLIFETIME(AFlask, Color);
}