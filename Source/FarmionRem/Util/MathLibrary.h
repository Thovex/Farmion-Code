
#pragma once

#include "CoreMinimal.h"
#include "MathLibrary.generated.h"

UCLASS(Blueprintable)
class UMathLibrary : public UBlueprintFunctionLibrary {
public:
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static int32 RoundOffset(const int32 Value, const int32 Offset);

	UFUNCTION( BlueprintCallable, Category = "Math", meta = ( WorldContext = "WorldContextObject" ) )
	static bool VectorSmallerCheck( FVector V1, FVector V2 );

	UFUNCTION( BlueprintCallable, Category = "Math", meta = ( WorldContext = "WorldContextObject" ) )
	static bool FloatSmallerCheck( float F1, float F2 );

	UFUNCTION( BlueprintCallable, Category = "Math", meta = ( WorldContext = "WorldContextObject" ) )
	static float VectorToVectorPercentageCheck( FVector V1, FVector V2, FVector OriginScale );

	UFUNCTION( BlueprintCallable, Category = "Math", meta = ( WorldContext = "WorldContextObject" ) )
	static float SetScale( float CurrentValue, float OldMinScale, float OldMaxScale, float NewMinScale, float NewMaxScale );

	UFUNCTION( BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe), Category = "Math", meta = ( WorldContext = "WorldContextObject" ) )
	static float SetScalePure( float CurrentValue, float OldMinScale, float OldMaxScale, float NewMinScale, float NewMaxScale );
	
	UFUNCTION(BlueprintPure, Category = "UI Math")
	static bool ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool& bTargetBehindCamera, bool bPlayerViewportRelative = false);

	UFUNCTION(BlueprintPure, Category = "UI Math")
	static bool IsWithinViewport(const FVector2D& ScreenPosition, const FVector2D& ViewportSize, float Tolerance = 0.0f);
};
