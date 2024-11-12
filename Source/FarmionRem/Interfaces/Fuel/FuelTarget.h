// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "FuelTarget.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFuelTarget : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API IFuelTarget {
	GENERATED_BODY()

public:
	virtual bool Supply(int32 FuelValue);
	virtual bool Light();
};