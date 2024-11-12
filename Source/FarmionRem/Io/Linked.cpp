// Copyright 2020-2023: Jesse J. van Vliet


#include "Linked.h"

#include "FarmionRem/Interactables/LinkerComponent.h"

void ILinked::BindEvent(ULinkerComponent* Linker) {
	Linker->OnLink.AddDynamic(this, &ILinked::Link);
}

void ILinked::Link() {
	UE_LOG(LogTemp, Error, TEXT("ILinked::Link() - Not Implemented for %s"), *this->_getUObject()->GetName());
}