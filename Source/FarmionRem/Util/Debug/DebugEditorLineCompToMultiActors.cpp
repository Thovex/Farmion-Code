
#include "DebugEditorLineCompToMultiActors.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"


UDebugEditorLineCompToMultiActors::UDebugEditorLineCompToMultiActors() {
	FetchComponents();
}

void UDebugEditorLineCompToMultiActors::BeginPlay() {
	Super::BeginPlay();
}

void UDebugEditorLineCompToMultiActors::FetchComponents() {
	if ( GetOwner() ) {
		for ( UActorComponent* Component : GetOwner()->GetComponents() ) {
			if ( Component ) {
				USceneComponent* ComponentToScene = Cast<USceneComponent>( Component );

				if ( ComponentToScene ) {
					if ( ComponentToScene->GetFName() == StartComponentName ) {
						StartComponent = ComponentToScene;
					}
				}
			}
		}
	}
}

#if WITH_EDITOR
void UDebugEditorLineCompToMultiActors::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) {
	Super::PostEditChangeProperty( PropertyChangedEvent );

	FetchComponents();
}
#endif

void UDebugEditorLineCompToMultiActors::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	const UWorld* World = GetWorld();

	if ( World ) {
		if ( StartComponent && EndActors.Num() > 0 ) {
			for ( AActor* EndActor : EndActors ) {
				if ( EndActor ) {
					DrawDebugLine( World, StartComponent->GetComponentLocation(), EndActor->GetActorLocation(), DebugLineColor, false, .1F, 0, DebugLineThickness );
				}
			}
		} else {
			if ( Retries < 50 ) {
				FetchComponents();
				Retries++;
			}
		}
	}
}


