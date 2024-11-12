// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CoreMinimal.h"
#include "FarmionRem/LogCategories.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Object.h"
#include "CommonLibrary.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class FARMIONREM_API UCommonLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	static int32 GetNetId(const UWorld* WorldContext);

	UFUNCTION(BlueprintCallable)
	static TArray<FName> SortNameArray(const TArray<FName>& InArray);

	UFUNCTION(BlueprintCallable, Category = "Collision")
	static void SetCollision(UPrimitiveComponent* PrimitiveComponent, ECollisionEnabled::Type NewCollision, bool bPropagateToChildren);

	UFUNCTION(BlueprintCallable, meta = ( WorldContext = "WorldContextObject" ))
	static void SetKeyboardFocusNull();

	template <typename T>
	static void Shuffle(TArray<T>& Array);

	UFUNCTION(BlueprintCallable, Category = "Input", meta = ( WorldContext = "WorldContextObject" ))
	static TArray<FKey> GetKeysForAction(const UObject* WorldContextObject, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Input", meta = ( WorldContext = "WorldContextObject" ))
	static void RebindAction(const UObject* WorldContextObject, const FName& ActionName, const FKey& NewKey, const int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	static void CreateRectangleSpline(USplineComponent* Spline, UStaticMeshComponent* MeshComponent, float Offset);

	UFUNCTION(BlueprintCallable, Category = "Date and Time")
	static void ParseDateTime(const FString& InputString, int32& Day, int32& Month, int32& Year, int32& Hour, int32& Minutes, int32& Seconds);
	
	template <typename T>
	static constexpr bool HasFlag(const int32& Value, const int32& Flag) {
		return (Value & Flag) != 0;
	}

	template <typename T>
	static constexpr bool HasFlag(const int32& Value, const T& Flag) {
		return HasFlag<T>(Value, static_cast<int32>(Flag));
	}

	template <typename T>
	static constexpr bool HasFlag(const T& Value, const int32& Flag) {
		return HasFlag<T>(static_cast<int32>(Value), Flag);
	}

	template <typename T>
	static constexpr bool HasFlag(const T& Value, const T& Flag) {
		return HasFlag<T>(static_cast<int32>(Value), static_cast<int32>(Flag));
	}

	template <typename T>
	static constexpr bool HasAllFlags(const int32& Value, const int32& Flags) {
		return (Value & Flags) == Flags;
	}

	template <typename T>
	static constexpr bool HasAllFlags(const int32& Value, const T& Flags) {
		return HasAllFlags<T>(Value, static_cast<int32>(Flags));
	}

	template <typename T>
	static constexpr bool HasAllFlags(const T& Value, const int32& Flags) {
		return HasAllFlags<T>(static_cast<int32>(Value), Flags);
	}

	template <typename T>
	static constexpr bool HasAllFlags(const T& Value, const T& Flags) {
		return HasAllFlags<T>(static_cast<int32>(Value), static_cast<int32>(Flags));
	}

	template <typename T>
	static void SetFlag(T& value, const T& flag) {
		UE_LOG(LogInteractCallbackFlags, Log, TEXT("Set Flag Called: [%d], [%d]. RETURNING: [%d]"), static_cast<int32>(value), static_cast<int32>(flag), static_cast<int32>(value | flag));
		value |= flag;
	}

	template <typename T>
	static void ClearFlag(T& value, const T& flag) {
		UE_LOG(LogInteractCallbackFlags, Log, TEXT("Clear Flag Called: [%d], [%d]. RETURNING: [%d]"), static_cast<int32>(value), static_cast<int32>(flag), static_cast<int32>(value & ~flag));
		value &= ~flag;
	}

	template <typename T>
	static void ToggleFlag(T& value, const T& flag) {
		UE_LOG(LogInteractCallbackFlags, Log, TEXT("Toggle Flag Called: [%d], [%d]. RETURNING: [%d]"), static_cast<int32>(value), static_cast<int32>(flag), static_cast<int32>(value ^ flag));
		value ^= flag;
	}

	template <typename T>
	static constexpr bool HasAnyFlag(const T& value, const T& flags) {
		return (value & flags) != 0;
	}

	template <typename T>
	static constexpr bool HasAnyFlag(const T& value) {
		return value != 0;
	}

	template <typename T>
	static constexpr int CountFlags(const T& value) {
		int count = 0;
		for (int i = 0; i < sizeof(value) * 8; i++) {
			if ((value & (1 << i)) != 0) {
				count++;
			}
		}
		return count;
	}
};

template <typename T>
void UCommonLibrary::Shuffle(TArray<T>& Array) {
	if (Array.Num() > 0) {
		const int32 LastIndex = Array.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i) {
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index) {
				Array.Swap(i, Index);
			}
		}
	}
}