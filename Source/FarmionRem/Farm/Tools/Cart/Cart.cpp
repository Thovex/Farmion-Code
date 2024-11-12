// Copyright 2020-2023: Jesse J. van Vliet

#include "Cart.h"

#include "CartWheel.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ACart::ACart() {
	PrimaryActorTick.bCanEverTick = true;
	AActor::SetReplicateMovement(false);

	CartPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CartPivot"));
	CartPivot->SetupAttachment(RootComponent);

	CartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CartMesh"));
	CartMesh->SetupAttachment(CartPivot);

	CartCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CartCollision->SetupAttachment(CartMesh);

	WheelInputCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WheelInputCollision"));
	WheelInputCollision->SetupAttachment(CartMesh);

	WheelL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelL"));
	WheelL->SetupAttachment(CartMesh);

	WheelR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelR"));
	WheelR->SetupAttachment(CartMesh);
}

void ACart::OnRep_InUse_Implementation() {}

void ACart::OnRep_WheelCount_Implementation() {}

int32 ACart::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	if (!bBought) {
		return Super::Interact(InteractInstigator, InteractionHit);
	}

	if (WheelCount == 2) {
		return bInUse ? (int32)EInteractCallback::STOP_USING_OBJECT : (int32)EInteractCallback::START_USING_OBJECT;
	}

	return (int32)EInteractCallback::NO_CALLBACK;
}

int32 ACart::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

bool ACart::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	return Super::MatchesSocketRequirements(Info, Carryable);
}

int32 ACart::GetPriority() const {
	return -10;
}

void ACart::Server_Use(AFarmionCharacter* User, bool bUse) {
	if (User) {
		bInUse = bUse;
		OnRep_InUse();

		UserPtr = bUse ? User : nullptr;
	} else {
		bInUse = false;
		OnRep_InUse();

		UserPtr = nullptr;
	}

	Multicast_Use(UserPtr ? bUse : false);
	CartCollision->SetSphereRadius(0.f);
}

void ACart::Multicast_Use(bool bUse) {
	Super::Multicast_Use(bUse);

	PastPositions.Empty();
	SetActorTickEnabled(bUse);
}

void ACart::CartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (!bInUse) {
		return;
	}

	Server_Use(nullptr, false);
}

void ACart::WheelOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (Cast<ACartWheel>(OtherActor)) {
		if (WheelCount < 2) {

			WheelInputCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			FTimerHandle WheelInputCollisionTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(WheelInputCollisionTimerHandle, [this]() {
				WheelInputCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}, 1.0f, false);
			
			WheelCount++;
			OnRep_WheelCount();

			if (AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor)) {
				FTransform TargetTransform = WheelCount == 0 ? WheelL->GetComponentTransform() : WheelR->GetComponentTransform();
				TargetTransform.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
				InteractableObject->Multicast_OneTimeMove({
					OtherActor->GetActorTransform(),
					TargetTransform,
				});
			} else {
				OtherActor->Destroy(true);
			}
		}
	}
}

void ACart::BeginPlay() {
	Super::BeginPlay();

	OnRep_WheelCount();

	if (HasAuthority()) {
		CartCollision->OnComponentBeginOverlap.AddDynamic(this, &ACart::CartOverlap);
		WheelInputCollision->OnComponentBeginOverlap.AddDynamic(this, &ACart::WheelOverlap);
	}
}

void ACart::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	// Required so Server call (and Replication) does not crash on us.
	ACharacter* LocalUserPtr = UserPtr;
	
	if (!IsValid(LocalUserPtr) || LocalUserPtr == nullptr) {
		if (HasAuthority()) {
			Server_Use(nullptr, false);
		}
		return;
	}

	constexpr float TargetSphereRad = 16.f;
	float SphereRad = CartCollision->GetUnscaledSphereRadius();
	if (HasAuthority() && SphereRad < TargetSphereRad) {
		SphereRad += DeltaSeconds * 5.f;
		CartCollision->SetSphereRadius(SphereRad, false);

		if (SphereRad >= TargetSphereRad) {
			SphereRad = TargetSphereRad;
			CartCollision->SetSphereRadius(SphereRad);
		}
	}

	// Calculate the squared distance to the player
	const float DistanceSquaredToPlayer = (GetActorLocation() - LocalUserPtr->GetActorLocation()).SizeSquared();

	// If the player is moving and the squared distance to the player is above the squared threshold, record their position
	if (LocalUserPtr->GetVelocity().SizeSquared() > 0.01f && DistanceSquaredToPlayer > FMath::Square(DistanceThreshold)) {
		PastPositions.Insert(LocalUserPtr->GetActorLocation(), 0);
		if (PastPositions.Num() > DelaySteps) {
			PastPositions.RemoveAt(DelaySteps);
		}
	}

	// if the cart is being used and there's a position in the history, move to that position
	if (bInUse && PastPositions.Num() == DelaySteps && DistanceSquaredToPlayer > FMath::Square(DistanceThreshold)) {
		const FVector OldPosition = GetActorLocation();

		// Interpolate the cart's rotation to face the player
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LocalUserPtr->GetActorLocation());
		const FRotator YawOnlyRotation(0, LookAtRotation.Yaw, 0);
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), YawOnlyRotation, DeltaSeconds, RotationSpeed));

		// Required so Multicast does not crash on us.
		if (PastPositions.IsEmpty()) {
			return;
		}
		
		FVector TargetPosition = PastPositions.Last();

		FHitResult HitResult;
		bool bGroundTrace = GetWorld()->LineTraceSingleByChannel(HitResult, TargetPosition, TargetPosition - FVector(0, 0, 1000), ECC_Visibility);
		if (bGroundTrace) {
			TargetPosition.Z = HitResult.ImpactPoint.Z;
		}

		const FVector NewPosition = FMath::VInterpTo(OldPosition, TargetPosition, DeltaSeconds, InterpolationSpeed);
		SetActorLocation(NewPosition);

		// Interpolate the cart's position towards the target
		const float DistanceMoved = (NewPosition - OldPosition).Size();
		const float RotationAngle = (-DistanceMoved / WheelR->Bounds.SphereRadius) * 50.f; // Assuming the wheels are circular
		const FRotator WheelRotation(RotationAngle, 0, 0);

		WheelR->AddLocalRotation(WheelRotation);
		WheelL->AddLocalRotation(WheelRotation);
	}
}

void ACart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACart, bInUse);
	DOREPLIFETIME(ACart, WheelCount);
	DOREPLIFETIME(ACart, UserPtr);
}

void ACart::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("WheelCount", WheelCount);
}

void ACart::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	WheelCount = JsonObject->GetNumberField("WheelCount");
	OnRep_WheelCount();
}

FText ACart::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	FString Text = FString::Printf(TEXT("%s"), *ToolName.ToString());

	switch (WheelCount) {
		case 0: {
			Text += TEXT("\n<Sub>Requires Wheels.</>");
		} break;
		case 1: {
			Text += TEXT("\n<Sub>Requires a Wheel.</>");
		} break;
	}

	return Super::GetInformationText_Implementation();
}
