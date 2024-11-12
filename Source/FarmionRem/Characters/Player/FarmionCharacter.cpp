// Copyright 2020-2023: Jesse J. van Vliet

#include "FarmionCharacter.h"

#include "FarmionInputConfig.h"
#include "InputActionValue.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/GlobalValues.h"
#include "FarmionRem/Characters/Player/FarmionPlayerController.h"
#include "FarmionRem/Characters/Player/FarmionStatsComponent.h"
#include "FarmionRem/Characters/Player/NameplateWidget.h"
#include "FarmionRem/Characters/Player/Ping.h"
#include "FarmionRem/Farm/Graveyard/Grave.h"
#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Farm/Planting/Seeds/InteractableProductSeed.h"
#include "FarmionRem/Farm/Tools/IgnoreDecayVolume.h"
#include "FarmionRem/Interfaces/Hover/Information.h"
#include "FarmionRem/Interfaces/Interactable/Buyable.h"
#include "FarmionRem/Interfaces/Interactable/Carryable.h"
#include "FarmionRem/Interfaces/Interactable/Interactable.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "FarmionRem/Util/MathLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "AdvancedSessionsLibrary.h"

#include "Net/UnrealNetwork.h"


AFarmionCharacter::AFarmionCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	// Set Camera Details
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = true;
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0, 0, 25);
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->AddLocalOffset(FVector(0, 0, 100));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(CameraBoom);
	CameraBoom->AddLocalRotation(FRotator(0, -10, 0));

	FPSCameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("FPSCameraBoom"));
	FPSCameraBoom->bUsePawnControlRotation = true;
	FPSCameraBoom->bInheritPitch = true;
	FPSCameraBoom->bInheritYaw = true;
	FPSCameraBoom->bInheritRoll = true;
	FPSCameraBoom->TargetArmLength = 0.f;
	FPSCameraBoom->SetupAttachment(GetCapsuleComponent());

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(FName("FPSCamera"));
	FPSCamera->SetupAttachment(FPSCameraBoom);
	
	CarryPivot = CreateDefaultSubobject<USceneComponent>(FName("CarryPivot"));
	CarryPivot->SetupAttachment(GetMesh());
	
	CarryBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CarryBoom"));
	CarryBoom->SetupAttachment(CarryPivot);

	CarryPos = CreateDefaultSubobject<USceneComponent>(FName("CarryPos"));
	CarryPos->SetupAttachment(CarryBoom);

	PingMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("PingMesh"));
	PingMesh->SetupAttachment(GetMesh(), FName("PingSocket"));

	NameBoom = CreateDefaultSubobject<USpringArmComponent>(FName("NameBoom"));
	NameBoom->SetupAttachment(GetMesh());

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(FName("NameWidget"));
	NameWidget->SetupAttachment(NameBoom);

	NameSphere = CreateDefaultSubobject<USphereComponent>(FName("NameSphere"));
	NameSphere->SetupAttachment(GetMesh());

	Stats = CreateDefaultSubobject<UFarmionStatsComponent>(FName("Stats"));

	PettoBoom = CreateDefaultSubobject<USpringArmComponent>(FName("PettoBoom"));
	PettoBoom->SetupAttachment(GetMesh());

	BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	Stamina = MaxStamina;
}

void AFarmionCharacter::OnRep_Petto_Implementation() {}

void AFarmionCharacter::Server_Use_Implementation(AActor* Useable, const bool bUse) {
	AInteractableObject* InteractableObject = Cast<AInteractableObject>(Useable);
	check(InteractableObject);

	InteractableObject->Server_Use(this, bUse);
}

void AFarmionCharacter::Server_Sell_Implementation(AActor* Sellable) {
	const TScriptInterface<ISellable> SellableInt = TScriptInterface<ISellable>(Sellable);
	check(SellableInt);

	GAME_STATE

	if (SellableInt->CanSell()) {
		GameState->Money += SellableInt->GetFullSellPrice();
		GameState->OnRep_Money();

		SellableInt->Sell();
	}
}

void AFarmionCharacter::Server_Buy_Implementation(AActor* Buyable) {
	const TScriptInterface<IBuyable> BuyableInt = TScriptInterface<IBuyable>(Buyable);
	check(BuyableInt);

	GAME_STATE

	if (BuyableInt->CanBuy(GameState->Money)) {
		GameState->Money -= BuyableInt->Execute_GetBuyPrice(BuyableInt.GetObject());
		GameState->OnRep_Money();

		BuyableInt->Buy();
	}
}

#pragma region BeginInteract

void HandleInteract(AFarmionCharacter* Character, const FHitResult& HitResult, const int32 InCallback) {
	AActor* Target = HitResult.GetActor();
	UE_LOG(LogPlayer, Log, TEXT("[%s] Interact with %s"), *GET_STR_ROLE(Character->GetRemoteRole()), *Target->GetFullName());

	if (UCommonLibrary::HasFlag<EInteractCallback>(InCallback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED)) {
		UE_LOG(LogPlayer, Log, TEXT("[%s] PREDICTION: Start Carrying %s"), *GET_STR_ROLE(Character->GetRemoteRole()), *HitResult.GetActor()->GetFullName());
		Character->Server_HandleStartCarrying(Target, HitResult);
		return; // TODO: Does this need to be here?
	}

	if (UCommonLibrary::HasFlag<EInteractCallback>(InCallback, (int32)EInteractCallback::START_USING_OBJECT)) {
		UE_LOG(LogPlayer, Log, TEXT("[%s] Start Using %s"), *GET_STR_ROLE(Character->GetRemoteRole()), *HitResult.GetActor()->GetFullName());
		Character->Server_Use(Target, true);
	}

	if (UCommonLibrary::HasFlag<EInteractCallback>(InCallback, (int32)EInteractCallback::STOP_USING_OBJECT)) {
		UE_LOG(LogPlayer, Log, TEXT("[%s] Stop Using %s"), *GET_STR_ROLE(Character->GetRemoteRole()), *HitResult.GetActor()->GetFullName());
		Character->Server_Use(Target, false);
	}

	if (UCommonLibrary::HasFlag<EInteractCallback>(InCallback, (int32)EInteractCallback::INTERACTABLE_CAN_BE_BOUGHT)) {
		UE_LOG(LogPlayer, Log, TEXT("[%s] Try Buy! %s"), *GET_STR_ROLE(Character->GetRemoteRole()), *HitResult.GetActor()->GetFullName());
		Character->Server_Buy(Target);
	}

	if (UCommonLibrary::HasFlag<EInteractCallback>(InCallback, (int32)EInteractCallback::INTERACTABLE_CAN_BE_SOLD)) {
		UE_LOG(LogPlayer, Log, TEXT("[%s] Try Sell! %s"), *GET_STR_ROLE(Character->GetRemoteRole()), *HitResult.GetActor()->GetFullName());
		Character->Server_Sell(Target);
	}
}


void AFarmionCharacter::OnRep_Rotation() {
	CarryBoom->SetWorldRotation(CarryBoomRotation);
}

void AFarmionCharacter::Interact() {
	if (HeldObject) {
		StopInteract();
		return;
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	Params.bTraceComplex = true;
	constexpr float RaycastLength = 1000.f;

	const FVector CameraLocation = (bThirdPersonCamera ? Camera : FPSCamera )->GetComponentLocation();
	const FVector CameraForwardVector = (bThirdPersonCamera ? Camera : FPSCamera )->GetForwardVector();
	const FVector EndPoint = CameraLocation + (CameraForwardVector * RaycastLength);

	FHitResult SingleHitResult;

	// TODO (Jesse): Is this commented out neccessary? Who knows lmao
	if (GetWorld()->LineTraceSingleByChannel(SingleHitResult, CameraLocation, EndPoint, ECC_Visibility, Params)) {
		constexpr float SphereRadius = 50.f; // Define the radius for your sphere overlap

		//DrawDebugLine(GetWorld(), CameraLocation, EndPoint, FColor::Red, false, 1.f, 0, 1.f);
		UE_LOG(LogTemp, Log, TEXT("Interacting with Actor: %s. Comp: %s"), *SingleHitResult.GetActor()->GetName(), *SingleHitResult.GetComponent()->GetName());

		TArray<FHitResult> SphereHits;
		if (UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), SingleHitResult.Location, SingleHitResult.Location, SphereRadius, {UEngineTypes::ConvertToObjectType(ECC_Visibility)}, true, {this}, EDrawDebugTrace::None, SphereHits, true)) {

			//DrawDebugSphere(GetWorld(), SingleHitResult.Location, SphereRadius, 12, FColor::Red, false, 1.f, 0, 1.f);

			SphereHits.Sort([](const FHitResult& A, const FHitResult& B) {
				const bool AIgnores = (A.Component != nullptr && A.Component->GetCollisionResponseToChannel(ECC_GameTraceChannel1) == ECR_Ignore);
				const bool BIgnores = (B.Component != nullptr && B.Component->GetCollisionResponseToChannel(ECC_GameTraceChannel1) == ECR_Ignore);

				if (AIgnores && !BIgnores) {
					return true; // A ignores, B does not ignore - A comes before B
				}

				if (!AIgnores && BIgnores) {
					return false; // B ignores, A does not ignore - B comes before A
				}

				return false; // Both ignore or both do not ignore - maintain the original order
			});

			// Sort HitResults based on the priority of the interactable objects
			SphereHits.Sort([](const FHitResult& A, const FHitResult& B) {
				const IInteractable* InteractableA = Cast<IInteractable>(A.GetActor());
				const IInteractable* InteractableB = Cast<IInteractable>(B.GetActor());

				const int32 PriorityA = InteractableA ? InteractableA->GetPriority() : TNumericLimits<int32>::Lowest();
				const int32 PriorityB = InteractableB ? InteractableB->GetPriority() : TNumericLimits<int32>::Lowest();

				return PriorityA > PriorityB;
			});

			for (auto& HitResult : SphereHits) {
				if (HitResult.Component->ComponentHasTag(GCarryable_Mesh_Ignore_Tag_Name)) {
					continue;
				}

				const TScriptInterface<IInteractable> InteractableInt(HitResult.GetActor());
				if (InteractableInt) {
					const int32 Callback = InteractableInt->Interact(this, HitResult);
					if (UCommonLibrary::HasAnyFlag(Callback)) {
						HandleInteract(this, HitResult, Callback);
						break;
					}
				}
			}
		}
	}
}

void AFarmionCharacter::Server_HandleStartCarrying_Implementation(AActor* Actor, const FHitResult& HitResult) {
	if (!Actor) {
		return;
	}

	const int32 Callback = TScriptInterface<IInteractable>(Actor)->Interact(this, HitResult);
	TScriptInterface<ICarryable>(Actor)->Server_MeetsCarryRequirements(this);

	if (UCommonLibrary::HasFlag(Callback, EInteractCallback::CARRYABLE_CAN_BE_CARRIED)) {
		UE_LOG(LogPlayer, Log, TEXT("PREDICTION CORRECT. %s"), *Actor->GetName());

		// This turns ProductSeeds (Like Carrots, Seeds turned into Products) to use diff. meshes, etc (see: blueprint) depending if harvested or not.
		AInteractableProductSeed* AsSeed = Cast<AInteractableProductSeed>(Actor);
		if (AsSeed && !AsSeed->bPicked) {
			AsSeed->bPicked = true;
			AsSeed->OnRep_Picked();
		}
	} else {
		UE_LOG(LogPlayer, Error, TEXT("PREDICTION INCORRECT. Whatever Carry case is, we need to Sync it to our Boy. %s"), *Actor->GetName());
		return;
	}

	if (UCommonLibrary::HasFlag(Callback, EInteractCallback::CONSUME_OBJECT)) {
		UE_LOG(LogPlayer, Warning, TEXT("Consuming %s"), *Actor->GetName());

		AInteractableObject* InteractableObject = Cast<AInteractableObject>(Actor);
		if (InteractableObject && !InteractableObject->bConsumed) {
			InteractableObject->Server_Consume();
		}
	}

	// Handle special network cases, is preferred to be on the client, however it is not possible to call a server function from a local without
	// Assigning ownership, which is fucking annoying to do, so we do it here instead.

	/* This case handles: - Seeds whom are assigned a CropPtr, which means they are "attached" to a Crop and not yet "harvested" by a player.
	 * This will "unhook" them from the crop once they have been picked up, the server needs to do this to validate the amount of remaining
	 * produce on the Crop. Notably necessary for loaded Crops which may have variable amounts of products left. */
	if (auto* SeedProduct = Cast<AInteractableProductSeed>(Actor)) {
		if (SeedProduct->CropPtr) {
			SeedProduct->CropPtr->PickupProduct(SeedProduct);
			SeedProduct->CropPtr = nullptr;
			SeedProduct->Tags.Remove(GProtected_Tag_Name);
		}
	}

	Multicast_HandleStartCarrying(Actor);
}

void AFarmionCharacter::Multicast_HandleStartCarrying_Implementation(AActor* Actor) {
	if (Actor == nullptr) {
		return;
	}

	HeldObject = Actor;
	HeldObject->SetCollisions(ECollisionEnabled::Type::NoCollision);

	HeldObject->CarryTransforms = { HeldObject->GetActorFromInterface()->GetActorTransform(), CarryPos->GetComponentTransform() };
	HeldObject->CarryTarget = CarryPos;
	HeldObject->bMustWeld = true;
	HeldObject->bCarry = true;

	HeldObject->StartCarryTick();

	Multicast_CastAudio_Carryable(HeldObject->GetActorFromInterface()->GetActorLocation(), HeldObject->Execute_GetCarryEffectType(HeldObject->_getUObject()), true);
	
	HeldObject->bIsBeingCarriedEarly = true;
}

#pragma endregion

#pragma region StopInteract
void AFarmionCharacter::StopInteract() {
	if (!HeldObject) {
		Interact();
		return;
	}
	
	if (HeldObject) {
		if (UCommonLibrary::HasFlag(HeldObject->MeetsDropRequirements(this), EInteractCallback::CARRYABLE_CAN_BE_DROPPED)) {
			Server_HandleStopCarrying();
		}
	}
}

void AFarmionCharacter::Server_StartSprinting_Implementation() {
	StartSprinting();
}

void AFarmionCharacter::Server_StopSprinting_Implementation() {
	StopSprinting();
}

void AFarmionCharacter::OnRep_IsSprinting_Implementation() {}

void AFarmionCharacter::HandleStamina(float DeltaTime) {
	if (bIsSprinting && Stamina > 0) {
		Stamina -= StaminaDepletionRate * DeltaTime;
		if (Stamina < 0) {
			StopSprinting();
		}
	} else if (Stamina < MaxStamina) {
		Stamina += StaminaRegenerationRate * DeltaTime;
		if (Stamina > MaxStamina) {
			Stamina = MaxStamina;
		}
	}
}

void AFarmionCharacter::Server_Bite_Implementation() {
	GAME_STATE

	bool bHasBiteResistance;
	const float BiteResistanceValue = GameState->GetBuffValue(0.f, EBuffType::SnakeBiteResistance, bHasBiteResistance);

	if (bHasBiteResistance && BiteResistanceValue > 0.f) {
		UE_LOG(LogPlayer, Log, TEXT("Bite Resistance: %f"), BiteResistanceValue);
	} else {
		Multicast_Bite();
	}
}

void AFarmionCharacter::Multicast_Bite_Implementation() {
	Stamina = 0;
	bMovementImpedingHit = true;

	this->StaminaRegenerationRate = 0.f;

	if (bIsSprinting) {
		StopSprinting();
	}
	
	GetWorldTimerManager().ClearTimer(TimerTillRecovery);
	GetWorldTimerManager().SetTimer(TimerTillRecovery, [this] {
		bMovementImpedingHit = false;
		this->StaminaRegenerationRate = this->OriginalStaminaRegenerationRate;

		
	}, 10.f, false); // Note: Hardcoded 10, also set in Bite_BP()... Cba to variable

	if (IsLocallyControlled()) {
		Bite_BP();
	}
}

void AFarmionCharacter::UpdateNameplates() {
	UNameplateWidget* Widget = Cast<UNameplateWidget>(NameWidget->GetWidget());

	if (Widget) {
		if (GetPlayerState()) {
			bIsNameSet = true;
			Widget->SetName(Cast<AFarmionPlayerController>(GetOwner()), FName(GetPlayerState()->GetPlayerName()));
		} else {
			bIsNameSet = false;
			Widget->SetName(Cast<AFarmionPlayerController>(GetOwner()), FName("Unknown"));
		}
	}
}

void AFarmionCharacter::OnNameplateOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA<AFarmionCharacter>()) {
		if (!IsLocallyControlled()) {
			if (NameWidget && NameWidget->GetWidget()) {
				NameWidget->GetWidget()->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void AFarmionCharacter::OnNameplateEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA<AFarmionCharacter>()) {
		if (!IsLocallyControlled()) {
			if (NameWidget && NameWidget->GetWidget()) {
				NameWidget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void AFarmionCharacter::BeginPlay() {
	Super::BeginPlay();
	
	GAME_STATE
	GameState->OnUpdateNameplates.AddDynamic(this, &AFarmionCharacter::UpdateNameplates);
	NameSphere->OnComponentBeginOverlap.AddDynamic(this, &AFarmionCharacter::OnNameplateOverlap);
	NameSphere->OnComponentEndOverlap.AddDynamic(this, &AFarmionCharacter::OnNameplateEndOverlap);

	OriginalWalkSpeed = BaseWalkSpeed;
	OriginalStaminaRegenerationRate = StaminaRegenerationRate;

	if (IsLocallyControlled()) {
		if (NameWidget && NameWidget->GetWidget()) {
			NameWidget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	CameraOffsetLerp = 325.f;

	if (HasAuthority()) {
		Server_SetClothingColor();
	}
}

void AFarmionCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	float NewWalkSpeed;
	if (bMovementImpedingHit) {
		NewWalkSpeed = 200;
	} else {
		GAME_STATE
		float CalculatedWalkSpeed = (BaseWalkSpeed + GameState->FlatMovementSpeedBonus);
		CalculatedWalkSpeed += BaseWalkSpeed * (GameState->PercentageMovementSpeedBonus * 0.01f);
		CalculatedWalkSpeed *= bIsSprinting ? SprintSpeedMultiplier : 1.0f;
		NewWalkSpeed = CalculatedWalkSpeed;
	}

	if (PreviousWalkSpeed != NewWalkSpeed) {
		GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;
		PreviousWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}

	if (bCharging) {
		if (HasAuthority()) {
			ServerCharge();
		}

		Charge();
		
		FPredictProjectilePathParams Params;
		Params.StartLocation = CarryPos->GetComponentLocation();
		Params.LaunchVelocity = ThrowVelocity;
		Params.bTraceWithCollision = true;
		Params.bTraceComplex = true;
		Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;

		FPredictProjectilePathResult Result;
		UGameplayStatics::PredictProjectilePath(this, Params, Result);
	}

	if (!bIsNameSet) {
		UpdateNameplates();
	}

	if (IsLocallyControlled()) {

		if (!bThirdPersonCamera) {
			const FVector& FPSTargetLocation = GetMesh()->GetSocketLocation(FName("headCamera"));
			FVector NewLocation = FMath::VInterpTo(FPSCameraBoom->GetComponentLocation(), FPSTargetLocation, DeltaSeconds, 10.f);
			FPSCameraBoom->SetWorldLocation(NewLocation);
		}
		
		if (CameraOffsetLerp != CameraBoom->TargetArmLength) {
			CameraOffsetLerp = FMath::Lerp(CameraOffsetLerp, CameraBoom->TargetArmLength, DeltaSeconds * 5.0f);
			CameraBoom->TargetArmLength = CameraOffsetLerp;
		}

		CurrentInformation = FText();

		TArray<FHitResult> HitResults;
		TArray<TScriptInterface<IInformation>> AddedInformationActors;

		const float RaycastLength = 1000.f;
		
		if (GetWorld()->LineTraceMultiByChannel(HitResults,
			(bThirdPersonCamera ? Camera : FPSCamera )->GetComponentLocation(),
			(bThirdPersonCamera ? Camera : FPSCamera )->GetComponentLocation() + ((bThirdPersonCamera ? Camera : FPSCamera )->GetForwardVector() * RaycastLength),
			ECollisionChannel::ECC_Visibility)) {

			HitResults.Sort([](const FHitResult& A, const FHitResult& B) {
				const IInteractable* InteractableA = Cast<IInteractable>(A.GetActor());
				const IInteractable* InteractableB = Cast<IInteractable>(B.GetActor());

				if (InteractableA == nullptr || InteractableB == nullptr) {
					return false;
				}

				const int32 PriorityA = InteractableA ? InteractableA->GetPriority() : TNumericLimits<int32>::Lowest();
				const int32 PriorityB = InteractableB ? InteractableB->GetPriority() : TNumericLimits<int32>::Lowest();

				return PriorityA > PriorityB;
			});

			for (auto& HitResult : HitResults) {
				if (HitResult.Component->ComponentHasTag(GCarryable_Mesh_Ignore_Tag_Name)) {
					continue;
				}

				TScriptInterface<IIQuality> QualityInt = TScriptInterface<IIQuality>(HitResult.GetActor());
				TScriptInterface<IInformation> InformationInt = TScriptInterface<IInformation>(HitResult.GetActor());
				if (InformationInt && !AddedInformationActors.Contains(InformationInt)) {
					if (!InformationInt->IsActive()) {
						continue;
					}
					
					FText TextToAdd = InformationInt->Execute_GetInformationText(InformationInt.GetObject());
					if (TextToAdd.IsEmpty()) {
						continue;
					}

					if (QualityInt) {
						QualityTransformText(QualityInt->GetQuality(), TextToAdd);
					}

					if (CurrentInformation.IsEmpty()) {
						CurrentInformation = TextToAdd;
					} else {
						CurrentInformation = FText::FromString(TextToAdd.ToString() + "\n\n" + CurrentInformation.ToString());
					}
					AddedInformationActors.Add(InformationInt);
				}
			}
		}

		AFarmionPlayerController* PlayerController = Cast<AFarmionPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PlayerController) {
			PlayerController->UpdateInformationWidget(CurrentInformation);
		}
	}


	if (HasAuthority()) {
		TimeSinceLastUpdate += DeltaSeconds;
		HandleStamina(DeltaSeconds);

		// Update every 0.25 seconds using modulo
		if (TimeSinceLastUpdate >= 0.25f) {
			TimeSinceLastUpdate = FMath::Fmod(TimeSinceLastUpdate, 0.25f);

			CarryBoomRotation = CarryBoom->GetComponentRotation();
			float ControlPitch = GetControlRotation().Pitch;

			if (ControlPitch > 180.0f) {
				ControlPitch -= 360.0f;
			}

			const float ClampedPitch = UMathLibrary::SetScale(ControlPitch, -90.f, 90.f, -15.f, 70.f);
			CarryBoomRotation.Pitch = -ClampedPitch;
			CarryBoom->SetWorldRotation(CarryBoomRotation);
			OnRep_Rotation();
		}
	}
}

void AFarmionCharacter::Server_HandleStopCarrying_Implementation() {
	if (!HeldObject) {
		return;
	}

	AInteractableObject* Interactable = Cast<AInteractableObject>(HeldObject->GetActorFromInterface());

	TArray<FVector> DropPath;
	FRotator DropRotation;
	const int32 Callback = HeldObject->Server_MeetsDropRequirements(this, DropPath, DropRotation);
	if (UCommonLibrary::HasFlag(Callback, EInteractCallback::CARRYABLE_CAN_BE_DROPPED)) {
		// Handle special network cases, is preferred to be on the client, however it is not possible to call a server function from a local without
		// Assigning ownership, which is fucking annoying to do, so we do it here instead.

		if (UCommonLibrary::HasFlag(Callback, EInteractCallback::INTERACTABLE_CAN_BE_SOLD)) {
			GAME_STATE
			GameState->Sell(TScriptInterface<ISellable>(Interactable), HeldObject->SellContainer);
		}

		Multicast_HandleStopCarrying(Callback, DropPath, DropRotation);
	}
}

void AFarmionCharacter::Multicast_HandleStopCarrying_Implementation(const int32& Callback, const TArray<FVector>& DropPath, const FRotator& DropRotation) {

	HeldObject->GetActorFromInterface()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	HeldObject->CarryTransforms.Empty();
	HeldObject->CarryTransforms.Push( CarryPos->GetComponentTransform());
	for (auto& Path : DropPath) {
		HeldObject->CarryTransforms.Push(FTransform(DropRotation, Path, HeldObject->GetActorFromInterface()->GetActorScale3D()));
	}

	HeldObject->bCarry = false;
	HeldObject->StartCarryTick();

	Multicast_CastAudio_Carryable(HeldObject->GetActorFromInterface()->GetActorLocation(), HeldObject->Execute_GetDropEffectType(HeldObject->_getUObject()), false);

	HeldObject = nullptr;
}

#pragma endregion

#pragma region Input

void AFarmionCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AFarmionCharacter::Move);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AFarmionCharacter::Look);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &AFarmionCharacter::Jump);
	PEI->BindAction(InputActions->InputInteract, ETriggerEvent::Triggered, this, &AFarmionCharacter::Interact);
	PEI->BindAction(InputActions->InputPing, ETriggerEvent::Triggered, this, &AFarmionCharacter::SpawnPing);
	PEI->BindAction(InputActions->InputScroll, ETriggerEvent::Triggered, this, &AFarmionCharacter::Scroll);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Started , this, &AFarmionCharacter::StartSprinting);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Completed , this, &AFarmionCharacter::StopSprinting);
	PEI->BindAction(InputActions->InputCharge, ETriggerEvent::Started , this, &AFarmionCharacter::StartCharge);
	PEI->BindAction(InputActions->InputCharge, ETriggerEvent::Completed , this, &AFarmionCharacter::StopCharge);
	PEI->BindAction(InputActions->InputCamera, ETriggerEvent::Triggered, this, &AFarmionCharacter::ToggleCamera);

}

void AFarmionCharacter::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);
		
		if (MoveValue.Y != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(Direction, MoveValue.Y);
		}
		if (MoveValue.X != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(Direction, MoveValue.X);
		}
	}
}

void AFarmionCharacter::Look(const FInputActionValue& Value) {
	if (Controller != nullptr) 	{
		const FVector2d LookValue = Value.Get<FVector2d>();
		if (LookValue.X != 0.f) {
			AddControllerYawInput(LookValue.X);
		}
		
		if (LookValue.Y != 0.f) {
			AddControllerPitchInput(LookValue.Y);
		}
	}
}

void AFarmionCharacter::StartSprinting() {
	if (GetLocalRole() < ROLE_Authority) {
		Server_StartSprinting();
	}

	if (Stamina > 0) {
		bIsSprinting = true;
		OnRep_IsSprinting();
	}
}

void AFarmionCharacter::StopSprinting() {
	if (GetLocalRole() < ROLE_Authority) {
		Server_StopSprinting();
	}
	
	bIsSprinting = false;
	OnRep_IsSprinting();
}

#pragma endregion

void AFarmionCharacter::Scroll(const FInputActionValue& Value)
{
	if (Value.Get<float>() > 0.f)
	{
		CameraOffsetLerp -= 5.f;
		if (CameraOffsetLerp < MinCameraOffset) {
			CameraOffsetLerp = MinCameraOffset;
		}
	}
	else
	{
		CameraOffsetLerp += 5.f;
		if (CameraOffsetLerp > MaxCameraOffset) {
			CameraOffsetLerp = MaxCameraOffset;
		}
	}
}



void AFarmionCharacter::Charge() {
	CarryBoom->TargetArmLength = FMath::Lerp(200.f, 150.f, ChargeAlpha);
	CarryPivot->SetRelativeRotation(FRotator(0.f, 0.f, FMath::Lerp(0.f, -15.f, ChargeAlpha)));
}

void AFarmionCharacter::ServerCharge() {
	ChargeAlpha = FMath::Min(ChargeAlpha + (1.f * GetWorld()->GetDeltaSeconds()), 1.f);
	ThrowVelocity = FMath::Lerp(FVector::ZeroVector, (((bThirdPersonCamera ? Camera : FPSCamera )->GetForwardVector() * 1.25f) + ((bThirdPersonCamera ? Camera : FPSCamera )->GetUpVector() * 0.75f)) * 850, ChargeAlpha);
}

void AFarmionCharacter::Server_StartCharge_Implementation() {
	bCharging = true;
	
	ChargeAlpha = 0.f;
	ThrowVelocity = FVector::ZeroVector;
}

void AFarmionCharacter::StartCharge() {
	bCharging = true;
	Server_StartCharge();
}

void AFarmionCharacter::Server_StopCharge_Implementation() {
	bCharging = false;
	
	ChargeAlpha = 0.f;
	ThrowVelocity = FVector::ZeroVector;
}

void AFarmionCharacter::StopCharge() {
	bCharging = false;

	ChargeAlpha = 0.f;
	ThrowVelocity = FVector::ZeroVector;

	Charge();
	
	Server_StopCharge();
}

void AFarmionCharacter::ToggleCamera() {
	bThirdPersonCamera = !bThirdPersonCamera;
	if (bThirdPersonCamera) {
		FPSCamera->Deactivate();
		Camera->Activate();
	} else {
		Camera->Deactivate();
		FPSCamera->Activate();
	}

	SetMeshForFPS(!bThirdPersonCamera);
	
	PingMesh->SetVisibility(bThirdPersonCamera);
}

ELoadMethod AFarmionCharacter::LoadMethod() {
	return ELoadMethod::ManualReset;
}

void AFarmionCharacter::PerformCycle() {
	TArray<AActor*> Graves;
	UGameplayStatics::GetAllActorsOfClass(this, AGrave::StaticClass(), Graves);

	GAME_STATE

	for (AActor* Grave : Graves) {
		AGrave* GravePtr = Cast<AGrave>(Grave);
		if (!GravePtr->bActiveGrave) {
			GravePtr->bActiveGrave = true;
			GravePtr->OnRep_ActiveGrave();

			const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
			if (OnlineSubsystem) {
				const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
				if (IdentityInterface.IsValid()) {
					const TSharedPtr<const FUniqueNetId> PlayerId = IdentityInterface->GetUniquePlayerId(0);
					if (PlayerId.IsValid() && PlayerId->IsValid()) {
						FString PlayerName = Cast<APlayerController>(GetController())->GetPlayerState<APlayerState>()->GetPlayerName();

						GravePtr->GravePlayerName = FName(PlayerName);
						GravePtr->OnRep_SetNameplate();

						GameState->ReceiveEventMessage_Tunnel(FText::FromString(FString::Printf(TEXT("%s has used a spare grave."), *PlayerName)));
					}
				}
			}
			break;
		}
	}

#ifdef WITH_EDITOR
	return;
#endif

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), SpawnPoints);

	const AActor* TargetSpawnPt = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
	SetActorLocation(TargetSpawnPt->GetActorLocation());
	SetActorRotation(TargetSpawnPt->GetActorRotation());

	GameState->Money /= 2;
	GameState->OnRep_Money();
}

EDecayFlags AFarmionCharacter::GetDecayFlag() {
	return EDecayFlags::Player;
}

FText AFarmionCharacter::GetInformationText_Implementation() const {
	return FText::FromString("Player");
}

void AFarmionCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == MOVE_Falling && GetCharacterMovement()->MovementMode == MOVE_Walking) {
		bJustLanded = true;

		FTimerHandle JustLandedHandle;
		GetWorldTimerManager().SetTimer(JustLandedHandle, [this] {
			bJustLanded = false;
		}, 0.1f, false);
	} else {
		bJustLanded = false;
	}
}

void AFarmionCharacter::SpawnPing() {
	Server_SpawnPing((bThirdPersonCamera ? Camera : FPSCamera )->GetComponentLocation(), (bThirdPersonCamera ? Camera : FPSCamera )->GetForwardVector());	
}

void AFarmionCharacter::Server_SpawnPing_Implementation(const FVector& Location, const FVector& Forward) {

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + (Forward * 10000.0f), ECollisionChannel::ECC_Visibility)) {
		if (PingPtr.IsValid()) {
			PingPtr->Destroy(true);
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		PingPtr = GetWorld()->SpawnActor<APing>(PingClass, GetActorLocation(), GetActorRotation(), SpawnParameters);
		
		PingPtr->PrimaryColor = PrimaryColor;
		PingPtr->OnRep_PrimaryColor();
		
		PingPtr->SetLifeSpan(GPing_Lifetime);
		PingPtr->Multicast_TransformToLocation(FTransform(GetActorRotation().Quaternion(), HitResult.ImpactPoint, FVector::OneVector));

		GAME_STATE
		TScriptInterface<IInformation> Info = TScriptInterface<IInformation>(HitResult.GetActor());

		FString PingInfo = "Look here!";
		if (Info) {
			FText MiniText = Info->Execute_GetMiniInformationText(Info->_getUObject());

			if (!MiniText.IsEmpty()) {
				PingInfo = FString::Printf(TEXT("Look at: %s"), *MiniText.ToString());
			}
		}

		GameState->ReceiveMessage_Tunnel(this, FText::FromString(GetPlayerState()->GetPlayerName()), FText::FromString(PingInfo));
	}
}

FColor GenerateVibrantColor() {
	int32 r = FMath::RandRange(150, 255);
	int32 g = FMath::RandRange(150, 255);
	int32 b = FMath::RandRange(150, 255);

	// This factor determines the amount of vibrancy to add; 1.0 will leave the color unchanged, 
	// while values closer to 0 will make it more vibrant.
	float BlendFactor = 0.8f;

	// Blend the color with white to get a pastel shade
	r = static_cast<int32>(r * BlendFactor + 255 * (1 - BlendFactor));
	g = static_cast<int32>(g * BlendFactor + 255 * (1 - BlendFactor));
	b = static_cast<int32>(b * BlendFactor + 255 * (1 - BlendFactor));

	return FColor(r, g, b);
}

void AFarmionCharacter::Server_SetClothingColor_Implementation() {
    PrimaryColor = GenerateVibrantColor();
	OnRep_PrimaryColor();

    SecondaryColor = GenerateVibrantColor();
	OnRep_SecondaryColor();

	SkinColor = FColor(FMath::RandRange(200, 255), FMath::RandRange(150, 210), FMath::RandRange(130, 180));
	OnRep_SkinColor();
}

void AFarmionCharacter::Color_HelperFunc(TMap<int32, UMaterialInstanceDynamic*>& MaterialMappings, FColor Color) {
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++) {
		if (MaterialMappings.Contains(i)) {
			if (!MaterialMappings[i]) {
				if (!bThirdPersonCamera) {
					if (FPSMappings.Contains(i)) {
						MaterialMappings.Add(i, UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(i), nullptr));
						// Do not Set material for this index.. FPS mesh needs to be transparent
					}
				} else {
					MaterialMappings.Add(i, UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(i), nullptr));
					GetMesh()->SetMaterial(i, MaterialMappings[i]);
				}
			}

			MaterialMappings[i]->SetVectorParameterValue("Color_Tint_R", Color);
			MaterialMappings[i]->SetVectorParameterValue("Color_Tint_G", Color);
			MaterialMappings[i]->SetVectorParameterValue("Color_Tint_B", Color);
			
			MaterialMappings[i]->SetVectorParameterValue("Emissive_Color", Color);
		}
	}
}

void AFarmionCharacter::OnRep_PrimaryColor_Implementation() {
	Color_HelperFunc(PrimaryMaterialMappings, PrimaryColor);
}

void AFarmionCharacter::OnRep_SecondaryColor_Implementation() {
	Color_HelperFunc(SecondaryMaterialMappings, SecondaryColor);
}

void AFarmionCharacter::OnRep_SkinColor_Implementation() {
	Color_HelperFunc(SkinMaterialMappings, SkinColor);
}

void AFarmionCharacter::Server_CastAudio_Carryable_Implementation(const FVector& Location, const ESoundEffectType Effect, const bool bCarry) {
	Multicast_CastAudio_Carryable(Location, Effect, bCarry);
}

void AFarmionCharacter::Multicast_CastAudio_Carryable_Implementation(const FVector& Location, const ESoundEffectType Effect, const bool bCarry) {
	Multicast_CastAudio_Carryable_BP(Location, Effect, bCarry);
}


void AFarmionCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFarmionCharacter, Stamina);
	DOREPLIFETIME(AFarmionCharacter, MaxStamina);

	DOREPLIFETIME(AFarmionCharacter, bIsSprinting);
	DOREPLIFETIME(AFarmionCharacter, bMovementImpedingHit);
	DOREPLIFETIME(AFarmionCharacter, CarryBoomRotation);

	DOREPLIFETIME(AFarmionCharacter, ChargeAlpha);
	DOREPLIFETIME(AFarmionCharacter, ThrowVelocity);
	
	DOREPLIFETIME(AFarmionCharacter, PrimaryColor);
	DOREPLIFETIME(AFarmionCharacter, SecondaryColor);
	DOREPLIFETIME(AFarmionCharacter, SkinColor);

	DOREPLIFETIME(AFarmionCharacter, Petto);

}