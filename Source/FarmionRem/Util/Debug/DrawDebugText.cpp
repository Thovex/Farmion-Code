
#include "DrawDebugText.h"
#include "DrawDebugHelpers.h"
#include "EngineGlobals.h"
#include "Components/LineBatchComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

#if ENABLE_DRAW_DEBUG

void DrawTextDebugLine( const UWorld* InWorld, FVector const& LineStart, FVector const& LineEnd, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness ) {
	// no debug line drawing on dedicated server
	if ( GEngine->GetNetMode( InWorld ) != NM_DedicatedServer ) {
		// this means foreground lines can't be persistent 

		ULineBatchComponent* const LineBatcher = ( InWorld ? ( SDPG_Foreground ? InWorld->ForegroundLineBatcher : ( ( bPersistentLines || ( LifeTime > 0.f ) ) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher ) ) : NULL );

		if ( LineBatcher != NULL ) {
			float const LineLifeTime = ( LifeTime > 0.f ) ? LifeTime : LineBatcher->DefaultLifeTime;
			LineBatcher->DrawLine( LineStart, LineEnd, Color, DepthPriority, Thickness, LineLifeTime );
		}
	}
}

void DrawDebugSegmentById( const UWorld * InWorld, uint32 SegmentId, FVector const & Location, FColor const & Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness, float Size, FRotator Rotation ) {
	FRotator SegmentRotation = Rotation;

	FRotationMatrix SegmentRotationMatrix( SegmentRotation );

	const FVector TopLeft = Location + SegmentRotationMatrix.TransformVector( FVector( -15, 0, 25 ) * Size );
	const FVector TopCenter = Location + SegmentRotationMatrix.TransformVector( FVector( 0, 0, 25 ) * Size );
	const FVector TopRight = Location + SegmentRotationMatrix.TransformVector( FVector( 15, 0, 25 ) * Size );

	const FVector MidLeft = Location + SegmentRotationMatrix.TransformVector( FVector( -15, 0, 0 ) * Size );
	const FVector MidCenter = Location + SegmentRotationMatrix.TransformVector( FVector( 0, 0, 0 ) * Size );
	const FVector MidRight = Location + SegmentRotationMatrix.TransformVector( FVector( 15, 0, 0 ) * Size );

	const FVector BotLeft = Location + SegmentRotationMatrix.TransformVector( FVector( -15, 0, -25 ) * Size );
	const FVector BotCenter = Location + SegmentRotationMatrix.TransformVector( FVector( 0, 0, -25 ) * Size );
	const FVector BotRight = Location + SegmentRotationMatrix.TransformVector( FVector( 15, 0, -25 ) * Size );

	switch ( SegmentId ) {
		case 1:
		{
			DrawTextDebugLine( InWorld, TopLeft, TopCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 2:
		{
			DrawTextDebugLine( InWorld, TopCenter, TopRight, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 4:
		{
			DrawTextDebugLine( InWorld, TopRight, MidRight, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 8:
		{
			DrawTextDebugLine( InWorld, MidRight, BotRight, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 16:
		{
			DrawTextDebugLine( InWorld, BotRight, BotCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 32:
		{
			DrawTextDebugLine( InWorld, BotLeft, BotCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 64:
		{
			DrawTextDebugLine( InWorld, BotLeft, MidLeft, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 128:
		{
			DrawTextDebugLine( InWorld, MidLeft, TopLeft, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 256:
		{
			DrawTextDebugLine( InWorld, TopLeft, MidCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 512:
		{
			DrawTextDebugLine( InWorld, MidCenter, TopCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 1024:
		{
			DrawTextDebugLine( InWorld, TopRight, MidCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );

		} break;
		case 2048:
		{
			DrawTextDebugLine( InWorld, MidCenter, MidRight, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 4096:
		{
			DrawTextDebugLine( InWorld, BotRight, MidCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 8192:
		{
			DrawTextDebugLine( InWorld, MidCenter, BotCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 16384:
		{
			DrawTextDebugLine( InWorld, BotLeft, MidCenter, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
		case 32768:
		{
			DrawTextDebugLine( InWorld, MidCenter, MidLeft, Color, bPersistentLines, LifeTime, DepthPriority, Thickness );
		} break;
	}
}

void DrawDebugCharacter( const UWorld * InWorld, char Character, FVector const & Location, FColor const & Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness, float Size, FRotator Rotation ) {
	uint32 Shifter = 0x01;

	for ( uint32 i = 0; i < 16; i++ ) {
		if ( UKismetMathLibrary::And_IntInt( SixteenSegmentASCII[Character - 32], Shifter ) == Shifter ) {
			DrawDebugSegmentById( InWorld, Shifter, Location, Color, bPersistentLines, LifeTime, DepthPriority, Thickness, Size, Rotation );
		}
		Shifter = Shifter << 1;
	}
}

void DrawDebugText( const UWorld * InWorld, FString Text, FVector const & Location, FColor const & Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness, float Size, bool bRotateToCamera, bool bHasMaxDistance, int32 MaxDistance, bool bCapitalize ) {
	FRotator CameraRotation = FRotator::ZeroRotator;

	auto EditorCameraViewLocations = InWorld->ViewLocationsRenderedLastFrame;
	if ( EditorCameraViewLocations.Num() == 0 ) {
		return;
	}

	if ( bHasMaxDistance ) {
		FVector EditorCameraToLocationVector = EditorCameraViewLocations[0] - Location;
		float EditorCameraDistance = EditorCameraToLocationVector.Size();

		if ( EditorCameraDistance > MaxDistance ) {
			return;
		}
	}

	if ( bRotateToCamera ) {
		FVector EditorCameraLocation = EditorCameraViewLocations[0];
		CameraRotation = UKismetMathLibrary::FindLookAtRotation( Location, EditorCameraLocation );
		CameraRotation = FRotator( 0, CameraRotation.Yaw - 90, 0 );
	}


	if ( bCapitalize ) {
		Text = Text.ToUpper();
	}

	for ( int32 i = 0; i < Text.Len(); i++ ) {
		FRotator SegmentRotation = CameraRotation;
		FRotationMatrix SegmentRotationMatrix( SegmentRotation );

		float CenterOffset = ( float ) -Text.Len() / 2 * 40 * Size;

		FVector LocationOffset = Location + SegmentRotationMatrix.TransformVector( FVector( CenterOffset, 0, 0 ) + FVector( 40 * Size, 0, 0 ) * i );
		DrawDebugCharacter( InWorld, Text[i], LocationOffset, Color, bPersistentLines, LifeTime, DepthPriority, Thickness, Size, CameraRotation );
	}
}


#endif // ENABLE_DRAW_DEBUG
