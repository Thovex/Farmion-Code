// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "ForestPest.h"
#include "Snake.generated.h"

class USphereComponent;

UCLASS(Blueprintable)
class FARMIONREM_API ASnake : public AForestPest {
	GENERATED_BODY()
public:
	ASnake();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* AttackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* WarnAudioSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* BiteSphere;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayWarnAudio();

	UFUNCTION(Server, Reliable)
	void Server_Bite(AFarmionCharacter* Target);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Bite(AFarmionCharacter* Target);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DoBite(AFarmionCharacter* Target);

protected:
	UPROPERTY()
	TArray<AFarmionCharacter*> Targets;
	
	UFUNCTION()
	void OnWarnAudio(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnStopWarnAudio(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnBite(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void OnRep_Death() override;

	virtual void PerformCycle() override;
	

private:
	FTimerHandle AttackAnimTimerHandle;
};