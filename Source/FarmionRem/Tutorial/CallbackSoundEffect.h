// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "GameFramework/Actor.h"
#include "CallbackSoundEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnded);

UCLASS(Blueprintable)
class FARMIONREM_API ACallbackSoundEffect : public AActor {
	GENERATED_BODY()

public:
	ACallbackSoundEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bPlayAtLocation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TArray<USoundBase*> Sounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TArray<FText> Narrations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	float DelayBetweenSounds = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	float DelayTillCallback = 0.0f;

	UPROPERTY(BlueprintAssignable)
	FOnEnded OnEnded;

	UFUNCTION(BlueprintCallable)
	void PlaySound();

protected:
	UPROPERTY()
	int32 CurrentIndex = 0;
	
	virtual void BeginPlay() override;

private:
	void Evaluate();
	void Expire();
};