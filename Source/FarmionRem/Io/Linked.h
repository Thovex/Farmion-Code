// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "UObject/Interface.h"
#include "Linked.generated.h"

class ULinkerComponent;

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class ULinked : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ILinked {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void BindEvent(ULinkerComponent* Linker);
	
	UFUNCTION()
	virtual void Link();
};