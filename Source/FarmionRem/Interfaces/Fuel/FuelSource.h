// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "FuelSource.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFuelSource : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API IFuelSource {
	GENERATED_BODY()

public:
	virtual int32 GetFuelValue();
};