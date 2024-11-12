// Copyright 2020-2023: Jesse J. van Vliet


#include "GrowthPad.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Farm/Upgrades/Upgrade.h"
#include "FarmionRem/Interactables/LinkerComponent.h"
#include "Net/UnrealNetwork.h"

AGrowthPad::AGrowthPad() {
	PrimaryActorTick.bCanEverTick = false;
	
	ContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
	ContainerMesh->SetupAttachment(Transform);

	WaterInput = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterInput"));
	WaterInput->SetupAttachment(Transform);

	Linker = CreateDefaultSubobject<ULinkerComponent>(TEXT("Linker"));
}

void AGrowthPad::OnRep_Water_Implementation() {}

void AGrowthPad::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrowthPad, bWatered);
}

void AGrowthPad::BeginPlay() {
	Super::BeginPlay();

	OnRep_Water();
}

int32 AGrowthPad::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

void AGrowthPad::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("bWatered", bWatered);
	JsonObject->SetStringField("Mesh", ContainerMesh->GetStaticMesh()->GetPathName());

	const TSharedPtr<FJsonObject> JsonInputObject = MakeShareable(new FJsonObject);
	const FVector& InputLocation = WaterInput->GetRelativeLocation();
	JsonInputObject->SetNumberField("LocationX", InputLocation.X);
	JsonInputObject->SetNumberField("LocationY", InputLocation.Y);
	JsonInputObject->SetNumberField("LocationZ", InputLocation.Z);

	const FRotator& InputRotation = WaterInput->GetRelativeRotation();
	JsonInputObject->SetNumberField("RotationPitch", InputRotation.Pitch);
	JsonInputObject->SetNumberField("RotationYaw", InputRotation.Yaw);
	JsonInputObject->SetNumberField("RotationRoll", InputRotation.Roll);

	const FVector& InputScale = WaterInput->GetRelativeScale3D();
	JsonInputObject->SetNumberField("ScaleX", InputScale.X);
	JsonInputObject->SetNumberField("ScaleY", InputScale.Y);
	JsonInputObject->SetNumberField("ScaleZ", InputScale.Z);

	const FVector& InputBoxExtent = WaterInput->GetUnscaledBoxExtent();
	JsonInputObject->SetNumberField("BoxExtentX", InputBoxExtent.X);
	JsonInputObject->SetNumberField("BoxExtentY", InputBoxExtent.Y);
	JsonInputObject->SetNumberField("BoxExtentZ", InputBoxExtent.Z);

	JsonObject->SetObjectField("WaterInput", JsonInputObject);

	TArray<USceneComponent*> GrowthLocations;
	ContainerMesh->GetChildrenComponents(false, GrowthLocations);

	TArray<TSharedPtr<FJsonValue>> JsonTransformsArray;
	for (const USceneComponent* GrowthLocation : GrowthLocations) {
		const TSharedPtr<FJsonObject> JsonTransformObject = MakeShareable(new FJsonObject);
		const FVector& Location = GrowthLocation->GetRelativeLocation();
		JsonTransformObject->SetNumberField("LocationX", Location.X);
		JsonTransformObject->SetNumberField("LocationY", Location.Y);
		JsonTransformObject->SetNumberField("LocationZ", Location.Z);

		const FRotator& Rotation = GrowthLocation->GetRelativeRotation();
		JsonTransformObject->SetNumberField("RotationPitch", Rotation.Pitch);
		JsonTransformObject->SetNumberField("RotationYaw", Rotation.Yaw);
		JsonTransformObject->SetNumberField("RotationRoll", Rotation.Roll);

		const FVector& Scale = GrowthLocation->GetRelativeScale3D();
		JsonTransformObject->SetNumberField("ScaleX", Scale.X);
		JsonTransformObject->SetNumberField("ScaleY", Scale.Y);
		JsonTransformObject->SetNumberField("ScaleZ", Scale.Z);
		JsonTransformsArray.Add(MakeShareable(new FJsonValueObject(JsonTransformObject)));
	}
	JsonObject->SetArrayField("GrowthLocations", JsonTransformsArray);
}

void AGrowthPad::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	bWatered = JsonObject->GetBoolField("bWatered");
	OnRep_Water();

	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *JsonObject->GetStringField("Mesh"));
	ContainerMesh->SetStaticMesh(Mesh);

	const auto& WaterInputObject = JsonObject->GetObjectField("WaterInput");
	WaterInput->SetRelativeLocation(FVector(WaterInputObject->GetNumberField("LocationX"), WaterInputObject->GetNumberField("LocationY"), WaterInputObject->GetNumberField("LocationZ")));
	WaterInput->SetRelativeRotation(FRotator(WaterInputObject->GetNumberField("RotationPitch"), WaterInputObject->GetNumberField("RotationYaw"), WaterInputObject->GetNumberField("RotationRoll")));
	WaterInput->SetRelativeScale3D(FVector(WaterInputObject->GetNumberField("ScaleX"), WaterInputObject->GetNumberField("ScaleY"), WaterInputObject->GetNumberField("ScaleZ")));
	WaterInput->SetBoxExtent(FVector(WaterInputObject->GetNumberField("BoxExtentX"), WaterInputObject->GetNumberField("BoxExtentY"), WaterInputObject->GetNumberField("BoxExtentZ")));
	
	const TArray<TSharedPtr<FJsonValue>> JsonTransformsArray = JsonObject->GetArrayField("GrowthLocations");
	for (const TSharedPtr<FJsonValue>& JsonTransformValue : JsonTransformsArray) {
		const TSharedPtr<FJsonObject> JsonTransformObject = JsonTransformValue->AsObject();

		// Create new SceneComponent and set its transform
		USceneComponent* NewChild = NewObject<USceneComponent>(ContainerMesh);
		NewChild->AttachToComponent(ContainerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);

		const FVector SpawnLocation = FVector(JsonTransformObject->GetNumberField("LocationX"), JsonTransformObject->GetNumberField("LocationY"), JsonTransformObject->GetNumberField("LocationZ"));
		const FRotator SpawnRotation = FRotator(JsonTransformObject->GetNumberField("RotationPitch"), JsonTransformObject->GetNumberField("RotationYaw"), JsonTransformObject->GetNumberField("RotationRoll"));
		const FVector SpawnScale = FVector(JsonTransformObject->GetNumberField("ScaleX"), JsonTransformObject->GetNumberField("ScaleY"), JsonTransformObject->GetNumberField("ScaleZ"));

		NewChild->SetRelativeTransform(FTransform(SpawnRotation, SpawnLocation, SpawnScale));
		NewChild->RegisterComponent();
	}
}

bool AGrowthPad::Supply(TEnumAsByte<ELiquids> Liquid, TEnumAsByte<EProductColor> Color, int32 Charges) {
	if (bWatered) {
		return false;
	}

	bWatered = true;
	OnRep_Water();
	return true;
}

void AGrowthPad::PostPerformCycle() {
	if (!bWatered) {
		return;
	}

	if (bHasContext && LinkerContext) {
		if (!LinkerContext->bBought) {
			UE_LOG(LogTemp, Log, TEXT("GrowthPad::PostPerformCycle: Context is not bought"));
			return;
		}
	}

	bWatered = false;
	OnRep_Water();
	
	float TotalWeight = 0.0f;
	for (const auto& Pair : SpawnableActorClasses) {
		TotalWeight += Pair.Value;
	}

	TArray<USceneComponent*> GrowthLocations;
	ContainerMesh->GetChildrenComponents(false, GrowthLocations);

	for (const USceneComponent* GrowthLocation : GrowthLocations) {
		const FTransform& SpawnTransform = GrowthLocation->GetComponentTransform();
		
		if (FMath::FRand() > SpawnChance) {
			continue;
		}

		const float RandomValue = FMath::FRand() * TotalWeight;

		float AccumulatedWeight = 0.0f;
		for (const auto& Pair : SpawnableActorClasses) {
			if ((AccumulatedWeight += Pair.Value) >= RandomValue) {
				GetWorld()->SpawnActor<AInteractableObject>(Pair.Key, SpawnTransform.GetLocation(), SpawnTransform.Rotator());
				break;
			}
		}
	}
}

void AGrowthPad::Link() {
	const AActor* LinkedActor = Linker->GetLinkedActor(TEXT("Context"));
	if (LinkedActor) {
		bHasContext = true;
		LinkerContext = Cast<AUpgrade>(LinkedActor);
	}
}