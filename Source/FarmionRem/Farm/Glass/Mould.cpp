// Copyright 2020-2023: Jesse J. van Vliet


#include "Mould.h"

#include "Sand.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

AMould::AMould() {
	MouldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouldMesh"));
	MouldMesh->SetupAttachment(RootComponent);

	MouldInput = CreateDefaultSubobject<UBoxComponent>(TEXT("MouldInput"));
	MouldInput->SetupAttachment(MouldMesh);
}

void AMould::OnRep_Filled_Implementation() {
	
}

void AMould::UseMould() {
	bFilled = false;
	OnRep_Filled();
}

void AMould::OverlapMould(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult) {
	if (bBought) {
		if (OtherActor->IsA<ASand>() && !bFilled) {
			OtherActor->Destroy(true);
			bFilled = true;
			OnRep_Filled();
		}
	}
}

void AMould::BeginPlay() {
	Super::BeginPlay();

	OnRep_Filled();

	if (HasAuthority()) {
		MouldInput->OnComponentBeginOverlap.AddDynamic(this, &AMould::OverlapMould);
	}
}

void AMould::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMould, bFilled);
}
