// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interactables/Product.h"
#include "Mushroom.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AMushroom : public AProduct {
	GENERATED_BODY()

public:
	AMushroom();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MushroomMesh;

};