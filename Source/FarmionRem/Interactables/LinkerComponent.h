// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Components/ActorComponent.h"
#include "LinkerComponent.generated.h"

class AInteractableObject;
#if WITH_EDITORONLY_DATA
class UDebugEditorLineCompToMultiActors;
#endif

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLinkerDelegate);

USTRUCT(BlueprintType)
struct FLinkerData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Actor = nullptr;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FARMIONREM_API ULinkerComponent : public UActorComponent {
	GENERATED_BODY()

public:
	ULinkerComponent();

	UPROPERTY(ReplicatedUsing=OnRep_Links, EditAnywhere, BlueprintReadWrite)
	TArray<FLinkerData> Links;

	UFUNCTION()
	void OnRep_Links();

	UPROPERTY()
	FLinkerDelegate OnLink;

	UFUNCTION(BlueprintCallable)
	AActor* GetLinkedActor(FName Name) const;

protected:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	UPROPERTY()
	UDebugEditorLineCompToMultiActors* DebugLineComponent;
#endif

};

