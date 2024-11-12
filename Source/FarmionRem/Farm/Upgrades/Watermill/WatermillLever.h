#pragma once

#include "CoreMinimal.h"
#include "FarmionRem/Interactables/Tool.h"
#include "WatermillLever.generated.h"

class AWatermill;

UCLASS(Blueprintable)
class FARMIONREM_API AWatermillLever : public AInteractableObject
{
	GENERATED_BODY()

public:
	AWatermillLever();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULinkerComponent* Linker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Pulled)
	bool bPulled = false;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnRep_Pulled();

protected:
	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character) override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual ELoadMethod LoadMethod() override;

	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;

	virtual FText GetInformationText_Implementation() const override;

	virtual void Link() override;

private:
	TSoftObjectPtr<AWatermill> Watermill;
};