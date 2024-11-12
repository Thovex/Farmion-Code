// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Liquids.h"
#include "UObject/Interface.h"
#include "LiquidSource.generated.h"

UINTERFACE()
class ULiquidSource : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ILiquidSource {
	GENERATED_BODY()

public:
	virtual void UseSource();
	virtual bool IsSourceAvailable();
	
	virtual TEnumAsByte<ELiquids> GetLiquidType();
	virtual TEnumAsByte<EProductColor> GetLiquidColor();
};