// Copyright 2020-2023: Jesse J. van Vliet


#include "SellArea.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Interactables/Money.h"
#include "FarmionRem/Interfaces/Interactable/Sellable.h"

ASellArea::ASellArea() {
	SellArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SellArea"));
	SellArea->SetupAttachment(RootComponent);
}

void ASellArea::SellAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const TScriptInterface<ISellable> Sellable = TScriptInterface<ISellable>(OtherActor);

	if (Sellable && SellableClasses.Contains(OtherActor->GetClass())) {
		AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor);

		SpawnMoney(Sellable->GetFullSellPrice());
		FTransform TargetTransform = SellArea->GetComponentTransform();
		TargetTransform.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
		
		InteractableObject->Multicast_OneTimeMove({
			OtherActor->GetActorTransform(),
			TargetTransform,
		});
	}
}

void ASellArea::SpawnMoney(const int32& MonetaryValue) {
	const auto& SpawnPts = GetSpawnPoints();
	const FTransform& SpawnPt = SpawnPts[FMath::RandRange(0, SpawnPts.Num() - 1)];

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByChannel(Overlaps, SpawnPt.GetLocation(), SpawnPt.GetRotation(), ECC_WorldDynamic, FCollisionShape::MakeSphere(50.0f), QueryParams);

	for (auto& Overlap : Overlaps) {
		AMoney* OverlappingMoney = Cast<AMoney>(Overlap.GetActor());
		if (OverlappingMoney) {
			OverlappingMoney->Money += MonetaryValue;
			OverlappingMoney->OnRep_Money();
			return;
		}
	}

	AMoney* Money = GetWorld()->SpawnActorDeferred<AMoney>(MoneyClass, SpawnPt);
	Money->Money = MonetaryValue;
	Money->FinishSpawning(SpawnPt);
}

void ASellArea::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		SellArea->OnComponentBeginOverlap.AddDynamic(this, &ASellArea::SellAreaOverlap);
	}
}


