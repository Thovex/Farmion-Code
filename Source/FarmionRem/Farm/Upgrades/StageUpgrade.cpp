// Copyright 2020-2023: Jesse J. van Vliet

#include "StageUpgrade.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/FarmionGameState.h"
#include "Net/UnrealNetwork.h"

void AStageUpgrade::OnRep_BuildRequirementsMet_Implementation() {}
void AStageUpgrade::OnRep_BuildRequirementsNecessary_Implementation() {}

AStageUpgrade::AStageUpgrade() {
	BuildInput = CreateDefaultSubobject<UBoxComponent>(TEXT("BuildInput"));
	BuildInput->SetupAttachment(RootComponent);
}


void AStageUpgrade::OnBuildInputBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (bBought) {
		BuildInput->OnComponentBeginOverlap.RemoveAll(this);
		BuildInput->DestroyComponent();
		return;
	}

	UClass* InClass = OtherActor->GetClass();
	if (BuildRequirementsNecessary.Contains(InClass)) {
		BuildRequirementsNecessary.RemoveSingle(InClass);
		BuildRequirementsMet.Add(InClass);
		
		OnRep_BuildRequirementsMet();
		OnRep_BuildRequirementsNecessary();

		if (AInteractableObject* InteractableObject = Cast<AInteractableObject>(OtherActor)) {
			FTransform TargetTransform = GetActorTransform();
			TargetTransform.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
			InteractableObject->Multicast_OneTimeMove({
				OtherActor->GetActorTransform(),
				TargetTransform,
			});
		} else {
			OtherActor->Destroy(true);
		}

		if (MatchesBuildRequirements()) {
			Build();
		}
	}
}

void AStageUpgrade::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		BuildInput->OnComponentBeginOverlap.AddDynamic(this, &AStageUpgrade::OnBuildInputBeginOverlap);
	}

	OnRep_BuildRequirementsMet();
	OnRep_BuildRequirementsNecessary();
}

bool AStageUpgrade::MatchesBuildRequirements() {
	return BuildRequirementsNecessary.IsEmpty();
}

void AStageUpgrade::Build() {
	GAME_STATE
	Quality = DetermineQuality(GameState->CalculateFarmLevel());

	bBought = true;
	OnRep_Bought();
}

void AStageUpgrade::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	// Saving BuildRequirementsMet
	TArray<TSharedPtr<FJsonValue>> JsonBuildRequirementsMet;
	for (const TSubclassOf<AInteractableObject>& Requirement : BuildRequirementsMet) {
		JsonBuildRequirementsMet.Add(MakeShareable(new FJsonValueString(Requirement->GetPathName())));
	}
	JsonObject->SetArrayField("BuildRequirementsMet", JsonBuildRequirementsMet);

	// Saving BuildRequirementsNecessary
	TArray<TSharedPtr<FJsonValue>> JsonBuildRequirementsNecessary;
	for (const TSubclassOf<AInteractableObject>& Requirement : BuildRequirementsNecessary) {
		JsonBuildRequirementsNecessary.Add(MakeShareable(new FJsonValueString(Requirement->GetPathName())));
	}
	JsonObject->SetArrayField("BuildRequirementsNecessary", JsonBuildRequirementsNecessary);
}

void AStageUpgrade::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	TArray<TSharedPtr<FJsonValue>> JsonBuildRequirementsMet = JsonObject->GetArrayField("BuildRequirementsMet");
	BuildRequirementsMet.Empty();
	for (const TSharedPtr<FJsonValue>& JsonValue : JsonBuildRequirementsMet) {
		FString RequirementPath = JsonValue->AsString();
		UClass* RequirementClass = StaticLoadClass(AInteractableObject::StaticClass(), nullptr, *RequirementPath);
		if (RequirementClass) {
			BuildRequirementsMet.Add(RequirementClass);
		}
	}
	OnRep_BuildRequirementsMet();

	TArray<TSharedPtr<FJsonValue>> JsonBuildRequirementsNecessary = JsonObject->GetArrayField("BuildRequirementsNecessary");
	BuildRequirementsNecessary.Empty();
	for (const TSharedPtr<FJsonValue>& JsonValue : JsonBuildRequirementsNecessary) {
		FString RequirementPath = JsonValue->AsString();
		UClass* RequirementClass = StaticLoadClass(AInteractableObject::StaticClass(), nullptr, *RequirementPath);
		if (RequirementClass) {
			BuildRequirementsNecessary.Add(RequirementClass);
		}
	}
	OnRep_BuildRequirementsNecessary();
	
	Super::Load(JsonObject);
}

void AStageUpgrade::OnRep_Bought() {
	Super::OnRep_Bought();

	if (bBought) {
		FinishConstruction();
	}
}

FText AStageUpgrade::GetInformationText_Implementation() const {
	if (!bBought) {
		return FText::FromString(FString::Printf(TEXT("%s\n<Sub>Requires Construction</>\n<Sub>Needs %d Object%hs</>"),
			*UpgradeName.ToString(), BuildRequirementsNecessary.Num(), BuildRequirementsNecessary.Num() > 1 ? "s" : ""));
	}
	return FText::FromString(FString::Printf(TEXT("%s"), *UpgradeName.ToString()));
}

void AStageUpgrade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStageUpgrade, BuildRequirementsMet);
	DOREPLIFETIME(AStageUpgrade, BuildRequirementsNecessary);
}