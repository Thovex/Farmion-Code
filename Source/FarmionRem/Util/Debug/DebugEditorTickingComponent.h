
#pragma once

#include "Components/ActorComponent.h"
#include "DebugEditorTickingComponent.generated.h"


UCLASS( ClassGroup = ( Debug ), meta = ( BlueprintSpawnableComponent ), BlueprintType, Blueprintable )
class UDebugEditorTickingComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UDebugEditorTickingComponent();


protected:
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

};
