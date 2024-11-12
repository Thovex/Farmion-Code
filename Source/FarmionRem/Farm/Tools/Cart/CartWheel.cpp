// Copyright 2020-2023: Jesse J. van Vliet

#include "CartWheel.h"

ACartWheel::ACartWheel() {
	WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMesh"));
	WheelMesh->SetupAttachment(RootComponent);
}