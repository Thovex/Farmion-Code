// Copyright 2020-2023: Jesse J. van Vliet

#include "WoodcuttingAxe.h"

AWoodcuttingAxe::AWoodcuttingAxe() {
	ToolName = "Woodcutting Axe";
	
	AxeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AxeMesh"));
	AxeMesh->SetupAttachment(RootComponent);
}

