// Copyright 2020-2023: Jesse J. van Vliet


#include "MathLibrary.h"

#include "Kismet/KismetMathLibrary.h"

int32 UMathLibrary::RoundOffset(const int32 Value, const int32 Offset) {
	return (UKismetMathLibrary::Round((double)Value / (double)Offset)) * Offset;
}

bool UMathLibrary::VectorSmallerCheck( FVector V1, FVector V2 ) {
	if ( FloatSmallerCheck( V1.X, V2.X ) && FloatSmallerCheck( V1.Y, V2.Y ) && FloatSmallerCheck( V1.Z, V2.Z ) ) return true;
	return false;
}

bool UMathLibrary::FloatSmallerCheck( float F1, float F2 ) {
	if ( F1 < F2 ) return true;
	return false;
}

float UMathLibrary::SetScale( float CurrentValue, float OldMinScale, float OldMaxScale, float NewMinScale, float NewMaxScale ) {

	return ( CurrentValue - OldMinScale ) * ( NewMaxScale - NewMinScale ) / ( OldMaxScale - OldMinScale ) + NewMinScale;
}

float UMathLibrary::VectorToVectorPercentageCheck( FVector V1, FVector V2, FVector OriginScale ) {
	const float X1 = V1.X;
	const float Y1 = V1.Y;
	const float Z1 = V1.Z;

	const float X2 = V2.X;
	const float Y2 = V2.Y;
	const float Z2 = V2.Z;

	const float XPercentage = SetScale( X1, OriginScale.X, X2, 0, 100 );
	const float YPercentage = SetScale( Y1, OriginScale.Y, Y2, 0, 100 );
	const float ZPercentage = SetScale( Z1, OriginScale.Z, Z2, 0, 100 );

	return ( XPercentage + YPercentage + ZPercentage ) / 3;
}

float UMathLibrary::SetScalePure( float CurrentValue, float OldMinScale, float OldMaxScale, float NewMinScale, float NewMaxScale ) {

	return ( CurrentValue - OldMinScale ) * ( NewMaxScale - NewMinScale )  / ( OldMaxScale - OldMinScale )  + NewMinScale;
}

bool UMathLibrary::ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool& bTargetBehindCamera, bool bPlayerViewportRelative)
{
	FVector Projected;
	bool bSuccess = false;

	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, ProjectionData))
		{
			const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
			const FIntRect ViewRectangle = ProjectionData.GetConstrainedViewRect();

			FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
			if (Result.W < 0.f) { bTargetBehindCamera = true; }
			if (Result.W == 0.f) { Result.W = 1.f; } // Prevent Divide By Zero

			const float RHW = 1.f / FMath::Abs(Result.W);
			Projected = FVector(Result.X, Result.Y, Result.Z) * RHW;

			// Normalize to 0..1 UI Space
			const float NormX = (Projected.X / 2.f) + 0.5f;
			const float NormY = 1.f - (Projected.Y / 2.f) - 0.5f;

			Projected.X = (float)ViewRectangle.Min.X + (NormX * (float)ViewRectangle.Width());
			Projected.Y = (float)ViewRectangle.Min.Y + (NormY * (float)ViewRectangle.Height());

			bSuccess = true;
			ScreenPosition = FVector2D(Projected.X, Projected.Y);

			if (bPlayerViewportRelative)
			{
				ScreenPosition -= FVector2D(ProjectionData.GetConstrainedViewRect().Min);
			}
		}
		else
		{
			ScreenPosition = FVector2D(1234, 5678);
		}
	}

	return bSuccess;
}

bool UMathLibrary::IsWithinViewport(const FVector2D& ScreenPosition, const FVector2D& ViewportSize, float Tolerance) {
	return ScreenPosition.X >= -Tolerance && ScreenPosition.Y >= -Tolerance && ScreenPosition.X <= ViewportSize.X + Tolerance && ScreenPosition.Y <= ViewportSize.Y + Tolerance;
}