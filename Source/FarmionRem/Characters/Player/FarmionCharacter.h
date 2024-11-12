// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "InputActionValue.h"
#include "FarmionRem/Interfaces/Cycle/CycleAffected.h"
#include "FarmionRem/Interfaces/Hover/Information.h"
#include "FarmionRem/Interfaces/Interactable/Carryable.h"
#include "FarmionRem/Io/SaveLoad.h"
#include "GameFramework/Character.h"

#include "FarmionCharacter.generated.h"

class APetto;
enum class ESoundEffectType : uint8;
enum class EInteractCallback : int32;
enum class EDecayFlags : uint8;

class AFarmionPlayerController;

class IBuyable;
class IInteractable;
class ICarryable;
class IInformation;

class APing;
class AInteractableObject;

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UWidgetComponent;
class UTextRenderComponent;
class UFarmionStatsComponent;

UCLASS(Blueprintable)
class FARMIONREM_API AFarmionCharacter : public ACharacter, public ICycleAffected, public IInformation, public ISaveLoad {
	GENERATED_BODY()

public:
	AFarmionCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFarmionStatsComponent* Stats;

#pragma region Start Carrying/StopCarrying
	
	UFUNCTION(Server, Reliable)
	void Server_HandleStartCarrying(AActor* Actor, const FHitResult& HitResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleStartCarrying(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void Server_HandleStopCarrying();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleStopCarrying(const int32& Callback, const TArray<FVector>& DropPath, const FRotator& DropRotation);

	UFUNCTION(Server, Reliable)
	void Server_Buy(AActor* Buyable);

	UFUNCTION(Server, Reliable)
	void Server_Sell(AActor* Sellable);

	UFUNCTION(Server, Reliable)
	void Server_Use(AActor* Useable, bool bUse);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaRegenerationRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaDepletionRate = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Petto)
	APetto* Petto = nullptr;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_Petto();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int32, UMaterialInstanceDynamic*> PrimaryMaterialMappings = {
		{0, nullptr},
		{1, nullptr},
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int32, UMaterialInstanceDynamic*> SecondaryMaterialMappings = {
		{4, nullptr},
		{5, nullptr},
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int32, UMaterialInstanceDynamic*> SkinMaterialMappings = {
		{6, nullptr},
		{7, nullptr},
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int32, UMaterialInstanceDynamic*> FPSMappings = {
		{1, nullptr},
		{7, nullptr},
	};

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetMeshForFPS(bool bFPS);

#pragma endregion
	TSoftObjectPtr<APing> PingPtr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APing> PingClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PingMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Name)
	USpringArmComponent* NameBoom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Name)
	UWidgetComponent* NameWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* NameSphere;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Carry)
	USceneComponent* CarryPivot;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Carry)
	USpringArmComponent* CarryBoom;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Carry)
	USceneComponent* CarryPos;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Camera)
	USpringArmComponent* FPSCameraBoom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Camera)
	UCameraComponent* FPSCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category=Petto)
	USpringArmComponent* PettoBoom;

	UPROPERTY(ReplicatedUsing = OnRep_Rotation)
	FRotator CarryBoomRotation;
	
	UFUNCTION()
	void OnRep_Rotation();

	UPROPERTY(Replicated)
	bool bMovementImpedingHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<ICarryable> HeldObject;

	void Interact();
	void StopInteract();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CurrentInformation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxCameraOffset = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinCameraOffset = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_PrimaryColor)
	FColor PrimaryColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SecondaryColor)
	FColor SecondaryColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SkinColor)
	FColor SkinColor;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetClothingColor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_PrimaryColor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_SecondaryColor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_SkinColor();
	
	UFUNCTION(Server, Unreliable)
	void Server_CastAudio_Carryable(const FVector& Location, ESoundEffectType Effect, bool bCarry);
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_CastAudio_Carryable(const FVector& Location, ESoundEffectType Effect, bool bCarry);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Multicast_CastAudio_Carryable_BP(const FVector& Location, ESoundEffectType Effect, bool bCarry);
	
	UFUNCTION(Server, Reliable)
	void Server_StartSprinting();

	UFUNCTION(Server, Reliable)
	void Server_StopSprinting();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_IsSprinting)
	bool bIsSprinting;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_IsSprinting();

	UPROPERTY(Replicated)
	FVector ThrowVelocity;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float ChargeAlpha;

	UFUNCTION(Server, Reliable)
	void Server_StartCharge();
	
	UFUNCTION(Server, Reliable)
	void Server_StopCharge();

	UFUNCTION(Server, Reliable)
	void Server_Bite();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Bite();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Bite_BP();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bThirdPersonCamera = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bJustLanded = false; // Unreliable on Client, Serverside only changes occur to this variable. Replicate if necessary.

protected:
	UFUNCTION()
	void UpdateNameplates();
	UFUNCTION()
	void OnNameplateOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnNameplateEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual ELoadMethod LoadMethod() override;

	virtual void PerformCycle() override;
	virtual EDecayFlags GetDecayFlag() override;

	virtual FText GetInformationText_Implementation() const override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UFarmionInputConfig* InputActions;

private:
	FTimerHandle TimerTillRecovery;

	bool bIsNameSet = false;

	void SpawnPing();

	UFUNCTION(Server, Reliable)
	void Server_SpawnPing(const FVector& Location, const FVector& Forward);

	void Color_HelperFunc(TMap<int32, UMaterialInstanceDynamic*>& MaterialMappings, FColor Color);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Scroll(const FInputActionValue& Value);

	float OriginalWalkSpeed;
	float OriginalStaminaRegenerationRate;
	float BaseWalkSpeed;
	float PreviousWalkSpeed;

	void StartSprinting();
	void StopSprinting();

	void HandleStamina(float DeltaTime);
	float SprintSpeedMultiplier = 1.5f;
	
	float CameraOffsetLerp;

	bool bCharging;
	void Charge();
	void ServerCharge();
	void StartCharge();
	void StopCharge();

	float TimeSinceLastUpdate;

	void ToggleCamera();
};