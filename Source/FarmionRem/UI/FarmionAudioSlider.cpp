// Copyright 2020-2023: Jesse J. van Vliet


#include "FarmionAudioSlider.h"

UFarmionAudioSlider::UFarmionAudioSlider() {
	
	OutputRange = FVector2D(0, 100);
}

void UFarmionAudioSlider::SynchronizeProperties() {
	Super::SynchronizeProperties();

	OutputRange = FVector2D(0, 100);
}