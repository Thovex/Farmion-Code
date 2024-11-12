// Copyright 2020-2023: Jesse J. van Vliet

#include "CropPlant.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Farm/Land/FarmLand.h"
#include "FarmionRem/Farm/Planting/Seeds/InteractableProductSeed.h"
#include "FarmionRem/Farm/Upgrades/Beehive/Beehive.h"
#include "FarmionRem/Interactables/Seed.h"
#include "FarmionRem/Interfaces/Interactable/Quality.h"

#include "SingleCropPlot.h"
#include "Net/UnrealNetwork.h"

ACropPlant::ACropPlant() {
	bReplicates = true;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;
	
	WaterCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterCollider"));
	WaterCollider->SetupAttachment(RootComponent);

	SoilMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SoilMesh"));
	SoilMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	SoilMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	SoilMesh->SetupAttachment(RootComponent);
}

void ACropPlant::OnRep_ProductsTaken_Implementation() {}

void ACropPlant::BeginPlay() {
	Super::BeginPlay();

	OnRep_BeeAffected();
	OnRep_Water();
}


void ACropPlant::OnRep_Water_Implementation() {
	for (int32 i = 0; i < SoilMesh->GetNumMaterials(); i++) {
		SoilMesh->SetMaterial(i, bWatered ? WetSoilMaterial : SoilMaterial);
	}
}

AInteractableProductSeed* ACropPlant::CreateProduct(USceneComponent* TransformComp) {
	UE_LOG(LogTemp, Log, TEXT("Creating Product!"));

	GAME_STATE

	EItemQuality ParentQuality = EItemQuality::Adept;
	uint32 QualitySum = 0;
	uint32 QualityCount = 0;

	if (this->SingleCropPlotPtr) {
		QualitySum += static_cast<uint32>(this->SingleCropPlotPtr->Quality);
		QualityCount++;
	}

	if (this->BeehivePtr) {
		QualitySum += static_cast<uint32>(this->BeehivePtr->Quality);
		QualityCount++;
	}

	if (QualityCount > 0) {
		ParentQuality = static_cast<EItemQuality>(QualitySum / QualityCount);
	}
	
	const float Modifier = GET_DEFAULT_QUALITY_MODIFIER(ParentQuality);
	
	// If not loaded, no idea how much products are remaining, that's up to the blueprint.
	if (ProductsRemaining == -1) {
		checkf(Product != nullptr, TEXT("Product is nullptr! %s"), *GetFullName());
		AInteractableProductSeed* Seed = GetWorld()->SpawnActor<AInteractableProductSeed>(Product);
		Seed->AttachToComponent(TransformComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Seed->SetActorRelativeRotation(Seed->RotationAsSeed);
		Seed->Tags.Add(GProtected_Tag_Name);

		Seed->Quality = Seed->DetermineQualityWithModifier(GameState->CalculateFarmLevel(), Modifier);
		Seed->bPicked = false;
		Seed->OnRep_Picked();

		Seed->CropPtr = this;
		Products.Add(Seed);

		UE_LOG(LogTemp, Log, TEXT("Products Size: %d, Remaining: %d"),  Products.Num(), ProductsRemaining);
		return Seed;
	}

	if (ProductsRemaining > 0) {
		AInteractableProductSeed* Seed = GetWorld()->SpawnActor<AInteractableProductSeed>(Product);
		Seed->AttachToComponent(TransformComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Seed->SetActorRelativeRotation(Seed->RotationAsSeed);
		Seed->Tags.Add(GProtected_Tag_Name);

		Seed->Quality = Seed->DetermineQualityWithModifier(GameState->CalculateFarmLevel(), Modifier);
		Seed->bPicked = false;
		Seed->OnRep_Picked();

		Seed->CropPtr = this;
		Products.Add(Seed);
		--ProductsRemaining;

		UE_LOG(LogTemp, Log, TEXT("Products Size: %d, Remaining: %d"),  Products.Num(), ProductsRemaining);
		return Seed;
	}

	return nullptr;
}

void ACropPlant::Attack(const int32 Damage) {
	TotalHealth -= Damage;

	if (TotalHealth <= 0) {
		if (this->FarmLandPtr) {
			this->FarmLandPtr->FinishPlant(this);
			this->FarmLandPtr = nullptr;
		}

		if (this->SingleCropPlotPtr) {
			this->SingleCropPlotPtr->FinishPlant(this);
			this->SingleCropPlotPtr = nullptr;
		}

		this->DestroyAllRemainingProducts();
		this->Destroy(true);
	}
}

void ACropPlant::DestroyAllRemainingProducts() {
	for (auto It = Products.CreateIterator(); It; ++It) {
		UE_LOG(LogTemp, Warning, TEXT("Removed Entry from Plant Products: %s"), *(*It)->GetFullName());
		(*It)->Destroy();
	}
	Products.Empty();
}

void ACropPlant::PickupProduct_Implementation(const AInteractableProductSeed* Seed) {
	for (auto It = Products.CreateIterator(); It; ++It) {
		if (*It == Seed) {
			UE_LOG(LogTemp, Warning, TEXT("Removed Entry from Plant Products: %s"), *Seed->GetFullName());
			Products.RemoveAt(It.GetIndex());
			++ProductsTaken;
			OnRep_ProductsTaken();
			break;
		}
	}

	if (Products.IsEmpty()) {
		if (FarmLandPtr) {
			FarmLandPtr->FinishPlant(this);
		}

		if (SingleCropPlotPtr) {
			SingleCropPlotPtr->FinishPlant(this);
		}
		Multicast_KillPlant();
	}
}

void ACropPlant::Multicast_KillPlant_Implementation() {
	bPendingKill = true;

	OnKillPlant();
	this->SetLifeSpan(GDefault_Lifespan_Check_Value * 7);
}

void ACropPlant::OnRep_BeeAffected_Implementation() {}

void ACropPlant::OnRep_SeedStage() {
	switch (SeedStage) {
		case ESeedStage::SeedStage: {
			OnSeedStage();
		} break;
		case ESeedStage::BlossomStage: {
			OnBlossomStage();
		} break;
		case ESeedStage::HarvestStage: {
			OnHarvestStage();
		} break;
		case ESeedStage::DeathStage: {
			OnDeathStage();
		} break;
		default: {
			UE_LOG(LogTemp, Error, TEXT("Invalid Stage in OnRep_SeedStage: %s"), *GetSeedStageDisplayName(SeedStage));
		} break;
	}
}

void ACropPlant::OnRep_Days_Implementation() {}

void ACropPlant::OnRep_CollisionToggle() {
	if (bCollisionToggle) {
		TArray<UPrimitiveComponent*> PrimitiveComponents;
		this->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (auto&& PrimitiveComponent : PrimitiveComponents) {
			PrimitiveComponent->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
			PrimitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
			PrimitiveComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		}
	}
}

void ACropPlant::SetActivesByStage(const ESeedStage InStage,
   USceneComponent* SeedParent,
   USceneComponent* BlossomParent,
   USceneComponent* DeathParent
) {
	auto SetActiveState = [](USceneComponent* Root, const bool Visibility) -> void {
		UE_LOG(LogTemp, Log, TEXT("SetActiveState: %d"), Visibility);
		Root->SetVisibility(Visibility, true);

		TArray<USceneComponent*> Children;
		Root->GetChildrenComponents(true, Children);
		
		for (auto It = Children.CreateIterator(); It; ++It) {
			(*It)->SetHiddenInGame(!Visibility);
			UE_LOG(LogTemp, Log, TEXT("Visibility: %s, SetActiveState: %s"), Visibility ? TEXT("True") : TEXT("False"), *(*It)->GetFullName());
		}
	};
	
	switch (InStage) {
		case ESeedStage::None: {} break;
		case ESeedStage::SeedStage: {
			SetActiveState(SeedParent, true);
		} break;
		case ESeedStage::BlossomStage: {
			SetActiveState(SeedParent, false);
			SetActiveState(BlossomParent, true);
		} break;
		case ESeedStage::HarvestStage: {
			SetActiveState(BlossomParent, false);
			// Should be handled by the FarmLand/SingleCropPlot
		} break;
		case ESeedStage::DeathStage:
		{
			SetActiveState(DeathParent, true);
		}
	}
}

void ACropPlant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACropPlant, bWatered);
	DOREPLIFETIME(ACropPlant, Days);
	DOREPLIFETIME(ACropPlant, ProductsTaken);
	DOREPLIFETIME(ACropPlant, SeedStage);
	DOREPLIFETIME(ACropPlant, TotalHealth);
	DOREPLIFETIME(ACropPlant, bBeeAffected);
	DOREPLIFETIME(ACropPlant, bCollisionToggle);
}

void ACropPlant::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	// Intentionally Do Nothing. We will create ACropPlants from AFarmLands/ASingleCropPlots
}

void ACropPlant::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	// Intentionally Do Nothing. We will create ACropPlants from AFarmLands/ASingleCropPlots
}

FText ACropPlant::GetInformationText_Implementation() const {
	FString Text = "Crop";

	if (SeedStage == ESeedStage::DeathStage) {
		Text += FString::Printf(TEXT("\n<Sub>Not-Reapth-Able</>"));
		return FText::FromString(Text);
	}
	
	Text += FString::Printf(TEXT("\n<Sub>%hs</>"), TotalHealth <= GLow_Health_Crop ? "Sickly" : "Healthy");
	return FText::FromString(Text);
}

bool ACropPlant::Supply(const TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) {
	UE_LOG(LogTemp, Log, TEXT("Supplying Crop with Liquid: %s"), *GetLiquidDisplayName(Liquid));

	if (Liquid == ELiquids_Compost) {
		bWatered = true;
		OnRep_Water();

		this->SeedStage = ESeedStage::HarvestStage;
		this->OnRep_SeedStage();
		return true;
	}
	
	if (Liquid != ELiquids::ELiquids_Water) {
		return false;
	}
	
	bWatered = true;
	OnRep_Water();
	return true;
}