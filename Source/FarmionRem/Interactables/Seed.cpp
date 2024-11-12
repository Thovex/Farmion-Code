// Copyright 2020-2023: Jesse J. van Vliet

#include "Seed.h"

#include "Components/SphereComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Land/FarmLand.h"
#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Farm/Tools/IgnoreDecayVolume.h"
#include "FarmionRem/Interfaces/Interactable/Carryable.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ASeed::ASeed() {
	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	SeedBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SeedBox"));
	SeedBox->SetupAttachment(Pivot);

	SeedCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SeedCollision"));
	SeedCollision->SetupAttachment(Pivot);

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(5.0f);

	ThisAsCarryable = TScriptInterface<ICarryable>(this);
}

bool ASeed::FarmLandCollisionCheck() {
	const FVector ActorLocation = GetActorLocation();
	bool bHitFarmLand = false;
	
	TArray<FHitResult> Hits;
	if (UKismetSystemLibrary::LineTraceMulti(this, ActorLocation, ActorLocation - FVector(0, 0, 100.f), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, {}, EDrawDebugTrace::Type::None, Hits, true)) {
		for (const FHitResult& Hit : Hits) {
			if (Hit.GetActor()->IsA(AFarmLand::StaticClass())) {
				CurrentFarmCollidingPos = Hit.ImpactPoint;
				FarmLandPtr = Hit.GetActor();

				bHitFarmLand = true;
				break;
			}
		}
	}

	if (!bHitFarmLand) {
		return false;
	}

	Hits.Empty();
	if (UKismetSystemLibrary::SphereTraceMulti(this, ActorLocation, ActorLocation, SeedCollision->GetScaledSphereRadius(), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, {}, EDrawDebugTrace::Type::None, Hits, true)) {
		for (const FHitResult& Hit : Hits) {
			if (Hit.GetActor()->IsA(ACropPlant::StaticClass())) {
				return false;
			}
		}
	}

	return true;
}

void ASeed::Multicast_SeedError_Implementation() {
	Multicast_SeedError_BP();
}

void ASeed::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!HasAuthority()) {
		SetActorTickEnabled(false);
		UE_LOG(LogTemp, Log, TEXT("Disabling Tick on %s for Client."), *GetClass()->GetName());
		return;
	}

	// Disable planting on non-crop seeds.
	if (!(this->SeedType > ESeed::DO_NOT_USE_CROP_START && this->SeedType < ESeed::DO_NOT_USE_CROP_END)) {
		SetActorTickEnabled(false);
		UE_LOG(LogTemp, Log, TEXT("Disabling Tick on %s for Client."), *GetClass()->GetName());
		return;
	}

	if (ThisAsCarryable->bIsBeingCarried) {
		ResetFarmLandCollisionCheck();
		return;
	}

	if (FarmLandCollisionCheck()) {
		FarmLandPtr->AddSeed(CurrentFarmCollidingPos, GetActorRotation(), GrowthTableId);
		this->Destroy(true);
	} else {
		ResetFarmLandCollisionCheck();
	}
}

void ASeed::PerformCycle() {
	this->Destroy(true);
}

int32 ASeed::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	const int32 Callback = Super::MeetsCarryRequirements(Character);

	if (UCommonLibrary::HasFlag(Callback, EInteractCallback::CARRYABLE_CAN_BE_CARRIED)) {
		SetActorTickEnabled(true);
	}

	return Callback;
}

int32 ASeed::MeetsDropRequirements(const AFarmionCharacter* Character) {
	const int32 Callback = Super::MeetsDropRequirements(Character);
	return Callback;
}

EDecayFlags ASeed::GetDecayFlag() {
	return EDecayFlags::Seed;
}

FText ASeed::GetInformationText_Implementation() const {
	return FText::FromString(FString::Printf(TEXT("Seedbox\n<Sub>%s</>"), *GetSeedDisplayName(SeedType)));
}

FText ASeed::GetMiniInformationText_Implementation() const {
	return FText::FromString(FString::Printf(TEXT("%s seedbox"), *GetSeedDisplayName(SeedType)));
}

void ASeed::ResetFarmLandCollisionCheck() {
	CurrentFarmCollidingPos = FVector::ZeroVector;

	if (FarmLandPtr) {
		Multicast_SeedError();
	}
	FarmLandPtr = nullptr;
}