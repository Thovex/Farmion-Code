// Copyright 2020-2023: Jesse J. van Vliet

#include "MagicBook.h"

#include "Components/WidgetComponent.h"
#include "FarmionRem/Callbacks.h"
#include "Net/UnrealNetwork.h"

AMagicBook::AMagicBook() {
	BookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BookMesh"));
	BookMesh->SetupAttachment(RootComponent);

	PageLeftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PageLeftMesh"));
	PageLeftMesh->SetupAttachment(BookMesh);

	PageRightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PageRightMesh"));
	PageRightMesh->SetupAttachment(BookMesh);

	PageInputWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("PageInputWidgetComp"));
	PageInputWidgetComp->SetupAttachment(BookMesh);
	PageInputWidgetComp->SetDrawSize(FVector2D(1080, 1920));
	PageInputWidgetComp->SetTwoSided(true);
	PageInputWidgetComp->SetBlendMode(EWidgetBlendMode::Transparent);

	PageOutputWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("PageOutputWidgetComp"));
	PageOutputWidgetComp->SetupAttachment(BookMesh);
	PageOutputWidgetComp->SetDrawSize(FVector2D(1080, 1920));
	PageOutputWidgetComp->SetTwoSided(true);
	PageOutputWidgetComp->SetBlendMode(EWidgetBlendMode::Transparent);
}

void AMagicBook::OnRep_CurrentIndex_Implementation() {
	
}

void AMagicBook::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMagicBook, CurrentIndex);
}

int32 AMagicBook::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	const auto* HitComponent = InteractionHit.GetComponent();

	if (HitComponent == PageLeftMesh) {
		return (int32)EInteractCallback::START_USING_OBJECT;
	}

	if (HitComponent == PageRightMesh) {
		return (int32)EInteractCallback::STOP_USING_OBJECT;
	}

	return Super::Interact(InteractInstigator, InteractionHit);
}

void AMagicBook::Server_Use(AFarmionCharacter* User,bool bUse) {
	if (bUse) {
		// Switch to the previous page
		if (CurrentIndex > 0) {
			CurrentIndex--;
			OnRep_CurrentIndex();
		}
	} else {
		// Switch to the next page
		if (CurrentIndex < Pages.Num() - 1) {
			CurrentIndex++;
			OnRep_CurrentIndex();
		}
	}
}