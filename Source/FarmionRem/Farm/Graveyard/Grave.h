// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Components/TextRenderComponent.h"
#include "FarmionRem/Interactables/InteractableObject.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "Grave.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGraveActiveChanged, bool, Active);

class UBoxComponent;
UCLASS(Blueprintable)
class FARMIONREM_API AGrave : public AInteractableObject, public ICycleAffected {
	GENERATED_BODY()

public:
	AGrave();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* GraveMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* GraveActiveMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* FlowerArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* Nameplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SetNameplate)
	FName GravePlayerName;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_SetNameplate();
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_ActiveGrave)
	bool bActiveGrave;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_ActiveGrave();

	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FOnGraveActiveChanged OnGraveActiveChanged;
	
protected:
	UFUNCTION()
	void OnFlowerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PopVfx(TSubclassOf<AActor> VfxClass);

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;

	virtual FText GetInformationText_Implementation() const override;
	
	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	
	virtual void PerformCycle() override;

};