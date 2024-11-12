// Copyright 2020-2023: Jesse J. van Vliet

#include "Interactable.h"

#include "Carryable.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Util/CommonLibrary.h"

int32 IInteractable::GetPriority() const {
	return 0;
}

int32 IInteractable::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	UE_LOG(LogTemp, Warning, TEXT("Interacting with %s"), *this->_getUObject()->GetName());
	
	int32 Callback = (int32)EInteractCallback::NO_CALLBACK;

	ICarryable* CarryableObject = Cast<ICarryable>(this);
	if (CarryableObject) {
		UCommonLibrary::SetFlag(Callback, CarryableObject->MeetsCarryRequirements(InteractInstigator));
	}

	return Callback;
}

int32 IInteractable::StopInteract(AFarmionCharacter* InteractInstigator) {
	UE_LOG(LogTemp, Warning, TEXT("Stop Interact with %s"), *this->_getUObject()->GetName());
	return (int32)EInteractCallback::NO_CALLBACK;
}
