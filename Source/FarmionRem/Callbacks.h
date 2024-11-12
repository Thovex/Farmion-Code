// Something Something

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

enum class EInteractCallback : int32 {
	NO_CALLBACK = 0,
	CARRYABLE_CAN_BE_CARRIED = 1 << 0,
	CARRYABLE_CAN_BE_DROPPED = 1 << 1,
	INTERACTABLE_CAN_BE_SOLD = 1 << 2,
	INTERACTABLE_CAN_BE_BOUGHT = 1 << 3,
	CONSUME_OBJECT = 1 << 4,
	START_USING_OBJECT = 1 << 5,
	STOP_USING_OBJECT = 1 << 6,
};


