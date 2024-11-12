// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Liquids.generated.h"

UENUM(BlueprintType)
enum EProductColor {
	EProductColor_White = 0 UMETA(DisplayName = "White"),
	// DEFAULT

	EProductColor_Red = 1 UMETA(DisplayName = "Red"),
	EProductColor_Orange = 2 UMETA(DisplayName = "Orange"),
	EProductColor_Yellow = 3 UMETA(DisplayName = "Yellow"),
	EProductColor_Green = 4 UMETA(DisplayName = "Green"),
	EProductColor_Blue = 5 UMETA(DisplayName = "Blue"),
	EProductColor_Purple = 6 UMETA(DisplayName = "Purple"),
	EProductColor_Brown = 7 UMETA(DisplayName = "Brown"),
	EProductColor_Gray = 8 UMETA(DisplayName = "Gray"),
	EProductColor_Black = 9 UMETA(DisplayName = "Black"),
};

static FORCEINLINE FString GetProductColorDisplayName(EProductColor Color) {
	switch (Color) {
		case EProductColor_White:
			return "White";
		case EProductColor_Red:
			return "Red";
		case EProductColor_Orange:
			return "Orange";
		case EProductColor_Yellow:
			return "Yellow";
		case EProductColor_Green:
			return "Green";
		case EProductColor_Blue:
			return "Blue";
		case EProductColor_Purple:
			return "Purple";
		case EProductColor_Brown:
			return "Brown";
		case EProductColor_Gray:
			return "Gray";
		case EProductColor_Black:
			return "Black";
		default:
			return "Unknown";
	}
}

UENUM(BlueprintType)
enum ELiquids {
	ELiquids_None = 0 UMETA(DisplayName = "None"),
	ELiquids_Water = 1 UMETA(DisplayName = "Water"),
	ELiquids_Milk = 2 UMETA(DisplayName = "Milk"),
	ELiquids_Honey = 3 UMETA(DisplayName = "Honey"),
	ELiquids_Beer = 4 UMETA(DisplayName = "Beer"),
	ELiquids_Wine = 5 UMETA(DisplayName = "Wine"),
	ELiquids_Compost = 6 UMETA(DisplayName = "Liquid Compost"),
	ELiquids_GrapeJuice	= 7 UMETA(DisplayName= "Grape Juice"),
};

static FORCEINLINE FString GetLiquidDisplayName(ELiquids Liquid) {
	switch (Liquid) {
		case ELiquids_None:
			return "None";
		case ELiquids_Water:
			return "Water";
		case ELiquids_Milk:
			return "Milk";
		case ELiquids_Honey:
			return "Honey";
		case ELiquids_Beer:
			return "Beer";
		case ELiquids_Wine:
			return "Wine";
		case ELiquids_Compost:
			return "Liquid Compost";
		case ELiquids_GrapeJuice:
			return "Grape Juice";
		default:
			return "Unknown";
	}
}