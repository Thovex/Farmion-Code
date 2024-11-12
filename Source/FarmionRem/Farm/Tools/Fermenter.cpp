// Copyright 2020-2023: Jesse J. van Vliet


#include "Fermenter.h"

#include "Components/BoxComponent.h"

AFermenter::AFermenter() {
	FermenterInput = CreateDefaultSubobject<UBoxComponent>(TEXT("FermenterInput"));
	FermenterInput->SetupAttachment(RootComponent);
}

void AFermenter::BeginPlay() {
	Super::BeginPlay();
}
