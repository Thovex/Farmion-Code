#pragma once

#include "CoreMinimal.h"
#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "Watermill.generated.h"

class UBoxComponent;
class ULinkerComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AWatermill : public AUpgrade
{
	GENERATED_BODY()

public:
	AWatermill();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Rotating)
	bool bRotating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SecondsToRotate = -1.f; // -1 is Infinite
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Rotating();

protected:
	virtual void BeginPlay() override;
	
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void StopRotation();

	FTimerHandle RotateTimer;
	
};