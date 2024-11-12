// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "GameFramework/Actor.h"
#include "Ping.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API APing : public AActor {
	GENERATED_BODY()

public:
	APing();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_PrimaryColor)
	FColor PrimaryColor;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_PrimaryColor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lerp")
	UCurveFloat* LerpCurve;

	FTransform TargetTransform;
	
 	float AttachElapsedTime;
	
	FTimerHandle AttachTimerHandle;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TransformToLocation(const FTransform& T);

	void AttachCoroutine();
	
protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	

private:
	FTimerHandle MovementTimerHandle;

};