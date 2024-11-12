// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "AudioSlider.h"
#include "FarmionAudioSlider.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FARMIONREM_API UFarmionAudioSlider : public UAudioFrequencySlider {
	GENERATED_BODY()

public:
	UFarmionAudioSlider();
	
protected:
	virtual void SynchronizeProperties() override;
	

	
};
