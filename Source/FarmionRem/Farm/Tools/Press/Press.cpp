// Copyright 2020-2023: Jesse J. van Vliet


#include "Press.h"

#include "Components/SphereComponent.h"
#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Interactables/Product.h"
#include "Net/UnrealNetwork.h"

APress::APress() {
	InputArea = CreateDefaultSubobject<USphereComponent>(TEXT("InputArea"));
	InputArea->SetupAttachment(RootComponent);
}

void APress::OnRep_Progress_Implementation() {}

void APress::OnRep_InputCount_Implementation() {}

int32 APress::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

void APress::InputOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!bBought) {
		return;
	}

	if (!SelectedType) {
		if (!InputTypes.Contains(OtherActor->GetClass())) {
			return;
		}

		SelectedType = OtherActor->GetClass();
	} else {
		if (OtherActor->GetClass() != SelectedType) {
			return;
		}
	}

	if (InputCount >= InputCountRange.Y) {
		return;
	}

	if (AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor)) {
		FTransform TargetTransform = this->GetActorTransform();
		TargetTransform.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
		InteractableObject->Multicast_OneTimeMove({
			OtherActor->GetActorTransform(),
			TargetTransform,
		});
	} else {
		this->Destroy(true);
	}

	InputCount++;
	OnRep_InputCount();

	const float PercentagePerInput = 1.f / InputCountRange.Y;

	Progress *= (InputCount * PercentagePerInput);
	OnRep_Progress();
}

void APress::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		InputArea->OnComponentBeginOverlap.AddDynamic(this, &APress::InputOverlapBegin);
	}
}

void APress::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("Progress", Progress);
	JsonObject->SetNumberField("InputCount", InputCount);
	ISaveLoad::SaveClassPath(JsonObject, "SelectedType", SelectedType);
}

void APress::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	Progress = JsonObject->GetNumberField("Progress");
	InputCount = JsonObject->GetNumberField("InputCount");
	SelectedType = ISaveLoad::LoadClassPath(JsonObject, "SelectedType");

	OnRep_Progress();
	OnRep_InputCount();
}

FText APress::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}
	
	FString InformationStr = FString::Printf(TEXT("<Prod>%s</>"), *ToolName.ToString());

	if (SelectedType) {
		InformationStr += FString::Printf(TEXT("\n<Sub>Using %s (%d/%d)</>"), *SelectedType->GetDefaultObject<AProduct>()->ProductName.ToString(), InputCount, static_cast<int32>(InputCountRange.Y));

		const float ProgressAlpha = (Progress - ProgressRange.X) / (ProgressRange.Y - ProgressRange.X);

		if (ProgressAlpha >= 1.f) {
			InformationStr += FString::Printf(TEXT("\n<Sub>Ready to Collect</>"));
		} else {
			InformationStr += FString::Printf(TEXT("\n<Sub>Needs Pressing...</>"));
		}
	}

	return FText::FromString(InformationStr);
}


void APress::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APress, SelectedType);
	DOREPLIFETIME(APress, Progress);
	DOREPLIFETIME(APress, InputCount);
}

bool APress::IsSourceAvailable() {
	return SelectedType != nullptr && Progress >= ProgressRange.Y;
}

void APress::UseSource() {
	InputCount = 0;
	Progress = 0.f;

	OnRep_InputCount();
	OnRep_Progress();
	
	SelectedType = nullptr;
}

TEnumAsByte<ELiquids> APress::GetLiquidType() {
	if (!SelectedType) {
		return ELiquids_None;
	}

	return InputTypes[SelectedType];
}

TEnumAsByte<EProductColor> APress::GetLiquidColor() {
	if (!SelectedType) {
		return EProductColor_White;
	}

	return SelectedType->GetDefaultObject<AProduct>()->ProductColor;
}