// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Upgrades/StageUpgrade.h"
#include "FarmionRem/Interfaces/Liquid/LiquidSource.h"
#include "Beehive.generated.h"

class USphereComponent;

UCLASS(Blueprintable)
class FARMIONREM_API ABeehive : public AStageUpgrade, public ILiquidSource, public IInteractable, public ICarryable {
	GENERATED_BODY()

public:
	ABeehive();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Honey)
	bool bHoney;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Honey();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* BeehiveAura;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual bool IsSourceAvailable() override;

	FTimerHandle HoneyTimerHandle;

	UFUNCTION()
	void OnHoneyTimer();
	
	virtual void UseSource() override;
	virtual void Build() override;

	virtual TEnumAsByte<ELiquids> GetLiquidType() override;
	virtual TEnumAsByte<EProductColor> GetLiquidColor() override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	UFUNCTION()
	void OnBeehiveAuraBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBeehiveAuraEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual FText GetInformationText_Implementation() const override;
	virtual int32 GetPriority() const override;

	virtual void PostLoaded(const TSharedPtr<FJsonObject>& JsonObject, const TMap<uint32, AActor*>& UniqueIdToActorMap) override;
};
