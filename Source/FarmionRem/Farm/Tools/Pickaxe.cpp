// Copyright 2020-2023: Jesse J. van Vliet


#include "Pickaxe.h"

APickaxe::APickaxe() {
	ToolName = "Pickaxe";
	
	PickaxeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickaxeMesh"));
	PickaxeMesh->SetupAttachment(RootComponent);
}

