
#include "DebugEditorWorldText.h"
#include "DrawDebugText.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

ADebugEditorWorldText::ADebugEditorWorldText() {
	Transform = CreateDefaultSubobject<USceneComponent>( FName( "Transform" ) );
	RootComponent = Transform;

	bIsEditorOnlyActor = true;

	PrimaryActorTick.bCanEverTick = true;
}

void ADebugEditorWorldText::BeginPlay() {
	Super::BeginPlay();

	SetActorTickEnabled( false );

}

void ADebugEditorWorldText::Tick( float DeltaTime ) {
	if ( bDebugMode ) {
		DrawDebug();
	}
}

void ADebugEditorWorldText::DrawDebug() {

	if ( GetWorld() ) {
		DrawDebugText( GetWorld(), DisplayText, GetActorLocation(),
					   DebugTextColor, false, .1F, 1, DebugTextThickness, DebugTextSize, bRotateDebugTextToCamera,
					   bTextHasMaxDistance, DebugMaximumTextDistance, bTextCapitalize 
		);
	}
}

bool ADebugEditorWorldText::ShouldTickIfViewportsOnly() const {
	return true;
}


