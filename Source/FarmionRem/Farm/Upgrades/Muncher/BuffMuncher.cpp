// Copyright 2020-2023: Jesse J. van Vliet


#include "BuffMuncher.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Farm/Buffs.h"
#include "FarmionRem/Interfaces/Interactable/Ingredient.h"

ABuffMuncher::ABuffMuncher() {
	MunchInput = CreateDefaultSubobject<UBoxComponent>(TEXT("MunchInput"));
	MunchInput->SetupAttachment(RootComponent);

	Mouth = CreateDefaultSubobject<USceneComponent>(TEXT("Mouth"));
	Mouth->SetupAttachment(RootComponent);
}

void ABuffMuncher::Multicast_Chomp_Implementation() {
	Chomp_BP();
}

void ABuffMuncher::OnMunchInputOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (const TScriptInterface<IIngredient> Ingredient = TScriptInterface<IIngredient>(OtherActor)) {

		GAME_STATE
		FBuff GetBuff = Ingredient->Execute_GetBuff(Ingredient->_getUObject());

		if (GetBuff.BuffName == NAME_None) {
			UE_LOG(LogTemp, Warning, TEXT("No buff found for %s"), *Ingredient.GetObject()->GetName());
		} else {
			GameState->AddBuff(GetBuff);
		}
		
		if (AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor)) {
			FTransform TargetTransform = Mouth->GetComponentTransform();
			TargetTransform.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
			InteractableObject->Multicast_OneTimeMove({
				OtherActor->GetActorTransform(),
				TargetTransform,
			});
		} else {
			OtherActor->Destroy(true);
		}

		Multicast_Chomp();
		
		UE_LOG(LogTemp, Warning, TEXT("Munching %s"), *Ingredient.GetObject()->GetName());
	}
}

void ABuffMuncher::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		MunchInput->OnComponentBeginOverlap.AddDynamic(this, &ABuffMuncher::OnMunchInputOverlapBegin);
	}
}

FText ABuffMuncher::GetInformationText_Implementation() const {
	return FText::FromString(TEXT("Sir Thoveks is hungry!\n<Sub>Feed him ingredients to get buffs!</>"));
}

FText ABuffMuncher::GetMiniInformationText_Implementation() const {
	return FText::FromString(TEXT("Sir Thoveks, the Muncher!"));
}