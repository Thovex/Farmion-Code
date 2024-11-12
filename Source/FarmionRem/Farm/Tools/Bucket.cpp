
#include "Bucket.h"

#include "Components/BoxComponent.h"
#include "FarmionRem/Interfaces/Liquid/LiquidSource.h"
#include "FarmionRem/Interfaces/Liquid/LiquidTarget.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(FarmionBucket);



ABucket::ABucket() : ATool() {
	PrimaryActorTick.bCanEverTick = true;

	BucketMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BucketMesh"));
	BucketMesh->SetupAttachment(Transform);
	
	LiquidMeshN = CreateDefaultSubobject<UStaticMeshComponent>(FName("LiquidMesh"));
	LiquidMeshN->SetupAttachment(BucketMesh);

	BucketCollision = CreateDefaultSubobject<UBoxComponent>(FName("BucketCollision"));
	BucketCollision->SetupAttachment(BucketMesh);
}

void ABucket::BeginPlay() {
	Super::BeginPlay();

	SetActorTickEnabled(false);

	OnRep_Charges();
	OnRep_Color();
	OnRep_Liquid();

	if (HasAuthority()) {
		UE_LOG(FarmionBucket, Log, TEXT("[Collision] Bound Handling Liquid Insertion"));
		BucketCollision->OnComponentBeginOverlap.AddDynamic(this, &ABucket::BeginBucketCollision);
		BucketCollision->OnComponentEndOverlap.AddDynamic(this, &ABucket::EndBucketCollision);
	}
}

void ABucket::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!bOnCooldown) {
		HandleBucketCollision();
	}
}

void ABucket::OnRep_Charges_Implementation() {
	UE_LOG(FarmionBucket, Log, TEXT("[Charges] RepNotify: [%d]"), Charges);
}

void ABucket::OnRep_Liquid_Implementation() {
	UE_LOG(FarmionBucket, Log, TEXT("[Liquid] RepNotify: [%s]"), *GetLiquidDisplayName(Liquid));
}

void ABucket::OnRep_Color_Implementation() {
	UE_LOG(FarmionBucket, Log, TEXT("[Color] RepNotify: [%s]"), *GetProductColorDisplayName(Color));
}

void ABucket::HandleBucketCollision() {
	if (!bBought) {
		return;
	}
	
	bool bHandledLiquid = false;
	for (auto OverlappingSourceIt = OverlappingSources.CreateIterator(); OverlappingSourceIt; ++OverlappingSourceIt) {
		ILiquidSource* LiquidSourceInt = OverlappingSourceIt->GetInterface();
		if (Charges < MaxCharges && LiquidSourceInt->IsSourceAvailable() && (Liquid == LiquidSourceInt->GetLiquidType() || Liquid == ELiquids::ELiquids_None)) {
			UE_LOG(FarmionBucket, Log, TEXT("[Collision] Refilled Liquid charges [%d] -> [%d]"), Charges, MaxCharges);
			
			Charges = MaxCharges;
			OnRep_Charges();

			if (Liquid == ELiquids::ELiquids_None) {
				Liquid = LiquidSourceInt->GetLiquidType();
				OnRep_Liquid();

				if (Liquid == ELiquids_Milk) {
					Color = LiquidSourceInt->GetLiquidColor();
					OnRep_Color();
				}
			}

			LiquidSourceInt->UseSource();

			bHandledLiquid = true;
		} else {
			OverlappingSources.RemoveAt(OverlappingSourceIt.GetIndex());
			--OverlappingSourceIt;
		}
	}

	for (auto OverlappingTargetIt = OverlappingTargets.CreateIterator(); OverlappingTargetIt; ++OverlappingTargetIt) {
		if (Charges > 0 && OverlappingTargetIt->GetInterface()->Supply(Liquid, Color, 1)) {
			UE_LOG(FarmionBucket, Log, TEXT("[Collision] Supplying Liquid charges [%d] -> [%d]"), Charges, Charges - 1);

			bool bUseCharges = true;
			switch (Quality) {
				case EItemQuality::Arcane: {
					// 90% chance to use a charge
					if (FMath::RandRange(0, 99) < 90) {
						bUseCharges = true;
					}
				} break;
				case EItemQuality::Eldar: {
					// 80% chance to use a charge
					if (FMath::RandRange(0, 99) < 80) {
						bUseCharges = true;
					}
				}
			}

			if (bUseCharges) {
				Charges -= 1;
				OnRep_Charges();
			}

			if (Charges <= 0) {
				Liquid = ELiquids::ELiquids_None;
				OnRep_Liquid();

				Color = EProductColor::EProductColor_White;
				OnRep_Color();
			}

			bHandledLiquid = true;
		} else {
			OverlappingTargets.RemoveAt(OverlappingTargetIt.GetIndex());
			--OverlappingTargetIt;
		}
	}
	
	if (bHandledLiquid) {
		bOnCooldown = true;

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] {
			bOnCooldown = false;
		}, GBucket_Default_Cooldown, false);
	}

	if (OverlappingTargets.IsEmpty() && OverlappingSources.IsEmpty()) {
		SetActorTickEnabled(false);
	}
}

void ABucket::BeginBucketCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult) {
	if (TScriptInterface<ILiquidTarget>(OtherActor)) {
		OverlappingTargets.AddUnique(OtherActor);
		UE_LOG(FarmionBucket, Log, TEXT("[Collision] Liquid Target [%s]"), *OtherActor->GetName());
	}

	if (TScriptInterface<ILiquidSource>(OtherActor)) {
		OverlappingSources.AddUnique(OtherActor);
		UE_LOG(FarmionBucket, Log, TEXT("[Collision] Liquid Source [%s]"), *OtherActor->GetName());
	}

	if (!OverlappingTargets.IsEmpty() || !OverlappingSources.IsEmpty()) {
		SetActorTickEnabled(true);
	}
}

void ABucket::EndBucketCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (TScriptInterface<ILiquidTarget>(OtherActor)) {
		OverlappingTargets.Remove(OtherActor);
	}

	if (TScriptInterface<ILiquidSource>(OtherActor)) {
		OverlappingSources.Remove(OtherActor);
	}

	if (OverlappingTargets.IsEmpty() && OverlappingSources.IsEmpty()) {
		SetActorTickEnabled(false);
	}
}

void ABucket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABucket, Charges);
	DOREPLIFETIME(ABucket, MaxCharges);
	DOREPLIFETIME(ABucket, Liquid);
	DOREPLIFETIME(ABucket, Color);
}

void ABucket::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetNumberField("Charges", Charges);
	JsonObject->SetNumberField("MaxCharges", MaxCharges);
	JsonObject->SetNumberField("Liquid", Liquid);
	JsonObject->SetNumberField("Color", Color);
}

void ABucket::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	Charges = JsonObject->GetNumberField("Charges");
	MaxCharges = JsonObject->GetNumberField("MaxCharges");
	Liquid = static_cast<ELiquids>(JsonObject->GetNumberField("Liquid"));
	Color = static_cast<EProductColor>(JsonObject->GetNumberField("Color"));

	OnRep_Charges();
	OnRep_Liquid();
	OnRep_Color();
}

FText ABucket::GetInformationText_Implementation() const {
	
	if (!bBought) {
		return Super::GetInformationText_Implementation();
	}

	FString Text = Super::GetInformationText_Implementation().ToString();
	
	FString StatusString;
	if (Liquid != ELiquids_None) {
		if (Liquid == ELiquids_Milk) {
			StatusString += GetProductColorDisplayName(Color) + " ";
		}
		StatusString += GetLiquidDisplayName(Liquid) + ", ";
	}
	
	if (Charges <= 0) {
		StatusString += "Empty";
	}
	else if (Charges >= MaxCharges) {
		StatusString += "Full";
	}
	else if (Charges > MaxCharges / 2) {
		StatusString += "Filled";
	}
	else {
		StatusString += "Nearly Empty";
	}

	Text += FString::Printf(TEXT("\n<Sub>%s</>"), *StatusString);

	return FText::FromString(Text);
}

bool ABucket::CanCompost_Implementation() const {
	return false;
}

