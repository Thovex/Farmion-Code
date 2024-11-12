// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "Press.h"
#include "JumpPress.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API AJumpPress : public APress {
	GENERATED_BODY()

public:
	AJumpPress();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* PressArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PowerPerJump = 10.f;

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	TArray<AFarmionCharacter*> CharactersInPressArea;
};