// Copyright 2020-2023: Jesse J. van Vliet


#include "Petto.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

void APetto::OnRep_NewOwner() {
	if (OwningCharacter) {
		OwningCharacter->PettoBoom->SetRelativeLocation(FVector::ZeroVector);
		OwningCharacter->PettoBoom->SetRelativeRotation(CarryRotation);

		// Transformation
		OwningCharacter->PettoBoom->TargetArmLength = CarryLength;
		OwningCharacter->PettoBoom->SocketOffset = CarryOffset;

		// Collision
		OwningCharacter->PettoBoom->bDoCollisionTest = false;

		// Lag
		OwningCharacter->PettoBoom->bEnableCameraLag = true;
		OwningCharacter->PettoBoom->CameraLagSpeed = 5.f;
	
		OwningCharacter->PettoBoom->bEnableCameraRotationLag = true;
		OwningCharacter->PettoBoom->CameraRotationLagSpeed = 5.f;

		this->AttachToComponent(OwningCharacter->PettoBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		this->SetActorEnableCollision(false);
	} else {
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		this->SetActorTransform(SpawnTransform);
		this->SetActorEnableCollision(true);
	}
}

void APetto::BeginPlay() {
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();
}

int32 APetto::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return 0;
}

int32 APetto::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	return static_cast<int32>(EInteractCallback::START_USING_OBJECT);
}

void APetto::Server_Use(AFarmionCharacter* User, bool bUse) {
	if (OwningCharacter) {
		return;
	}

	// Detach existing Petto
	if (User->Petto) {
		User->Petto->OwningCharacter = nullptr;
		User->Petto->OnRep_NewOwner();
	}
	
	User->Petto = this;
	User->OnRep_Petto();

	OwningCharacter = User;
	OnRep_NewOwner();
}

bool APetto::CanCompost_Implementation() const {
	return false;
}

void APetto::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APetto, OwningCharacter);
}

