// Copyright 2020-2023: Jesse J. van Vliet


#include "ForageBag.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Interactables/Product.h"
#include "Net/UnrealNetwork.h"

AForageBag::AForageBag() {
	BagPivot = CreateDefaultSubobject<USceneComponent>(TEXT("BagPivot"));
	BagPivot->SetupAttachment(RootComponent);
	
	BagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BagMesh"));
	BagMesh->SetupAttachment(BagPivot);

	BagInput = CreateDefaultSubobject<UBoxComponent>(TEXT("BagInput"));
	BagInput->SetupAttachment(BagMesh);
}

void AForageBag::OnRep_Timer_Implementation() { }

void AForageBag::OnRep_InputCount_Implementation() {}

void AForageBag::OnBagInputBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!bBought) {
		return;
	}
	
	if (OtherActor->IsA<AProduct>()) {
		const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		const TScriptInterface<ISaveLoad> SaveLoad = TScriptInterface<ISaveLoad>(OtherActor);

		SaveLoad->Save(JsonObject);
		Products.Add(JsonObject);

		InputCount = Products.Num();
		OnRep_InputCount();

		OtherActor->Destroy(true);

		if (!GetWorld()->GetTimerManager().IsTimerActive(TimeTillPopHandle)) {
			GetWorld()->GetTimerManager().SetTimer(TimerUpdaterHandle, this, &AForageBag::UpdateTimer, 1.f, true);
			GetWorld()->GetTimerManager().SetTimer(TimeTillPopHandle, this, &AForageBag::TryExtract, TimeTillPop, false);
		}
	}
}

void AForageBag::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		BagInput->OnComponentBeginOverlap.AddDynamic(this, &AForageBag::OnBagInputBeginOverlap);
	}

	Timer = TimeTillPop;
	OnRep_Timer();
	OnRep_InputCount();
}

void AForageBag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AForageBag, InputCount);
	DOREPLIFETIME(AForageBag, Timer);
}

void AForageBag::Extract(TArray<FVector> SpawnLocations) {
	UWorld* World = GetWorld();

	// Sucks, but better than hitting random ensure?
	auto Copy = Products;
	
	for (const TSharedPtr<FJsonObject>& Product : Copy) {
		int32 RefCount = Product.GetSharedReferenceCount();
		UE_LOG(LogTemp, Warning, TEXT("Product has %d shared references"), RefCount);

		UClass* LoadedClass = LoadObject<UClass>(nullptr, *Product->GetStringField("Class"));

		if (LoadedClass && World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* NewObject = nullptr;
			int32 RetryCount = 0;

			while (NewObject == nullptr && RetryCount < 100) {
				NewObject = World->SpawnActor<AActor>(LoadedClass, SpawnLocations[FMath::RandRange(0, SpawnLocations.Num() - 1)], GetActorRotation(), SpawnParams);
				RetryCount++;
			}

			if (NewObject) {
				const TScriptInterface<ISaveLoad> NewObjectInterface(NewObject);
				if (NewObjectInterface) {
					NewObjectInterface->Load(Product);
				}
				NewObject->SetReplicates(true);
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Could not spawn class: %s"), *Product->GetStringField("Class"));
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Could not load class: %s"), *Product->GetStringField("Class"));
		}
	}

	Products.Empty();

	if (!bExplosive) {

		InputCount = 0;
		OnRep_InputCount();

		Timer = TimeTillPop;
		OnRep_Timer();
	} else {
		if (ConsumeVfx) {
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* VfxActor = GetWorld()->SpawnActor<AActor>(ConsumeVfx, this->GetActorLocation(), this->GetActorRotation(), SpawnParameters);
			VfxActor->SetLifeSpan(5.0f);
		}

		this->Destroy(true);
	}
}

void AForageBag::TryExtract() {
	UE_LOG(LogTemp, Warning, TEXT("Amount in Bag: %d"), Products.Num());
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	StartExtractionProcess();
}

void AForageBag::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (const TSharedPtr<FJsonObject>& Product : Products) {
		JsonArray.Add(MakeShareable(new FJsonValueObject(Product)));
	}

	JsonObject->SetArrayField(TEXT("Products"), JsonArray);
	JsonObject->SetNumberField(TEXT("TimeRemaining"), GetWorld()->GetTimerManager().GetTimerRemaining(TimeTillPopHandle));
}

void AForageBag::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	const TArray<TSharedPtr<FJsonValue>>* JsonArray;
	if (JsonObject->TryGetArrayField(TEXT("Products"), JsonArray)) {
		for (const TSharedPtr<FJsonValue>& JsonValue : *JsonArray) {
			if (JsonValue.IsValid() && JsonValue->Type == EJson::Object) {
				const TSharedPtr<FJsonObject>& ProductObj = JsonValue->AsObject();
				if (ProductObj.IsValid()) {
					Products.Add(ProductObj);
				}
			}
		}
	}

	if (!Products.IsEmpty()) {
		float TimeRemaining = JsonObject->GetNumberField(TEXT("TimeRemaining"));
		GetWorld()->GetTimerManager().SetTimer(TimerUpdaterHandle, this, &AForageBag::UpdateTimer, 1.f, true);
		GetWorld()->GetTimerManager().SetTimer(TimeTillPopHandle, this, &AForageBag::TryExtract, TimeRemaining, false);
	}

	InputCount = Products.Num();
	OnRep_InputCount();
}

void AForageBag::UpdateTimer() {
	Timer = FMath::CeilToInt(GetWorldTimerManager().GetTimerRemaining(TimeTillPopHandle));
	OnRep_Timer();
}

FText AForageBag::GetInformationText_Implementation() const {
	FString Text = ToolName.ToString();
	if (bExplosive) {
		Text += FString(TEXT("\n<Sub>Explosive</>"));
	}

	return FText::FromString(Text);
}