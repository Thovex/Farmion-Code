// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "StorageUnit.h"
#include "NoCollisionStorage.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ANoCollisionStorage : public AStorageUnit {
	GENERATED_BODY()

protected:

	// Turns off the collision of the socketed interactables.
	virtual bool TurnSocketedCollisionOff() override;
};