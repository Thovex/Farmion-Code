// Copyright 2020-2023: Jesse J. van Vliet

#include "InteractableObject.h"

#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"

AInteractableObject::AInteractableObject() : AActor() {
	PrimaryActorTick.bCanEverTick = false;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	bReplicates = true;
	//SetReplicatingMovement(true);
}

void AInteractableObject::Multicast_OneTimeMove_Implementation(const TArray<FTransform>& Path) {
	this->SetActorEnableCollision(false);

	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	this->CarryTransforms.Empty();
	this->CarryTransforms = Path;
	this->bCarry = false;
	this->bNetworkKill = true;
	this->StartCarryTick();
}

void AInteractableObject::BeginPlay() {
	Super::BeginPlay();

	GAME_STATE
	
	if (!HasQuality()) {
		Quality = DetermineQuality(GameState->CalculateFarmLevel());
	}

	if (SpawnVfx) {
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AActor* VfxActor = GetWorld()->SpawnActor<AActor>(SpawnVfx, this->GetActorLocation(), this->GetActorRotation(), SpawnParameters);
		VfxActor->SetLifeSpan(5.0f);
	}

	// Get all StaticMeshComponents attached to the Actor
	//TArray<UStaticMeshComponent*> StaticMeshComponents;
	//this->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	// Iterate through all StaticMeshComponents and toggle their replication state
	//for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents) {
		// TODO If something is fucky regarding static mesh comps, we can turn this bkac on perhaps? but dunno why its here

		
		//UE_LOG(LogTemp, Warning, TEXT("Toggled Replicates to: True on %s"), *StaticMeshComponent->GetFullName());
		//StaticMeshComponent->SetIsReplicated(true);
	//}
}

void AInteractableObject::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	ISaveLoad::Save(JsonObject);

	JsonObject->SetNumberField("Quality", static_cast<int32>(Quality));

	// Is Being Held... DoNotRecreate
	if (CarryableSocket) {
		JsonObject->SetBoolField("AutoRecreate", false);
	}
}

void AInteractableObject::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	ISaveLoad::Load(JsonObject);

	Quality = static_cast<EItemQuality>(JsonObject->GetNumberField("Quality"));

}

void AInteractableObject::Destroyed() { 
	UE_LOG(LogTemp, Log, TEXT("Destroying: %s"), *GetName());
	Super::Destroyed();
}

FText AInteractableObject::GetInformationText_Implementation() const {
	return FText();
}

void AInteractableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableObject, Quality);
	DOREPLIFETIME(AInteractableObject, bNetworkKill);
}

EItemQuality AInteractableObject::GetQuality() {
	return Quality;
}

bool AInteractableObject::HasQuality() const {
	return Quality != EItemQuality::None;
}


void AInteractableObject::Server_Use_Implementation(AFarmionCharacter* User, bool bUse) {
	
}

void AInteractableObject::Multicast_Use_Implementation(bool bUse) {
	
}

void AInteractableObject::Multicast_Consume_Implementation() {
}

void AInteractableObject::Server_Consume_Implementation() {
	bConsumed = true;
}