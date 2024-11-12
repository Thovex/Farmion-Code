// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Storage/StorageUnit.h"
#include "Mould.generated.h"

class UBoxComponent;
UCLASS(Blueprintable)
class FARMIONREM_API AMould : public ATool {
	GENERATED_BODY()

public:
	AMould();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MouldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* MouldInput;

	UPROPERTY(ReplicatedUsing=OnRep_Filled, EditAnywhere, BlueprintReadWrite)
	bool bFilled = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Filled();

	void UseMould();

protected:
	UFUNCTION()
	void OverlapMould(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};