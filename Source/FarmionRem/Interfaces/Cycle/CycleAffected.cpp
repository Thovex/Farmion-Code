// Copyright 2020-2023: Jesse J. van Vliet

#include "CycleAffected.h"

#include "FarmionRem/GlobalValues.h"
#include "FarmionRem/Farm/Tools/IgnoreDecayVolume.h"

EDecayFlags ICycleAffected::GetDecayFlag() {
	return EDecayFlags::Other;
}

void ICycleAffected::PerformCycle() {
	
}

void ICycleAffected::PostPerformCycle() {
	
}

bool ICycleAffected::IsProtected() const {
	const AActor* ThisAsActor = Cast<AActor>(this->_getUObject());
	return ThisAsActor->Tags.Contains(GProtected_Tag_Name);
}

void ICycleAffected::TrySetProtect(EDecayFlags InFlags, const bool Enabled) {
	if (static_cast<uint8>(InFlags) & static_cast<uint8>(GetDecayFlag())) {
		AActor* ThisAsActor = Cast<AActor>(this->_getUObject());
		if (Enabled && !ThisAsActor->Tags.Contains(GProtected_Tag_Name)) {
			UE_LOG(LogTemp, Log, TEXT("Protecting: %s"), *this->_getUObject()->GetName());
			ThisAsActor->Tags.Add(GProtected_Tag_Name);
			return;
		}

		if (!Enabled && ThisAsActor->Tags.Contains(GProtected_Tag_Name)) {
			ThisAsActor->Tags.Remove(GProtected_Tag_Name);
			UE_LOG(LogTemp, Log, TEXT("Stopped Protecting: %s"), *this->_getUObject()->GetName());
			return;
		}
	}
}

