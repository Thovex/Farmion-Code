// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "Converter.h"
#include "Components/WidgetComponent.h"
#include "MagicBook.generated.h"

USTRUCT(BlueprintType)
struct FMagicBookPage {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InputText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> InputImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText OutputText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> OutputImage;
};

UCLASS(Blueprintable)
class FARMIONREM_API AMagicBook : public AConverter {
	GENERATED_BODY()

public:
	AMagicBook();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BookMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PageRightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PageLeftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* PageInputWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* PageOutputWidgetComp;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentIndex, EditAnywhere, BlueprintReadWrite)
	int32 CurrentIndex;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_CurrentIndex();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMagicBookPage> Pages;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual int32 Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) override;

	virtual void Server_Use(AFarmionCharacter* User, bool bUse) override;
};