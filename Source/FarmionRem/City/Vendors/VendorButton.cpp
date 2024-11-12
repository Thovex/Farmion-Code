// Copyright 2020-2023: Jesse J. van Vliet

#include "VendorButton.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "Kismet/GameplayStatics.h"

AVendorButton::AVendorButton() {
	PrimaryActorTick.bCanEverTick = false;

	FallbackSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("FallbackSpawnLocation"));
	FallbackSpawnLocation->SetupAttachment(RootComponent);
	
	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));

	bAlwaysRelevant = true;
}

int32 AVendorButton::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return (int32)EInteractCallback::INTERACTABLE_CAN_BE_BOUGHT;
}

void AVendorButton::Buy() {

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(FallbackSpawnLocation->GetComponentLocation());
	SpawnTransform.SetRotation(FallbackSpawnLocation->GetComponentRotation().Quaternion());
	
	AInteractableObject* SpawnedObject = GetWorld()->SpawnActorDeferred<AInteractableObject>(InteractableToSpawn, SpawnTransform);

	if (SpawnStorage) {
		FCarryableSocketInfo Info;
		int32 Index;
		if (SpawnStorage->GetMatchingSocket(Info, Index, SpawnedObject)) {
			SpawnStorage->SetSocketAvailability(SpawnedObject, Index, false);
			FVector SocketOrigin, SocketBoundsExtent;
			Info.GetSocketBounds(SocketOrigin, SocketBoundsExtent);

			UGameplayStatics::FinishSpawningActor(SpawnedObject, SpawnTransform);
			SpawnedObject->AttachToActor(SpawnStorage.Get(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SpawnedObject->SetActorLocation(Info.Component->GetComponentLocation() - FVector(0, 0, SocketBoundsExtent.Z));
			SpawnedObject->SetActorRotation(Info.Component->GetComponentRotation());
			
			const TScriptInterface<ICarryable> CarryableInt = TScriptInterface<ICarryable>(SpawnedObject);
			const TScriptInterface<ICarryableSocket> CarryableSocketInt = TScriptInterface<ICarryableSocket>(SpawnStorage.Get());
			CarryableInt->CarryableSocket = CarryableSocketInt;
			CarryableInt->CarryableSocketIndex = Index;
		
			UE_LOG(LogTemp, Log, TEXT("Spawned %s into Storage %s"), *SpawnedObject->GetName(), *SpawnStorage->GetName());
		}
	} else {
		UGameplayStatics::FinishSpawningActor(SpawnedObject, SpawnTransform);
		UE_LOG(LogTemp, Log, TEXT("Spawned %s into Fallback"), *SpawnedObject->GetName());
	}

	Multicast_Purchased();
}

int32 AVendorButton::GetBuyPrice_Implementation() const {
	return BuyPrice;
}

FText AVendorButton::GetInformationText_Implementation() const {
	return FText::FromString(FString::Printf(TEXT("Upgrade %s?\n<Sub>$%d</>"), *Text.ToString(), IBuyable::Execute_GetBuyPrice((UObject*)this)));
}

void AVendorButton::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	const FVector& Loc = FallbackSpawnLocation->GetComponentLocation();
	JsonObject->SetNumberField("FallbackSpawnX", Loc.X);
	JsonObject->SetNumberField("FallbackSpawnY", Loc.Y);
	JsonObject->SetNumberField("FallbackSpawnZ", Loc.Z);
}

void AVendorButton::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	FallbackSpawnLocation->SetWorldLocation(FVector(
		JsonObject->GetNumberField("FallbackSpawnX"),
		JsonObject->GetNumberField("FallbackSpawnY"),
		JsonObject->GetNumberField("FallbackSpawnZ")
	));
}

void AVendorButton::Multicast_Purchased_Implementation() {
	
}

void AVendorButton::Link() {
	SpawnStorage = Linker->GetLinkedActor(TEXT("Storage"));
}

