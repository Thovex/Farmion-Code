// Copyright 2020-2023: Jesse J. van Vliet

#include "Product.h"

#include "FarmionRem/Farm/Tools/IgnoreDecayVolume.h"

AProduct::AProduct()
	: AInteractableObject() {

}

void AProduct::Sell() {
	if (SellVfx) {
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* VfxActor = GetWorld()->SpawnActor<AActor>(SellVfx, this->GetActorLocation(), this->GetActorRotation(), SpawnParameters);
		VfxActor->SetLifeSpan(5.0f);
	}
}

void AProduct::PerformCycle() {
	this->Destroy(true);
}

EDecayFlags AProduct::GetDecayFlag() {
	return EDecayFlags::Produce;
}

FText AProduct::GetInformationText_Implementation() const {
	return FText::FromString(FString::Printf(TEXT("<Prod>%s</>"), *ProductName.ToString()));
}

FText AProduct::GetMiniInformationText_Implementation() const {
	return FText::FromString(ProductName.ToString());
}

int32 AProduct::GetPriority() const {
	return 1; 
}