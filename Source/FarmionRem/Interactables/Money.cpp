// Copyright 2020-2023: Jesse J. van Vliet

#include "Money.h"

#include "Components/TextRenderComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/GlobalValues.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Internationalization/Text.h"
#include "Net/UnrealNetwork.h"

AMoney::AMoney() {
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	MoneyText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("MoneyText"));
	MoneyText->SetupAttachment(SpringArm);
}

void AMoney::OnRep_Money() {
	this->MoneyText->SetText(FText::FromString(FString::FromInt(this->Money)));
}

void AMoney::BeginPlay() {
	Super::BeginPlay();
	this->MoneyText->SetText(FText::FromString(FString::FromInt(this->Money)));
}

void AMoney::Server_Consume() {
	Super::Server_Consume();

	GAME_STATE

	GameState->Money += this->Money;
	GameState->OnRep_Money();

	SetLifeSpan(GCarry_Grab_Interpolation_Duration * 0.5f);
}

void AMoney::Multicast_Consume() {
	if (ConsumeVfx) {
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AActor* VfxActor = GetWorld()->SpawnActor<AActor>(ConsumeVfx, this->GetActorLocation(), this->GetActorRotation(), SpawnParameters);
		VfxActor->SetLifeSpan(5.0f);
	}

	UE_LOG(LogTemp, Log, TEXT("Multicast Consume!"));
}

int32 AMoney::GetPriority() const {
	return 10;
}

void AMoney::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	switch (EndPlayReason) {
		case EEndPlayReason::Destroyed:
		{
			Multicast_Consume();
		} break;
		default: {} break;
	}
}

int32 AMoney::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	int32 Callback = Super::Interact(InteractInstigator, InteractionHit);
	UCommonLibrary::SetFlag(Callback, (int32)EInteractCallback::CONSUME_OBJECT);
	return Callback;
}

void AMoney::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMoney, Money);
}
