// Copyright 2020-2023: Jesse J. van Vliet

#include "Mushroom.h"

AMushroom::AMushroom() {
	MushroomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MushroomMesh"));
	MushroomMesh->SetupAttachment(RootComponent);
}