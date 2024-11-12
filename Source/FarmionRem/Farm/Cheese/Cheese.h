// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "CheeseAreaVolume.h"
#include "FarmionRem/Interactables/Product.h"
#include "Cheese.generated.h"

UENUM(BlueprintType)
enum ECheeseType {
	ECheeseType_None,
	ECheeseType_Cow UMETA(DisplayName = "Cow"),
	ECheeseType_Sheep UMETA(DisplayName = "Sheep"),
	ECheeseType_Goat UMETA(DisplayName = "Goat"),
};

static FORCEINLINE FString GetCheeseTypeDisplayName(ECheeseType CheeseType) {
	switch (CheeseType) {
	case ECheeseType::ECheeseType_None:
		return "None";
	case ECheeseType::ECheeseType_Cow:
		return "Cow";
	case ECheeseType::ECheeseType_Sheep:
		return "Sheep";
	case ECheeseType::ECheeseType_Goat:
		return "Goat";
	default:
		return "Unknown";
	}
}


USTRUCT(BlueprintType)
struct FCheeseVariant {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PreviousName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECheeseAreaType AreaType = ECheeseAreaType::ECheeseAreaType_Outside;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredCycles = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseValue = 0;
};

UCLASS(Blueprintable)
class FARMIONREM_API ACheese : public AProduct {
	GENERATED_BODY()

	ACheese();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheese Variants")
	TArray<FCheeseVariant> CheeseVariants = {{FName("Generic Gouda"), FName(NAME_None), nullptr, ECheeseAreaType::ECheeseAreaType_Outside, 1, 3}, {FName("Cow Curd"), FName(NAME_None), nullptr, ECheeseAreaType::ECheeseAreaType_Inside, 1, 5}, {FName("Mystical Cheddaron"), FName("Cow Curd"), nullptr, ECheeseAreaType::ECheeseAreaType_Outside, 3, 10}, {FName("Enchanted Bluemyst"), FName("Cow Curd"), nullptr, ECheeseAreaType::ECheeseAreaType_Inside, 4, 12}, {FName("Arcane Brie of Aether"), FName("Cow Curd"), nullptr, ECheeseAreaType::ECheeseAreaType_Cellar, 5, 20}, {FName("Eldritch Camembert Whispers"), FName("Mystical Cheddaron"), nullptr, ECheeseAreaType::ECheeseAreaType_Inside, 2, 25}, {FName("Gouda of the Stars"), FName("Enchanted Bluemyst"), nullptr, ECheeseAreaType::ECheeseAreaType_Cellar, 3, 30}, {FName("Fabled Feta of Fortune"), FName("Arcane Brie of Aether"), nullptr, ECheeseAreaType::ECheeseAreaType_Outside, 2, 45}, {FName("Havarti of the Moonlit Glade"), FName("Eldritch Camembert Whispers"), nullptr, ECheeseAreaType::ECheeseAreaType_Outside, 3, 50}, {FName("Provolone of the Celestial Seas"), FName("Gouda of the Stars"), nullptr, ECheeseAreaType::ECheeseAreaType_Inside, 2, 60}, {FName("Roquefort of the Timeless Caverns"), FName("Fabled Feta of Fortune"), nullptr, ECheeseAreaType::ECheeseAreaType_Cellar, 4, 30}, {FName("Mozzarella of the Dancing Shadows"), FName("Havarti of the Moonlit Glade"), nullptr, ECheeseAreaType::ECheeseAreaType_Inside, 3, 100}, {FName("Fontina of the Phoenix's Flame"), FName("Provolone of the Celestial Seas"), nullptr, ECheeseAreaType::ECheeseAreaType_Outside, 4, 115}, {FName("Comte of the Elemental Harmony"), FName("Roquefort of the Timeless Caverns"), nullptr, ECheeseAreaType::ECheeseAreaType_Cellar, 2, 165},};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CheeseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECheeseType> CheeseType;

	UPROPERTY(ReplicatedUsing=OnRep_CheeseName, EditAnywhere, BlueprintReadOnly, Category = "Cheese State")
	FName CurrentCheeseName;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRep_CheeseName();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Cheese State")
	int32 CurrentCheeseAge = 0;

	UFUNCTION()
	void UpdateAge(const EItemQuality RackQuality, ECheeseAreaType CurrentAreaType);

protected:
	virtual void BeginPlay() override;

	virtual void Save(const TSharedPtr<FJsonObject>& JsonObject) override;
	virtual void Load(const TSharedPtr<FJsonObject>& JsonObject) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FText GetInformationText_Implementation() const override;

private:
	ECheeseAreaType LastAreaType;
};