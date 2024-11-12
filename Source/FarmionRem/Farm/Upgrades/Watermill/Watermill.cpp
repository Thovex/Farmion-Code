#include "Watermill.h"

#include "Net/UnrealNetwork.h"


AWatermill::AWatermill() {}

void AWatermill::OnRep_Rotating_Implementation() {
	if (HasAuthority()) {
		if (FMath::IsNearlyEqual(SecondsToRotate, -1.f)) {
			return;
		}
		
		GetWorld()->GetTimerManager().SetTimer(RotateTimer, this, &AWatermill::StopRotation, SecondsToRotate, false);
	}
}

void AWatermill::BeginPlay() {
	Super::BeginPlay();

	OnRep_Rotating();
}

void AWatermill::Save(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Save(JsonObject);

	JsonObject->SetBoolField("Rotating", bRotating);
	JsonObject->SetNumberField("SecondsToRotate", SecondsToRotate);
	JsonObject->SetNumberField("RotateTimer", GetWorld()->GetTimerManager().GetTimerRemaining(RotateTimer));
}

void AWatermill::Load(const TSharedPtr<FJsonObject>& JsonObject) {
	Super::Load(JsonObject);

	SecondsToRotate = JsonObject->GetNumberField("SecondsToRotate");
	bRotating = JsonObject->GetBoolField("Rotating");
	OnRep_Rotating();

	if (FMath::IsNearlyEqual(SecondsToRotate, -1.f)) {
		return;
	}
		
	GetWorld()->GetTimerManager().SetTimer(RotateTimer, this, &AWatermill::StopRotation, JsonObject->GetNumberField("RotateTimer"), false);
}

void AWatermill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWatermill, bRotating);
}

void AWatermill::StopRotation() {
	bRotating = false;
	OnRep_Rotating();
}