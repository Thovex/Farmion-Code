// Copyright 2020-2023: Jesse J. van Vliet


#include "SelectableFurniture.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"

ASelectableFurniture::ASelectableFurniture() {
	PrimaryActorTick.bCanEverTick = false;
	
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Transform);

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(Collision);

	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NetPriority = 0.1f;
}

void ASelectableFurniture::OnRep_SetIndex() {
	GAME_STATE
	
	if (FurniturePtr.IsValid()) {
		FurniturePtr->SetActorEnableCollision(false);
		FurniturePtr->SetLifeSpan(1.0f);

		CutePopOut(IndexToEase.Contains(PreviousIndex) ? IndexToEase[PreviousIndex] : EFurniEasing::Default, FurniturePtr.Get());
		FurniturePtr.Reset(); // Reset the pointer if the furniture is valid regardless of Index
	}

	if (Index == -1) {
		if (GameState->bRenovateHouseMode) {
			DisplayMesh->SetHiddenInGame(false);
		}
		return;
	}

	if (Furniture.IsValidIndex(Index)) {
		FurniturePtr = GetWorld()->SpawnActor<AActor>(Furniture[Index], GetActorTransform());
		
		FurniturePtr->SetOwner(this);
		FurniturePtr->SetActorTickEnabled(false);
		FurniturePtr->SetActorEnableCollision(false);
		FurniturePtr->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		
		CutePopIn(IndexToEase.Contains(Index) ? IndexToEase[Index] : EFurniEasing::Default, FurniturePtr.Get());

		if (bCanRemoveFurniture) {
			DisplayMesh->SetHiddenInGame(true);
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("Furniture index out of bounds"));
	}

	PreviousIndex = Index;
}

void ASelectableFurniture::BeginPlay() {
	Super::BeginPlay();

	GAME_STATE

	RenovateModeChanged(GameState->bRenovateHouseMode);
	GameState->OnRenovateHouseMode.AddDynamic(this, &ASelectableFurniture::RenovateModeChanged);

	OnDestroyed.AddDynamic(this, &ASelectableFurniture::DestroyCreated);

	OnRep_SetIndex();
}

int32 ASelectableFurniture::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	GAME_STATE

	if (!GameState->bRenovateHouseMode) {
		return (int32)EInteractCallback::NO_CALLBACK;
	}
	return (int32)EInteractCallback::START_USING_OBJECT;
}

void ASelectableFurniture::Server_Use(AFarmionCharacter* User, bool bUse) {
	if (bUse) {
		GAME_STATE

		if (!GameState->bRenovateHouseMode) {
			return;
		}
		
		if (CooldownTimerHandle.IsValid()) {
			return;
		}
		
		if (Furniture.IsEmpty()) {
			UE_LOG(LogTemp, Error, TEXT("Furniture is empty"));
			return;
		}

		const int32 PrevIndex = Index;

		if (!FurniturePtr.IsValid() || Index == -1) {
			Index = 0;
		} else {
			Index = (Index + 1) % (Furniture.Num() + (bCanRemoveFurniture ? 1 : 0));  
			if (bCanRemoveFurniture && Index == Furniture.Num()) {  
				Index = -1;
			}
		}

		if (PrevIndex == Index) {
			return;
		}
		
		OnRep_SetIndex();

		GetWorldTimerManager().SetTimer(CooldownTimerHandle, [this] {
			CooldownTimerHandle.Invalidate();
		}, 1.5f, false);
	}
}


int32 ASelectableFurniture::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return 0;
}

void ASelectableFurniture::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASelectableFurniture, Index);
}

void ASelectableFurniture::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("CanRemoveFurniture", bCanRemoveFurniture);
	JsonObject->SetNumberField("Index", Index);

	const TSharedPtr<FJsonObject> JsonCollisionObject = MakeShareable(new FJsonObject);
	{
		const FVector& RelativeLocation = Collision->GetRelativeLocation();
		JsonCollisionObject->SetNumberField("CollisionRelativeLocationX", RelativeLocation.X);
		JsonCollisionObject->SetNumberField("CollisionRelativeLocationY", RelativeLocation.Y);
		JsonCollisionObject->SetNumberField("CollisionRelativeLocationZ", RelativeLocation.Z);

		const FRotator& RelativeRotation = Collision->GetRelativeRotation();
		JsonCollisionObject->SetNumberField("CollisionRelativeRotationPitch", RelativeRotation.Pitch);
		JsonCollisionObject->SetNumberField("CollisionRelativeRotationYaw", RelativeRotation.Yaw);
		JsonCollisionObject->SetNumberField("CollisionRelativeRotationRoll", RelativeRotation.Roll);

		const FVector& RelativeScale = Collision->GetRelativeScale3D();
		JsonCollisionObject->SetNumberField("CollisionRelativeScaleX", RelativeScale.X);
		JsonCollisionObject->SetNumberField("CollisionRelativeScaleY", RelativeScale.Y);
		JsonCollisionObject->SetNumberField("CollisionRelativeScaleZ", RelativeScale.Z);

		const FVector& UnscaledBoxExtent = Collision->GetUnscaledBoxExtent();
		JsonCollisionObject->SetNumberField("CollisionUnscaledBoxExtentX", UnscaledBoxExtent.X);
		JsonCollisionObject->SetNumberField("CollisionUnscaledBoxExtentY", UnscaledBoxExtent.Y);
		JsonCollisionObject->SetNumberField("CollisionUnscaledBoxExtentZ", UnscaledBoxExtent.Z);
	}

	JsonObject->SetObjectField("Collision", JsonCollisionObject);
	
	TArray<TSharedPtr<FJsonValue>> FurnitureArray;
	for (auto& FurnitureClass : Furniture) {
		const TSharedPtr<FJsonObject> JsonFurnitureObject = MakeShareable(new FJsonObject);
		ISaveLoad::SaveClassPath(JsonFurnitureObject, "Class", FurnitureClass);
		FurnitureArray.Add(MakeShareable(new FJsonValueObject(JsonFurnitureObject)));
	}

	TArray<TSharedPtr<FJsonValue>> IndexToEaseArray;
	for (auto& IndexToEasePair : IndexToEase) {
		const TSharedPtr<FJsonObject> JsonIndexToEaseObject = MakeShareable(new FJsonObject);
		JsonIndexToEaseObject->SetNumberField("Index", (int32)IndexToEasePair.Key);
		JsonIndexToEaseObject->SetNumberField("Ease", (int32)IndexToEasePair.Value);
		IndexToEaseArray.Add(MakeShareable(new FJsonValueObject(JsonIndexToEaseObject)));
	}

	JsonObject->SetArrayField("IndexToEase", IndexToEaseArray);
	JsonObject->SetArrayField("Furniture", FurnitureArray);
}

void ASelectableFurniture::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bCanRemoveFurniture = JsonObject->GetBoolField("CanRemoveFurniture");

	const TSharedPtr<FJsonObject>& JsonCollision = JsonObject->GetObjectField("Collision");
	Collision->SetRelativeTransform(FTransform(
		FRotator(
			JsonCollision->GetNumberField("CollisionRelativeRotationPitch"),
			JsonCollision->GetNumberField("CollisionRelativeRotationYaw"),
			JsonCollision->GetNumberField("CollisionRelativeRotationRoll")
		),
		FVector(
			JsonCollision->GetNumberField("CollisionRelativeLocationX"),
			JsonCollision->GetNumberField("CollisionRelativeLocationY"),
			JsonCollision->GetNumberField("CollisionRelativeLocationZ")
		),
		FVector(
			JsonCollision->GetNumberField("CollisionRelativeScaleX"),
			JsonCollision->GetNumberField("CollisionRelativeScaleY"),
			JsonCollision->GetNumberField("CollisionRelativeScaleZ")
		)
	));

	Collision->SetBoxExtent(FVector(
		JsonCollision->GetNumberField("CollisionUnscaledBoxExtentX"),
		JsonCollision->GetNumberField("CollisionUnscaledBoxExtentY"),
		JsonCollision->GetNumberField("CollisionUnscaledBoxExtentZ")
	), false);


	Furniture.Empty();
	const TArray<TSharedPtr<FJsonValue>> JsonFurnitureArray = JsonObject->GetArrayField("Furniture");
	for (const TSharedPtr<FJsonValue>& JsonFurnitureValue : JsonFurnitureArray) {
		const TSharedPtr<FJsonObject> JsonTransformObject = JsonFurnitureValue->AsObject();
		Furniture.Add(ISaveLoad::LoadClassPath(JsonTransformObject, "Class"));
	}

	IndexToEase.Empty();
	const TArray<TSharedPtr<FJsonValue>> JsonIndexToEaseArray = JsonObject->GetArrayField("IndexToEase");
	for (const TSharedPtr<FJsonValue>& JsonIndexToEaseValue : JsonIndexToEaseArray) {
		const TSharedPtr<FJsonObject> JsonIndexToEaseObject = JsonIndexToEaseValue->AsObject();
		IndexToEase.Add((int32)JsonIndexToEaseObject->GetNumberField("Index"), (EFurniEasing)JsonIndexToEaseObject->GetNumberField("Ease"));
	}

	Index = JsonObject->GetIntegerField("Index");
	OnRep_SetIndex();
}

void ASelectableFurniture::RenovateModeChanged(bool bRenovateMode) {
	if (bRenovateMode) {
		if (FurniturePtr.IsNull() || !bCanRemoveFurniture) {
			DisplayMesh->SetHiddenInGame(false);
		}
	} else {
		DisplayMesh->SetHiddenInGame(true);
	}
}

bool ASelectableFurniture::IsActive() {
	GAME_STATE

	return GameState->bRenovateHouseMode;
}

FText ASelectableFurniture::GetInformationText_Implementation() const {
	FString Text;

	if (FurniturePtr.IsValid()) {
		Text = FString::Printf(TEXT("Swap Furniture?\n<Sub>Current (%d/%d)</>"), Index + 1, Furniture.Num());
	} else {
		Text = "Select Furniture?";
	}
	return FText::FromString(Text);
}

void ASelectableFurniture::DestroyCreated(AActor* DestroyedActor) {
	if (FurniturePtr.IsValid()) {
		FurniturePtr->Destroy();
	}
}
