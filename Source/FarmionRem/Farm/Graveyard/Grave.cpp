// Copyright 2020-2023: Jesse J. van Vliet


#include "Grave.h"

#include "Flower.h"
#include "Components/BoxComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"


AGrave::AGrave() {
	GraveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GraveMesh"));
	GraveMesh->SetupAttachment(RootComponent);

	GraveActiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GraveActiveMesh"));
	GraveActiveMesh->SetupAttachment(GraveMesh);

	FlowerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("FlowerArea"));
	FlowerArea->SetupAttachment(GraveMesh);
	
	Nameplate = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Nameplate"));
	Nameplate->SetupAttachment(GraveMesh);
}

void AGrave::BeginPlay() {
	Super::BeginPlay();
	
	OnRep_ActiveGrave();
	OnRep_SetNameplate();

	if (HasAuthority()) {
		FlowerArea->OnComponentBeginOverlap.AddDynamic(this, &AGrave::OnFlowerAreaBeginOverlap);
	}
}

void AGrave::OnFlowerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult) {
	if (!bActiveGrave) {
		return;
	}
	
	if (OtherActor->IsA<AFlower>()) {
		bActiveGrave = false;
		OnRep_ActiveGrave();

		GravePlayerName = "";
		OnRep_SetNameplate();

		const AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor);
		if (InteractableObject->ConsumeVfx) {
			Multicast_PopVfx(InteractableObject->ConsumeVfx);
		}
		
		OtherActor->Destroy(true);
	}
}

void AGrave::Multicast_PopVfx_Implementation(TSubclassOf<AActor> VfxClass) {
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* VfxActor = GetWorld()->SpawnActor<AActor>(VfxClass, this->GetActorLocation(), this->GetActorRotation(), SpawnParameters);
	VfxActor->SetLifeSpan(5.0f);
}

void AGrave::OnRep_SetNameplate_Implementation() {
	Nameplate->SetText(FText::FromName(GravePlayerName));
}

void AGrave::OnRep_ActiveGrave_Implementation() {
	OnGraveActiveChanged.Broadcast(bActiveGrave);
}

void AGrave::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrave, GravePlayerName);
	DOREPLIFETIME(AGrave, bActiveGrave);
}

int32 AGrave::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

FText AGrave::GetInformationText_Implementation() const {
	if (bActiveGrave) {
		return FText::FromString(FString::Printf(TEXT("A grave\n<Sub>%s rests here...</>"), *GravePlayerName.ToString()));
	}
	return FText::FromString("An empty grave");
}

void AGrave::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetStringField("GravePlayerName", GravePlayerName.ToString());
	JsonObject->SetBoolField("bActiveGrave", bActiveGrave);
}

void AGrave::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bActiveGrave = JsonObject->GetBoolField("bActiveGrave");
	OnRep_ActiveGrave();
	
	GravePlayerName = FName(*JsonObject->GetStringField("GravePlayerName"));
	OnRep_SetNameplate();
}

void AGrave::PerformCycle() {
	if (bActiveGrave) {
		GAME_STATE

		GameState->CycleDeathCount++;
	}
}