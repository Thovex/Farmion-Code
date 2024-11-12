// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Converter.h"
#include "FarmionRem/Interfaces/Fuel/FuelTarget.h"
#include "GameFramework/Actor.h"
#include "CookingSpit.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ACookingSpit : public AConverter, public IFuelTarget {
	GENERATED_BODY()

public:
	ACookingSpit();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Fuel();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Fuel)
	bool bFuel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Lit)
	bool bLit;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Lit();

protected:
	virtual void BeginPlay() override;

	virtual bool MatchesConditions() override;
	
	virtual bool MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ManipulateSpawnedObject(AInteractableObject* SpawnedObject) override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual bool Supply(int32 FuelValue) override;
	virtual bool Light() override;

	virtual int32 GetPriority() const override;

	virtual void Output() override;

};