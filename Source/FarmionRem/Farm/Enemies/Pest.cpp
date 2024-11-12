// Copyright 2020-2023: Jesse J. van Vliet

#include "Pest.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "Net/UnrealNetwork.h"

APest::APest() {
	PestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PestMesh"));
	PestMesh->SetupAttachment(RootComponent);

	PestMesh->SetGenerateOverlapEvents(true);
	PestMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PestMesh->SetCollisionProfileName(FName("BlockAllDynamic"));
}

void APest::OnRep_Death() {
	if (bDead) {
		PestMesh->PlayAnimation(DeathAnimation, false);
	}
}

void APest::BeginPlay() {
	Super::BeginPlay();

	PestMesh->PlayAnimation(bDead ? DeathAnimation : AliveAnimation, bDead ? false : true);

	OnRep_Death();	
}

int32 APest::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	const int32 Callback = Super::Interact(InteractInstigator, InteractionHit);

	if (UCommonLibrary::HasFlag(Callback, EInteractCallback::CARRYABLE_CAN_BE_CARRIED)) {
		if (!bDead) {
			bDead = true;
			OnRep_Death();
		}
	}
	
	return Callback;
}

void APest::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("bDead", bDead);
}

void APest::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bDead = JsonObject->GetBoolField("bDead");
	OnRep_Death();
}

void APest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APest, bDead);
}

void APest::PerformCycle() {
	// Pests don't die.
}

FText APest::GetInformationText_Implementation() const {
	FString Text = Super::GetInformationText_Implementation().ToString();
	Text += bDead ? "\n<Sub>Dead</>" : "";
	return FText::FromString(Text);
}
