// Copyright 2020-2023: Jesse J. van Vliet

#include "SingleCropPlot.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Farm/Land/FarmLand.h"
#include "FarmionRem/Farm/Planting/CropPlant.h"
#include "FarmionRem/Farm/Upgrades/Beehive/Beehive.h"
#include "FarmionRem/Interactables/Seed.h"
#include "Net/UnrealNetwork.h"

ASingleCropPlot::ASingleCropPlot() {

	
	PlotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotMesh"));
	PlotMesh->SetupAttachment(RootComponent);

	PlotInput = CreateDefaultSubobject<UBoxComponent>(TEXT("PotInput"));
	PlotInput->SetupAttachment(PlotMesh);
	

	PlantTransform = CreateDefaultSubobject<USceneComponent>(TEXT("PlantTransform"));
	PlantTransform->SetupAttachment(PlotMesh);
}

void ASingleCropPlot::OnRep_Seed_Implementation() {}

void ASingleCropPlot::OverlapPlot(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (bBought) {
	
		if (OtherActor->IsA<ASeed>() && this->Seed == ESeed::None
			&& Cast<ASeed>(OtherActor)->SeedType >= StartAcceptedSeedsEnum
			&& Cast<ASeed>(OtherActor)->SeedType <= EndAcceptedSeedsEnum) {

			Server_AddSeed(Cast<ASeed>(OtherActor));
			OtherActor->Destroy(true);
			OnRep_Seed();
			return;
		}
	}
}

void ASingleCropPlot::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		PlotInput->OnComponentBeginOverlap.AddDynamic(this, &ASingleCropPlot::OverlapPlot);
	}
}

void ASingleCropPlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASingleCropPlot, Seed);
}

void ASingleCropPlot::PerformCycle() {
	for (auto It = FarmEntries.CreateIterator(); It; ++It) {
		auto& [Location, Rotation, Plant, Data, Row] = *It;

		Data.Day += 1;
		Plant->Days++;
		Plant->OnRep_Days();
		
		if (Plant->BeehivePtr) {
			Plant->HoneyDays++;
		}

		check(Row.DaysPerStage.Contains(ESeedStage::SeedStage));
		check(Row.DaysPerStage.Contains(ESeedStage::BlossomStage));
		check(Row.DaysPerStage.Contains(ESeedStage::HarvestStage));

		UE_LOG(LogTemp, Log, TEXT("[%s] Day: (%d) TotalDay: (%d)"), *Row.Id, Data.Day > *Row.DaysPerStage.Find(Plant->SeedStage) ? 1 : Data.Day, Row.GetTotalDays());

		if (Plant->BeehivePtr
			&& FMath::RandRange(0, 100) < GHoney_Skip_Progress_Rate
			&& Plant->SeedStage == ESeedStage::BlossomStage
			&& Data.Day < *Row.DaysPerStage.Find(Plant->SeedStage)
			&& (Plant->HoneyDays == Plant->Days || FMath::Abs(Plant->HoneyDays - Plant->Days) <= 1)) { 

			Data.Day = 1;

			UE_LOG(LogTemp, Warning, TEXT("Seed Progressed from [BlossomStage] -> [HarvestStage] - Due to the Honey Factor!"));
			Plant->SeedStage = ESeedStage::HarvestStage;
			Plant->OnRep_SeedStage();
			continue;
		}

		if (Data.Day > *Row.DaysPerStage.Find(Plant->SeedStage)) {
			Data.Day = 1;

			switch (Plant->SeedStage) {
				case ESeedStage::SeedStage: {
					UE_LOG(LogTemp, Warning, TEXT("Seed Progressed from [SeedStage] -> [BlossomStage]"));
					Plant->SeedStage = ESeedStage::BlossomStage;
					Plant->OnRep_SeedStage();
				} break;
				case ESeedStage::BlossomStage: {
					UE_LOG(LogTemp, Warning, TEXT("Seed Progressed from [BlossomStage] -> [HarvestStage]"));
					Plant->SeedStage = ESeedStage::HarvestStage;
					Plant->OnRep_SeedStage();
				} break;
				case ESeedStage::HarvestStage: {
					if (Plant->FarmLandPtr) {
						Plant->SeedStage = ESeedStage::DeathStage;
						Plant->OnRep_SeedStage();
							
						// FarmLandPtr needs to be Unset on Carry.
						UE_LOG(LogTemp, Warning, TEXT("Seed Progressed from [HarvestStage] -> [DeathStage], Good Day Sir!"));
						Plant->DestroyAllRemainingProducts();
					}
				} break;
				case ESeedStage::DeathStage: {
					Plant->Destroy(true);
					FarmEntries.RemoveAt(It.GetIndex());
					--It;
					continue;
				} break;
				default: {
					UE_LOG(LogTemp, Error, TEXT("Seed Progression in Invalid Stage!"));
					Plant->DestroyAllRemainingProducts();
					Plant->Destroy(true);
					FarmEntries.RemoveAt(It.GetIndex());
					--It;
					continue;
				}
			}
		}
	}
}

void ASingleCropPlot::Server_AddSeed_Implementation(const ASeed* InSeed) {
	this->Seed = InSeed->SeedType;

	FFarmEntry FarmEntry;
	FarmEntry.Location = PlantTransform->GetComponentLocation();
	FarmEntry.Rotation = PlantTransform->GetComponentRotation();

	TArray<FGrowthDataRow*> Rows;
	const FString ContextString(TEXT("MyContextString"));

	check(GrowthTable != nullptr);

	GrowthTable->GetAllRows<FGrowthDataRow>(ContextString, Rows);

	bool RowFound = false;
	for (const auto* Row : Rows) {
		if (Row->Id == InSeed->GrowthTableId) {
			RowFound = true;
			FarmEntry.Row = *Row;
			break;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Try Accessing Row: %s"), *InSeed->GrowthTableId);

	check(RowFound);

	FFarmData FarmData;
	FarmData.Day = 1;
	FarmData.TableId = InSeed->GrowthTableId;

	FarmEntry.Data = FarmData;

	FarmEntry.Plant = GetWorld()->SpawnActor<ACropPlant>(FarmEntry.Row.Plant, FarmEntry.Location, FarmEntry.Rotation);
	FarmEntry.Plant->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	
	FarmEntry.Plant->bCollisionToggle = true;
	FarmEntry.Plant->OnRep_CollisionToggle();

	for (const auto& KeyValue : FarmEntry.Row.DaysPerStage) {
		FarmEntry.Plant->TotalDays += KeyValue.Value;
	}
	
	FarmEntry.Plant->Days = 0;
	FarmEntry.Plant->OnRep_Days();

	FarmEntry.Plant->SeedStage = ESeedStage::SeedStage;
	FarmEntry.Plant->OnRep_SeedStage();

	//FarmEntry.Plant->bIsPlanted = true;
	FarmEntry.Plant->SingleCropPlotPtr = this;

	FarmEntries.Add(FarmEntry);
	UE_LOG(LogTemp, Warning, TEXT("Added Farm Entry with ID: %s"), *FarmEntry.Data.TableId);
}

void ASingleCropPlot::FinishPlant(const ACropPlant* Plant) {
	this->Seed = ESeed::None;
	this->OnRep_Seed();
	
	for (auto It = FarmEntries.CreateIterator(); It; ++It) {
		if (It->Plant == Plant) {
			UE_LOG(LogTemp, Warning, TEXT("Removed Entry from FarmEntries: %s"), *Plant->GetFullName());
			FarmEntries.RemoveAt(It.GetIndex());
			break;
		}
	}
}

TSoftObjectPtr<ACropPlant> ASingleCropPlot::GetPlant() {
	if (FarmEntries.IsEmpty()) {
		return nullptr;
	}

	return FarmEntries[0].Plant;
}

void ASingleCropPlot::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("Seed", static_cast<int32>(this->Seed));

	TArray<TSharedPtr<FJsonValue>> FarmEntriesArray;
	for (auto&& [Location, Rotation, Plant, Data, Row] : FarmEntries) {
		// Create a JSON object to store the farm entry data
		const TSharedPtr<FJsonObject> JsonFarmEntry = MakeShareable(new FJsonObject());
		if (Plant->bPendingKill) {
			continue;
		}

		// Set the JSON object's properties
		JsonFarmEntry->SetNumberField(TEXT("PositionX"), PlantTransform->GetComponentLocation().X);
		JsonFarmEntry->SetNumberField(TEXT("PositionY"), PlantTransform->GetComponentLocation().Y);
		JsonFarmEntry->SetNumberField(TEXT("PositionZ"), PlantTransform->GetComponentLocation().Z);

		JsonFarmEntry->SetNumberField(TEXT("RotationPitch"), Rotation.Pitch);
		JsonFarmEntry->SetNumberField(TEXT("RotationYaw"), Rotation.Yaw);
		JsonFarmEntry->SetNumberField(TEXT("RotationRoll"), Rotation.Roll);

		JsonFarmEntry->SetNumberField(TEXT("PlantSeedStage"), static_cast<int32>(Plant->SeedStage));
		JsonFarmEntry->SetNumberField(TEXT("PlantsAvailable"), Plant->Products.IsEmpty() ? -1 : Plant->Products.Num());
		JsonFarmEntry->SetBoolField(TEXT("Watered"), Plant->bWatered);
		JsonFarmEntry->SetNumberField(TEXT("HoneyDays"), Plant->HoneyDays);
		JsonFarmEntry->SetNumberField(TEXT("ProductsTaken"), Plant->ProductsTaken);
		JsonFarmEntry->SetNumberField(TEXT("DaysPassed"), Plant->Days);
		
		JsonFarmEntry->SetNumberField(TEXT("DataDay"), Data.Day);
		JsonFarmEntry->SetStringField(TEXT("DataTableId"), Data.TableId);


		// Add the JSON farm entry object to the JSON farm entries array
		TSharedPtr<FJsonValueObject> FarmEntryValue = MakeShareable(new FJsonValueObject(JsonFarmEntry));
		FarmEntriesArray.Add(FarmEntryValue);
	}

	JsonObject->SetArrayField(TEXT("FarmEntries"), FarmEntriesArray);
}

void ASingleCropPlot::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	FarmEntries.Empty();

	TArray<TSharedPtr<FJsonValue>> FarmEntriesArray = JsonObject->GetArrayField(TEXT("FarmEntries"));
	for (const auto& EntryValue : FarmEntriesArray) {
		const TSharedPtr<FJsonObject> JsonFarmEntry = EntryValue->AsObject();

		FFarmEntry FarmEntry;

		FVector Location;
		Location.X = JsonFarmEntry->GetNumberField(TEXT("PositionX"));
		Location.Y = JsonFarmEntry->GetNumberField(TEXT("PositionY"));
		Location.Z = JsonFarmEntry->GetNumberField(TEXT("PositionZ"));
		FarmEntry.Location = Location;

		FRotator Rotation;
		Rotation.Pitch = JsonFarmEntry->GetNumberField(TEXT("RotationPitch"));
		Rotation.Yaw = JsonFarmEntry->GetNumberField(TEXT("RotationYaw"));
		Rotation.Roll = JsonFarmEntry->GetNumberField(TEXT("RotationRoll"));
		FarmEntry.Rotation = Rotation;

		FFarmData FarmData;
		FarmData.Day = JsonFarmEntry->GetIntegerField(TEXT("DataDay"));
		FarmData.TableId = JsonFarmEntry->GetStringField(TEXT("DataTableId"));
		FarmEntry.Data = FarmData;

		const FString ContextString(TEXT("MyContextString"));
		FGrowthDataRow* GrowthDataRow = GrowthTable->FindRow<FGrowthDataRow>(*FarmData.TableId, ContextString);
		if (GrowthDataRow) {
			FarmEntry.Row = *GrowthDataRow;
		}

		const ESeedStage PlantSeedStage = static_cast<ESeedStage>(JsonFarmEntry->GetIntegerField(TEXT("PlantSeedStage")));

		FarmEntry.Plant = GetWorld()->SpawnActor<ACropPlant>(FarmEntry.Row.Plant, FarmEntry.Location, FarmEntry.Rotation);
		FarmEntry.Plant->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		FarmEntry.Plant->bCollisionToggle = true;
		FarmEntry.Plant->OnRep_CollisionToggle();
		
		FarmEntry.Plant->ProductsRemaining = JsonFarmEntry->GetIntegerField(TEXT("PlantsAvailable"));
		FarmEntry.Plant->ProductsTaken = JsonFarmEntry->GetIntegerField(TEXT("ProductsTaken"));
		FarmEntry.Plant->OnRep_ProductsTaken();

		for (const auto& KeyValue : FarmEntry.Row.DaysPerStage) {
			FarmEntry.Plant->TotalDays += KeyValue.Value;
		}
		FarmEntry.Plant->HoneyDays = JsonFarmEntry->GetIntegerField(TEXT("HoneyDays"));

		FarmEntry.Plant->SeedStage = PlantSeedStage;
		FarmEntry.Plant->OnRep_SeedStage();

		FarmEntry.Plant->Days = JsonFarmEntry->GetIntegerField(TEXT("DaysPassed"));
		FarmEntry.Plant->OnRep_Days();

		FarmEntry.Plant->bWatered = JsonFarmEntry->GetBoolField(TEXT("Watered"));
		FarmEntry.Plant->OnRep_Water();

		FarmEntry.Plant->SingleCropPlotPtr = this;

		FarmEntries.Add(FarmEntry);
	}


	Seed = static_cast<ESeed>(JsonObject->GetIntegerField("Seed"));


	OnRep_Seed();
}

FText ASingleCropPlot::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	FString Text = FString::Printf(TEXT("<Prod>%s</>\n"), *ToolName.ToString());


	if (Seed == ESeed::None) {
		Text += "<Sub>Requires Seed</>";
		return FText::FromString(Text);
	}

	if (FarmEntries.IsEmpty()) {
		return FText::FromString(Text);
	}

	Text += FString::Printf(TEXT("<Sub>%s</>"), *GetSeedStageDisplayName(FarmEntries[0].Plant->SeedStage));
	return FText::FromString(Text);
}