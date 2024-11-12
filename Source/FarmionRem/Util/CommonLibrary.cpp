// Copyright 2020-2023: Jesse J. van Vliet

#include "CommonLibrary.h"

#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

int32 UCommonLibrary::GetNetId(const UWorld* WorldContext) {
	const AController* Controller = UGameplayStatics::GetPlayerController(WorldContext, 0);
	if (!Controller) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Controller in GetNetID"));
		return -1;
	}

	const APlayerState* State = Controller->GetPlayerState<APlayerState>();
	if (!State) {
		UE_LOG(LogTemp, Error, TEXT("Invalid State in GetNetID"));
		return -1;
	}

	return State->GetPlayerId();
}

TArray<FName> UCommonLibrary::SortNameArray(const TArray<FName>& InArray) {
	TArray<FName> OutArray = InArray;
	OutArray.Sort([](const FName& A, const FName& B) { return A.FastLess(B); });
	return OutArray;
}

void UCommonLibrary::SetCollision(UPrimitiveComponent* PrimitiveComponent, ECollisionEnabled::Type NewCollision, bool bPropagateToChildren) {
	if (PrimitiveComponent == nullptr) {
		return;
	}

	PrimitiveComponent->SetCollisionEnabled(NewCollision);

	if (bPropagateToChildren) {
		const int32 ChildCount = PrimitiveComponent->GetNumChildrenComponents();
		for (int32 ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex) {
			UPrimitiveComponent* ChildComponent = Cast<UPrimitiveComponent>(PrimitiveComponent->GetChildComponent(ChildIndex));
			if (ChildComponent) {
				SetCollision(ChildComponent, NewCollision, bPropagateToChildren);
			}
		}
	}
}

void UCommonLibrary::SetKeyboardFocusNull() {
	FSlateApplication::Get().ResetToDefaultInputSettings();
}

TArray<FKey> UCommonLibrary::GetKeysForAction(const UObject* WorldContextObject, const FName& ActionName) {
	TArray<FKey> Result;

	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject->GetWorld(), 0)) {
		const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		for (FEnhancedActionKeyMapping& KeyMap : Subsystem->GetAllPlayerMappableActionKeyMappings()) {
			if (KeyMap.GetMappingName() == ActionName) {
				Result.Add(KeyMap.Key);
			}
		}
	}

	return Result;
}

void UCommonLibrary::RebindAction(const UObject* WorldContextObject, const FName& ActionName, const FKey& NewKey, const int32 SlotIndex) {
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject->GetWorld(), 0)) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

		FModifyContextOptions Options;
		Options.bForceImmediately = true;

		const FPlayerMappableKeySlot& Slot = FPlayerMappableKeySlot(SlotIndex);
		if (!NewKey.IsValid()) {
			Subsystem->RemovePlayerMappedKeyInSlot(ActionName, Slot, Options);
		} else {
			Subsystem->AddPlayerMappedKeyInSlot(ActionName, NewKey, Slot, Options);
		}
	}
}

void UCommonLibrary::CreateRectangleSpline(USplineComponent* Spline, UStaticMeshComponent* MeshComponent, float Offset) {
	// Clear all previous points in the spline.
	Spline->ClearSplinePoints(true);

	FVector Origin, BoxExtent;
	float SphereRadius;

	// Get mesh component bounds
	UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtent, SphereRadius);

	// Calculate the minimum and maximum points of the box
	FVector Min = BoxExtent;
	FVector Max = -BoxExtent;

	// Create a FBox instance
	FBox Bounds(Min, Max);

	// Define a small utility lambda function to add points to the spline.
	auto AddPointToSpline = [&Spline](const FVector& Position, const FVector& TangentIn, const FVector& TangentOut) {
		const int32 NewPointIndex = Spline->GetNumberOfSplinePoints();
		Spline->AddSplinePoint(Position, ESplineCoordinateSpace::Local, false);
		Spline->SetTangentsAtSplinePoint(NewPointIndex, TangentIn, TangentOut, ESplineCoordinateSpace::Local, false);
	};

	FVector MinBound = Bounds.Min;
	FVector MaxBound = Bounds.Max;

	// Generate corners of rectangle.
	FVector BottomLeft = FVector(MinBound.X - Offset, MinBound.Y - Offset, 0);
	FVector BottomRight = FVector(MaxBound.X + Offset, MinBound.Y - Offset, 0);
	FVector TopRight = FVector(MaxBound.X + Offset, MaxBound.Y + Offset, 0);
	FVector TopLeft = FVector(MinBound.X - Offset, MaxBound.Y + Offset, 0);

	// Add all points to the spline.
	AddPointToSpline(BottomLeft, FVector::ZeroVector, FVector(1, 0, 0));
	AddPointToSpline(BottomRight, FVector(1, 0, 0), FVector::ZeroVector);
	AddPointToSpline(TopRight, FVector::ZeroVector, FVector(-1, 0, 0));
	AddPointToSpline(TopLeft, FVector(-1, 0, 0), FVector::ZeroVector);

	// Set the spline to be a closed loop
	Spline->SetClosedLoop(true);

	// Don't forget to update the spline after adding all the points.
	Spline->UpdateSpline();
}

void UCommonLibrary::ParseDateTime(const FString& InputString, int32& Day, int32& Month, int32& Year, int32& Hour, int32& Minutes, int32& Seconds) {
	const FRegexPattern Pattern(TEXT("(\\d{4})\\.(\\d{2})\\.(\\d{2})-(\\d{2})\\.(\\d{2})\\.(\\d{2})"));
	FRegexMatcher Matcher(Pattern, InputString);

	if (Matcher.FindNext()) {
		Year = FCString::Atoi(*Matcher.GetCaptureGroup(1));
		Month = FCString::Atoi(*Matcher.GetCaptureGroup(2));
		Day = FCString::Atoi(*Matcher.GetCaptureGroup(3));
		Hour = FCString::Atoi(*Matcher.GetCaptureGroup(4));
		Minutes = FCString::Atoi(*Matcher.GetCaptureGroup(5));
		Seconds = FCString::Atoi(*Matcher.GetCaptureGroup(6));
	} else {
		// You can handle the error here if the string does not match the pattern
		Year = Month = Day = Hour = Minutes = Seconds = 0;
	}
}
