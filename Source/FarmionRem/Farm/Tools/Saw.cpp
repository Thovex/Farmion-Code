// Copyright 2020-2023: Jesse J. van Vliet


#include "Saw.h"

ASaw::ASaw() {
	ToolName = "Saw";

	SawMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SawMesh"));
	SawMesh->SetupAttachment(Transform);
}
