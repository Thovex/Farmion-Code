// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "Interactable.generated.h"

class AFarmionCharacter;

UINTERFACE()
class UInteractable : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API IInteractable {
	GENERATED_BODY()

public:
	virtual int32 GetPriority() const;
	
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit);
	virtual int32 StopInteract(AFarmionCharacter* InteractInstigator);
};
