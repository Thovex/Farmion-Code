
#include "DebugEditorTickingComponent.h"

UDebugEditorTickingComponent::UDebugEditorTickingComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	bTickInEditor = true;
	bIsEditorOnly = true;

}

void UDebugEditorTickingComponent::BeginPlay() {
	Super::BeginPlay();

	SetComponentTickEnabled( false );
}


void UDebugEditorTickingComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

}


