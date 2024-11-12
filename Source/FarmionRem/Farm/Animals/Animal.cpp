// Copyright 2020-2023: Jesse J. van Vliet


#include "Animal.h"

#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "FarmionRem/Farm/Storage/CarryableSocket.h"
#include "FarmionRem/Farm/Tools/IgnoreDecayVolume.h"
#include "FarmionRem/Interactables/Product.h"
#include "Net/UnrealNetwork.h"

AAnimal::AAnimal() {
	AnimalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AnimalMesh"));
	AnimalMesh->SetupAttachment(RootComponent);

	AnimalMesh->SetGenerateOverlapEvents(true);
	AnimalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AnimalMesh->SetCollisionProfileName(FName("BlockAllDynamic"));

	MouthTransform = CreateDefaultSubobject<USceneComponent>(TEXT("MouthTransform"));
	MouthTransform->SetupAttachment(AnimalMesh);
}

void AAnimal::SetNewTargetLocation(FVector NewTargetLocation) {
	if (bIsBeingCarriedEarly) {
		return;
	}

	CurrentTargetLocation = NewTargetLocation;
	StartTime = GetWorld()->GetTimeSeconds();
	StartLocation = GetActorLocation();

	const float Distance = FVector::Distance(GetActorLocation(), CurrentTargetLocation);
	Duration = Distance * DurationPerUnitDistance;

	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AAnimal::UpdateTransform, 0.016667f, true);

	TransitionState(GAnimal_Walk_Anim);
}

void AAnimal::TransitionState_Implementation(const FName AnimationName) {
	if (Animations.Contains(AnimationName)) {
		AnimalMesh->PlayAnimation(Animations[AnimationName].Sequence, Animations[AnimationName].bLooping);
	}
}

void AAnimal::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult) {
	if (Cast<AAnimal>(OtherActor)) {
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		TransitionState(GAnimal_Idle_Anim);
	}

	if (Cast<AProduct>(OtherActor)) {
		ProductFed = Cast<AProduct>(OtherActor)->GetClass();
		Multicast_Chomp();

		if (AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor)) {
			FTransform TargetTransform = MouthTransform->GetComponentTransform();
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

void AAnimal::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		TransitionState(GAnimal_Idle_Anim);
		AnimalMesh->OnComponentBeginOverlap.AddDynamic(this, &AAnimal::OnOverlapBegin);
	}
}

void AAnimal::UpdateTransform() {
	if (bIsBeingCarriedEarly) {
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		TransitionState(GAnimal_Idle_Anim);
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = CurrentTime - StartTime;

	if (ElapsedTime >= Duration) {
		TransitionState(GAnimal_Idle_Anim);
		SetActorLocation(CurrentTargetLocation);
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		return;
	}

	const float Alpha = ElapsedTime / Duration;
	const float CurveValue = MovementCurve->GetFloatValue(Alpha) * 100.f; // Arbitrary Number for big Speed.
	const FVector NewLocation = FMath::VInterpTo(StartLocation, CurrentTargetLocation, CurveValue, 1.0f);

	SetActorLocation(NewLocation);

	CurrentTimeInField += GetWorld()->GetDeltaSeconds();
	if (!bHarvestable && CurrentTimeInField > TimeInFieldTillHarvestable) {
		bHarvestable = true;
	}

	const FVector Direction = (CurrentTargetLocation - NewLocation).GetSafeNormal();
	if (Direction.SizeSquared() > 0.5f) {
		const FRotator TargetRotation = Direction.Rotation() + FRotator(0, -90, 0);
		const FRotator CurrentRotation = GetActorRotation();
		const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

		SetActorRotation(NewRotation);
	}

	constexpr float DistanceThreshold = 10.0f;
	constexpr float RotationThreshold = 5.0f;

	if (FVector::DistSquared(NewLocation, CurrentTargetLocation) <= FMath::Square(DistanceThreshold)) {
		const FRotator CurrentRotation = GetActorRotation();
		const FRotator TargetRotation = (CurrentTargetLocation - NewLocation).Rotation() + FRotator(0, -90, 0);

		if (CurrentRotation.Equals(TargetRotation, RotationThreshold)) {
			// Stop the movement and rotation when the animal has reached the destination
			TransitionState(GAnimal_Idle_Anim);
			GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		}
	}
}

void AAnimal::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetStringField("AnimalName", AnimalName.ToString());
	JsonObject->SetNumberField("CurrentTimeInField", CurrentTimeInField);
	JsonObject->SetNumberField("TimeInFieldTillHarvestable", TimeInFieldTillHarvestable);

	ISaveLoad::SaveClassPath(JsonObject, "ProductFed", ProductFed);
}

void AAnimal::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	AnimalName = FName(*JsonObject->GetStringField("AnimalName"));
	CurrentTimeInField = JsonObject->GetNumberField("CurrentTimeInField");
	TimeInFieldTillHarvestable = JsonObject->GetNumberField("TimeInFieldTillHarvestable");
	ProductFed = ISaveLoad::LoadClassPath(JsonObject, "ProductFed");
	bHarvestable = CurrentTimeInField >= TimeInFieldTillHarvestable;
}

bool AAnimal::IsBoilable_Implementation() const {
	return false;
}

bool AAnimal::IsFryable_Implementation() const {
	return true;
}

bool AAnimal::IsSpittable_Implementation() const {
	return true;
}

EDecayFlags AAnimal::GetDecayFlag() {
	return EDecayFlags::Animal;
}

void AAnimal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAnimal, bHarvestable);
	DOREPLIFETIME(AAnimal, ProductFed);
}

void AAnimal::Multicast_Chomp_Implementation() {
	Chomp_BP();
}

void AAnimal::PerformCycle() {
	if (ProductFed == nullptr || CarryableSocket == nullptr) {
		if (CarryableSocket) {
			CarryableSocket->SetSocketAvailability(this, CarryableSocketIndex, true);
			CarryableSocket = nullptr;
			CarryableSocketIndex = 0;
		}
		
		this->Destroy(true);
	}
}