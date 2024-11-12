// Copyright 2020-2023: Jesse J. van Vliet

#include "Fuel.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Interfaces/Fuel/FuelTarget.h"

AFuel::AFuel() {
	FuelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuelMesh"));
	FuelMesh->SetupAttachment(RootComponent);

	FuelCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FuelCollision"));
	FuelCollision->SetupAttachment(FuelMesh);
}

int32 AFuel::GetFuelValue() {
	return FuelValue;
}

void AFuel::BeginPlay() {
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (HasAuthority()) {
		UE_LOG(LogTemp, Log, TEXT("[Collision] Bound Handling Fuel Insertion"));
		FuelCollision->OnComponentBeginOverlap.AddDynamic(this, &AFuel::BeginFuelCollision);
	}
}

void AFuel::BeginFuelCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult) {
	const TScriptInterface<IFuelTarget> FuelTarget = TScriptInterface<IFuelTarget>(OtherActor);

	if (FuelTarget) {
		FuelTarget->Supply(GetFuelValue());

		if (AInteractableObject* InteractableObject = Cast<AInteractableObject>(this)) {
			FTransform TargetTransform = OtherActor->GetActorTransform();
			TargetTransform.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
			InteractableObject->Multicast_OneTimeMove({
				this->GetActorTransform(),
				TargetTransform,
			});
		} else {
			this->Destroy(true);
		}
	}
}
