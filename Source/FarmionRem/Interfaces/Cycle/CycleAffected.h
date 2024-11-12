// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "CycleAffected.generated.h"

enum class EDecayFlags : uint8;

// This class does not need to be modified.
UINTERFACE()
class UCycleAffected : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ICycleAffected {
	GENERATED_BODY()

public:
	virtual EDecayFlags GetDecayFlag();
	virtual void PerformCycle();
	virtual void PostPerformCycle();

	virtual void TrySetProtect(EDecayFlags InFlags, bool Enabled);

	bool IsProtected() const;
};