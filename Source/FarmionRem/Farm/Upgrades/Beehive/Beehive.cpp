// Copyright 2020-2023: Jesse J. van Vliet

#include "Beehive.h"

#include "Components/SphereComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "Net/UnrealNetwork.h"

void ABeehive::OnRep_Honey_Implementation() {
	
}

ABeehive::ABeehive() {
	BeehiveAura = CreateDefaultSubobject<USphereComponent>(TEXT("BeehiveAura"));
	BeehiveAura->SetupAttachment(RootComponent);

	BeehiveAura->SetSphereRadius(1000.f);

	BeehiveAura->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BeehiveAura->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
}

void ABeehive::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		BeehiveAura->OnComponentBeginOverlap.AddDynamic(this, &ABeehive::OnBeehiveAuraBeginOverlap);
		BeehiveAura->OnComponentEndOverlap.AddDynamic(this, &ABeehive::OnBeehiveAuraEndOverlap);
		BeehiveAura->UpdateOverlaps();
	}
	
	OnRep_Honey();
}

void ABeehive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABeehive, bHoney);
}

int32 ABeehive::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	int32 Callback = ICarryable::MeetsCarryRequirements(Character);

	if (!bBought) {
		UCommonLibrary::ClearFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED);
	}

	return Callback;
}

bool ABeehive::IsSourceAvailable() {
	return bHoney;
}

void ABeehive::OnHoneyTimer() {
	if (!bHoney) {
		bHoney = true;
		OnRep_Honey();
	}
}

void ABeehive::UseSource() {
	bHoney = false;
	OnRep_Honey();

	GetWorld()->GetTimerManager().SetTimer(HoneyTimerHandle, this, &ABeehive::OnHoneyTimer, GHoney_Regeneration_Time, false);
}

void ABeehive::Build() {
	Super::Build();

	GetWorld()->GetTimerManager().SetTimer(HoneyTimerHandle, this, &ABeehive::OnHoneyTimer, GHoney_Regeneration_Time, false);
	BeehiveAura->UpdateOverlaps();
}

TEnumAsByte<ELiquids> ABeehive::GetLiquidType() {
	return ELiquids_Honey;
}

TEnumAsByte<EProductColor> ABeehive::GetLiquidColor() {
	return EProductColor_Yellow;
}

void ABeehive::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField(TEXT("Honey"), bHoney);
	JsonObject->SetNumberField(TEXT("HoneyTimer"), GetWorld()->GetTimerManager().GetTimerRemaining(HoneyTimerHandle));
}

void ABeehive::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bHoney = JsonObject->GetBoolField(TEXT("Honey"));
	OnRep_Honey();

	if (!bHoney) {
		GetWorld()->GetTimerManager().SetTimer(HoneyTimerHandle, this, &ABeehive::OnHoneyTimer, JsonObject->GetNumberField(TEXT("HoneyTimer")), false);
	}

	BeehiveAura->UpdateOverlaps();
}

void ABeehive::OnBeehiveAuraBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA<ACropPlant>()) {
		ACropPlant* CropPlant = Cast<ACropPlant>(OtherActor);
		CropPlant->BeehivePtr = this;
		CropPlant->bBeeAffected = true;
		CropPlant->OnRep_BeeAffected();
	}
}

void ABeehive::OnBeehiveAuraEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA<ACropPlant>()) {
		ACropPlant* CropPlant = Cast<ACropPlant>(OtherActor);
		CropPlant->BeehivePtr = nullptr;
		CropPlant->bBeeAffected = false;
		CropPlant->OnRep_BeeAffected();

		return;
	}
}

FText ABeehive::GetInformationText_Implementation() const {
	if (!bBought) {
		return FText::FromString(FString::Printf(TEXT("%s\n<Sub>Requires Construction</>\n<Sub>Needs %d Beehive%hs</>"),
			*UpgradeName.ToString(), BuildRequirementsNecessary.Num(), BuildRequirementsNecessary.Num() > 1 ? "s" : ""));
	}

	return FText::FromString(FString::Printf(TEXT("<Prod>%s</>\n<Sub>The bees are happy!</>"), *UpgradeName.ToString()));
}

int32 ABeehive::GetPriority() const {
	return -1;
}

void ABeehive::PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap) {
	Super::PostLoaded(JsonObject, UniqueIdToActorMap);
	BeehiveAura->UpdateOverlaps();
}