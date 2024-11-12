// Copyright 2020-2023: Jesse J. van Vliet

#include "LiquidSource.h"

void ILiquidSource::UseSource() {
	// All good.
}

bool ILiquidSource::IsSourceAvailable() {
	return true;
}

TEnumAsByte<ELiquids> ILiquidSource::GetLiquidType() {
	UE_LOG(LogTemp, Error, TEXT("ILiquidSource::GetLiquidType() - Unimplemented"));
	return ELiquids::ELiquids_None;
}

TEnumAsByte<EProductColor> ILiquidSource::GetLiquidColor() {
	UE_LOG(LogTemp, Error, TEXT("ILiquidSource::GetLiquidColor() - Unimplemented"));
	return EProductColor::EProductColor_White;
}