// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "InfoElement.h"
#include "Tutorial.generated.h"

UCLASS(Blueprintable)
class FARMIONREM_API ATutorial : public AInfoElement {
	GENERATED_BODY()

public:
	ATutorial();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SegmentIndex)
	int32 SegmentIndex = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_SegmentIndex();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InfoText;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	bool PassesRequirements();

protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;

	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FText GetInformationText_Implementation() const override;
	virtual FText GetMiniInformationText_Implementation() const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
};