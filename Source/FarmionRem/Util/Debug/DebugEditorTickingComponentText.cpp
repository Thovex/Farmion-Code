
#include "DebugEditorTickingComponentText.h"
#include "DrawDebugText.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

void UDebugEditorTickingComponentText::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	DrawDebug();
}

void UDebugEditorTickingComponentText::DrawDebug() {
	if ( GetWorld() ) {
		DrawDebugText( GetWorld(), DisplayText, GetOwner()->GetActorLocation() + DisplayOffset,
					   DebugTextColor, false, .1F, 1, DebugTextThickness, DebugTextSize, bRotateDebugTextToCamera,
					   bTextHasMaxDistance, DebugMaximumTextDistance, bTextCapitalize
		);
	}
}
