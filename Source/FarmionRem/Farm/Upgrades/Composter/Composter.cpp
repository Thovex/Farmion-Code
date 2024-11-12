// Copyright 2020-2023: Jesse J. van Vliet

#include "Composter.h"

#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

AComposter::AComposter() {
	CompostBin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CompostBin"));
	CompostBin->SetupAttachment(RootComponent);

	ComposterInAndOutput = CreateDefaultSubobject<UBoxComponent>(TEXT("ComposterInput"));
	ComposterInAndOutput->SetupAttachment(CompostBin);
}

void AComposter::OnRep_CurrentCompostValue_Implementation() {}
void AComposter::OnRep_CurrentCompostCharges_Implementation() {}

void AComposter::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		ComposterInAndOutput->OnComponentBeginOverlap.AddDynamic(this, &AComposter::OnOverlapBegin);
	}

	OnRep_CurrentCompostValue();
	OnRep_CurrentCompostCharges();
}

void AComposter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AComposter, CurrentCompostValue);
	DOREPLIFETIME(AComposter, CurrentCompostCharges);
}

bool AComposter::IsSourceAvailable() {
	return CurrentCompostCharges > 0;
}

void AComposter::UseSource() {
	CurrentCompostCharges--;
	OnRep_CurrentCompostCharges();
}

TEnumAsByte<ELiquids> AComposter::GetLiquidType() {
	return ELiquids_Compost;
}

TEnumAsByte<EProductColor> AComposter::GetLiquidColor() {
	return EProductColor_Brown;
}

void AComposter::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("CurrentCompostValue", CurrentCompostValue);
	JsonObject->SetNumberField("CurrentCompostCharges", CurrentCompostCharges);
}

void AComposter::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	CurrentCompostValue = JsonObject->GetIntegerField("CurrentCompostValue");
	CurrentCompostCharges = JsonObject->GetIntegerField("CurrentCompostCharges");

	OnRep_CurrentCompostValue();
	OnRep_CurrentCompostCharges();
}

FText AComposter::GetInformationText_Implementation() const {
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	FString Text = UpgradeName.ToString();

	if (CurrentCompostCharges > 0) {
		Text += "\n<Sub>Collect w/ Bucket</>";
	}
	
	return FText::FromString(Text);
}

void AComposter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!bBought) {
		return;
	}
	
	const TScriptInterface<ICompostable> Compostable(OtherActor);

	if (Compostable && Compostable->Execute_CanCompost(OtherActor)) {

		if (CurrentCompostCharges >= RequiredCompostValue
			&& CurrentCompostCharges == MaxCompostCharges) {
			return;	
		}
		
		CurrentCompostValue += Compostable->Execute_GetCompostValue(OtherActor);

		if (AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor)) {
			FTransform TargetTransform = FTransform(GetActorRotation(), GetGrindLocation(), FVector::OneVector);
			TargetTransform.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
			InteractableObject->Multicast_OneTimeMove({
				OtherActor->GetActorTransform(),
				TargetTransform,
			});
		} else {
			OtherActor->Destroy(true);
		}
		
		if (CurrentCompostValue >= RequiredCompostValue) {
			CurrentCompostValue = CurrentCompostValue % RequiredCompostValue;

			if (CurrentCompostCharges != MaxCompostCharges) {
				CurrentCompostCharges++;
				OnRep_CurrentCompostCharges();
			}
		}
		OnRep_CurrentCompostValue();
	}
}
