// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/Interfaces/Hover/Information.h"
#include "GameFramework/Actor.h"
#include "InfoScroll.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AInfoScroll : public AActor, public IInformation {
	GENERATED_BODY()

public:
	AInfoScroll();

};