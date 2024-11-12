// Copyright 2020-2023: Jesse J. van Vliet


#include "PlanterPot.h"

#include "Ground.h"
#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Interactables/Seed.h"
#include "Net/UnrealNetwork.h"

APlanterPot::APlanterPot() {



}

void APlanterPot::OnRep_Ground_Implementation() {}

void APlanterPot::FinishPlant(const ACropPlant* Plant)
{
	Super::FinishPlant(Plant);

	this->bGround = false;
	this->OnRep_Ground();
}

void APlanterPot::OverlapPlot(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (bBought) {
	
		if (OtherActor->IsA<AGround>() && !bGround) {
			OtherActor->Destroy(true);
			bGround = true;
			OnRep_Ground();
			return;
		}

		if (!bGround) {
			return;
		}

		Super::OverlapPlot(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		
	}
}

void APlanterPot::BeginPlay()
{
	Super::BeginPlay();
	OnRep_Ground();
	
}


void APlanterPot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlanterPot,bGround);
	
}

void APlanterPot::Save(const TSharedPtr<FJsonObject>& JsonObject)
{
	Super::Save(JsonObject);
	JsonObject->SetBoolField("Ground", bGround);
}

void APlanterPot::Load(const TSharedPtr<FJsonObject>& JsonObject)
{
	Super::Load(JsonObject);
	
	bGround = JsonObject->GetBoolField("Ground");
	OnRep_Ground();
	
}

FText APlanterPot::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	FString Text = FString::Printf(TEXT("<Prod>%s</>\n"), *ToolName.ToString());

	if (!bGround) {
		Text += "<Sub>Requires Soil</>";
		return FText::FromString(Text);
	}

	if (bGround && Seed == ESeed::None) {
		Text += "<Sub>Requires Seed</>";
		return FText::FromString(Text);
	}

	if (FarmEntries.IsEmpty()) {
		return FText::FromString(Text);
	}

	Text += FString::Printf(TEXT("<Sub>%s</>"), *GetSeedStageDisplayName(FarmEntries[0].Plant->SeedStage));
	return FText::FromString(Text);
}
