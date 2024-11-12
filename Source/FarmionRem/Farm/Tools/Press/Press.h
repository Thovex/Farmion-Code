// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "FarmionRem/Interactables/Tool.h"
#include "FarmionRem/Interfaces/Liquid/LiquidSource.h"
#include "Press.generated.h"

class AProduct;
class USphereComponent;

UCLASS(Blueprintable)
class FARMIONREM_API APress : public ATool, public ILiquidSource {
	GENERATED_BODY()

public:
	APress();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* InputArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AProduct>, TEnumAsByte<ELiquids>> InputTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TSubclassOf<AProduct> SelectedType = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Progress)
	float Progress = 0.0f;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Progress();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D ProgressRange = FVector2D(0.0f, 100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_InputCount)
	int32 InputCount;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_InputCount();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InputCountRange = FVector2D(0, 8);

protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	
	UFUNCTION()
	void InputOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FText GetInformationText_Implementation() const override;

	virtual bool IsSourceAvailable() override;
	virtual void UseSource() override;
	virtual TEnumAsByte<ELiquids> GetLiquidType() override;
	virtual TEnumAsByte<EProductColor> GetLiquidColor() override;
};