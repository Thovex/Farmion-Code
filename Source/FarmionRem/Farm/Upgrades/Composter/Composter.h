// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Farm/Upgrades/StageUpgrade.h"
#include "FarmionRem/Interfaces/Liquid/LiquidSource.h"
#include "Composter.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AComposter : public AStageUpgrade, public ILiquidSource {
	GENERATED_BODY()

public:
	AComposter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CompostBin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* ComposterInAndOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentCompostValue)
	int32 CurrentCompostValue;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_CurrentCompostValue();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentCompostCharges)
	int32 CurrentCompostCharges;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_CurrentCompostCharges();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FVector GetGrindLocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredCompostValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCompostCharges;

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsSourceAvailable() override;
	virtual void UseSource() override;
	
	virtual TEnumAsByte<ELiquids> GetLiquidType() override;
	virtual TEnumAsByte<EProductColor> GetLiquidColor() override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;

};