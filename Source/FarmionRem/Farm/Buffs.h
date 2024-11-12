// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Buffs.generated.h"

// Buffs:
// - Movement
// - Jump Height
// - Throw Strength
// - Slowdown Day
// - Extra Money Gain
// - Farm Level Temporarily Increased
// - XP Gain Increased
// - Luck

UENUM(BlueprintType)
enum class EBuffModifier : uint8 {
	None,
	Flat,
	Percentage,
};

UENUM(BlueprintType)
enum class EBuffType : uint8 {
	None,
	MovementSpeed, // Implemented
	JumpHeight,
	ThrowStrength,
	SlowdownDay, // Implemented
	ExtraMoneyGain, // Implemented
	FarmLevelIncreased, // Implemented
	XPGainIncreased,
	Luck,
	SnakeBiteResistance, // Implemented
};

USTRUCT(BlueprintType)
struct FBuff {
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 BuffId = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	UTexture2D* BuffIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	EBuffType BuffType = EBuffType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FName BuffName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	float BuffValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	EBuffModifier Modifier = EBuffModifier::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	float BuffDuration = 10.f;
};

