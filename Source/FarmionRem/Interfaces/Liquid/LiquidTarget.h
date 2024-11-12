// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Liquids.h"
#include "UObject/Interface.h"
#include "LiquidTarget.generated.h"

UINTERFACE()
class ULiquidTarget : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ILiquidTarget {
	GENERATED_BODY()

public:
	virtual bool Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges);
};