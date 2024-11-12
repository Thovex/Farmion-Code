// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CycleAffected.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "Cycle.generated.h"

class UDirectionalLightComponent;

UENUM(BlueprintType)
enum EWeatherEffect : uint8 {
	Day,
	Rain,
	Winter,
};

UCLASS(Blueprintable)
class FARMIONREM_API ACycle : public AInteractableObject, public ICycleAffected {
	GENERATED_BODY()

public:
	ACycle();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDirectionalLightComponent* Sun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<AActor> SkySphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<AActor> Chameleon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float DayDuration = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_TimeUntilNight)
	float TimeUntilNight = 300.f;

	UFUNCTION()
	void OnRep_TimeUntilNight();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateSkySphere(const AActor* FoundSkySphere);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateChameleon(const AActor* FoundChameleon, const float PercentageOfDay);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void FoundSkySphere(const AActor* FoundSkySphere);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_ForceTime(float PercentageOfDay);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_ResumeTime();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual ELoadMethod LoadMethod() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void StartWeatherEffect(EWeatherEffect Effect);

	virtual void PostPerformCycle() override;

private:
	float LastPercentage;

};