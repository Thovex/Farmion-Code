
#pragma once

#include "DebugEditorTickingComponent.h"
#include "DebugEditorLineCompToMultiActors.generated.h"

/**
 *	
 */
UCLASS( ClassGroup = ( Debug ), meta = ( BlueprintSpawnableComponent ), BlueprintType, Blueprintable )
class UDebugEditorLineCompToMultiActors : public UDebugEditorTickingComponent {
	GENERATED_BODY()

public:

	UDebugEditorLineCompToMultiActors();

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Debug Line" )
		FName StartComponentName;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Debug Line" )
		TArray<AActor*> EndActors;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Debug Line" )
		FColor DebugLineColor = FColor::Red;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Debug Line" )
		float DebugLineThickness = 5.0F;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	#if WITH_EDITOR
	virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
	#endif

private:
	void FetchComponents();

	int8 Retries = 0;

	USceneComponent* StartComponent;


};
