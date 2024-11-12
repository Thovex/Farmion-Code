// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CoreMinimal.h"
#include "FarmionRem/Farm/Liquids.h"
#include "FarmionRem/Interactables/Tool.h"
#include "Bucket.generated.h"

class ILiquidTarget;
class ILiquidSource;
class UBoxComponent;

DECLARE_LOG_CATEGORY_EXTERN(FarmionBucket, Log, All);

UCLASS(Blueprintable)
class FARMIONREM_API ABucket : public ATool {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BucketMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LiquidMeshN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BucketCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Charges)
	int32 Charges = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Charges();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 MaxCharges = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Liquid)
	TEnumAsByte<ELiquids> Liquid = ELiquids::ELiquids_None;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Liquid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Color)
	TEnumAsByte<EProductColor> Color = EProductColor::EProductColor_White;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Color();

	ABucket();

	bool bOnCooldown = false;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual bool CanCompost_Implementation() const override;

private:
	UFUNCTION()
	void HandleBucketCollision();

	UFUNCTION()
	void BeginBucketCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndBucketCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<TScriptInterface<ILiquidTarget>> OverlappingTargets;
	TArray<TScriptInterface<ILiquidSource>> OverlappingSources;

};