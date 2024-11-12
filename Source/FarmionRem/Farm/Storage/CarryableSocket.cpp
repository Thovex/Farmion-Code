// Copyright 2020-2023: Jesse J. van Vliet

#include "CarryableSocket.h"

#include "Components/ShapeComponent.h"
#include "FarmionRem/Interfaces/Interactable/Carryable.h"
#include "Kismet/KismetSystemLibrary.h"

void FCarryableSocketInfo::GetSocketBounds(FVector& Origin, FVector& BoundsExtent) const {
	float SphereRadius; // Ignore.
	UKismetSystemLibrary::GetComponentBounds(Component, Origin, BoundsExtent, SphereRadius);
}

void ICarryableSocket::OnBeginPlay() {
	AvailableSockets.Empty();
	
	TArray<FCarryableSocketInfo> SocketInfos = Execute_GetCarryableSocketInfo(_getUObject());
	for (auto& SocketInfo : SocketInfos) {
		AvailableSockets.Add(SocketInfo);
	}

	Dispenser = Execute_GetSocketDispenserCollision(_getUObject());

	if (Dispenser) {
		//Dispenser->OnComponentBeginOverlap.AddDynamic(this, &ICarryableSocket::OnDispenserBeginOverlap);		
	}
}

AActor* ICarryableSocket::GetActorFromInterface() {
	return Cast<AActor>(this->_getUObject());
}

bool ICarryableSocket::MatchesSocketRequirements(const FCarryableSocketInfo& Info, const TScriptInterface<ICarryable>& Carryable) {
	if (IgnoreBounds()) {
		return true;
	}
	
	FBoxSphereBounds CarryableBounds;
	Carryable->GetCarryableBounds(CarryableBounds);

	FVector SocketOrigin, SocketBoundsExtent;
	Info.GetSocketBounds(SocketOrigin, SocketBoundsExtent);
	
	UE_LOG(LogTemp, Error, TEXT("AABB Extent: X=%f, Y=%f, Z=%f"), CarryableBounds.BoxExtent.X, CarryableBounds.BoxExtent.Y, CarryableBounds.BoxExtent.Z);
	UE_LOG(LogTemp, Error, TEXT("Max Allowed Bounds: X=%f, Y=%f, Z=%f"), SocketBoundsExtent.X, SocketBoundsExtent.Y, SocketBoundsExtent.Z);

	if (CarryableBounds.BoxExtent.X >SocketBoundsExtent.X
		|| CarryableBounds.BoxExtent.Y > SocketBoundsExtent.Y
		|| CarryableBounds.BoxExtent.Z > SocketBoundsExtent.Z) {
		const FString CarryableName = Carryable->GetActorFromInterface()->GetName();
		const FString ErrorMessage = FString::Printf(
			TEXT("The %s cannot be placed in this socket because its size exceeds the maximum allowed dimensions. " \
					"The maximum allowed dimensions for this socket are: X=%f, Y=%f, Z=%f. The size of the %s is: X=%f, Y=%f, Z=%f."),
					*CarryableName, SocketBoundsExtent.X, SocketBoundsExtent.Y, SocketBoundsExtent.Z, *CarryableName, CarryableBounds.BoxExtent.X, CarryableBounds.BoxExtent.Y, CarryableBounds.BoxExtent.Z);

		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return false;
	}
	

	return true;
}

bool ICarryableSocket::IsAllowedToPickFromSocket() {
	return true;
}

bool ICarryableSocket::GetMatchingSocket(const FVector& OverridePos, FCarryableSocketInfo& OutSocketInfo, int32& OutIndex, const TScriptInterface<ICarryable>& Carryable) {
	check(!AvailableSockets.IsEmpty());

	// [OPTIMIZATION TIP]: This checks MatchesSocketRequirements - but in a lot of cases, ALL requirements will fail (e.g. wrong type, such as Not being a Curio, etc)

	bool bFoundMatchingSocket = false;
	float MinDistanceSquared = MAX_FLT;
	int32 MinDistanceIndex = -1;
	FCarryableSocketInfo MinDistanceSocketInfo;

	for (auto It = AvailableSockets.CreateConstIterator(); It; ++It) {
		const FCarryableSocketInfo& SocketInfo = *It;

		if (SocketInfo.Available && MatchesSocketRequirements(SocketInfo, Carryable)) {
			FVector SocketWorldPos = SocketInfo.Component->GetComponentLocation();
			const float DistanceSquared = FVector::DistSquared(OverridePos, SocketWorldPos);

			//DrawDebugLine(Carryable->GetActorFromInterface()->GetWorld(), OverridePos, SocketWorldPos, FColor::Red, false, 1.0f, 0, 1.0f);

			if (DistanceSquared < MinDistanceSquared) {
				MinDistanceSquared = DistanceSquared;
				MinDistanceIndex = It.GetIndex();
				MinDistanceSocketInfo = SocketInfo;
				bFoundMatchingSocket = true;
			}
		}
	}

	if (bFoundMatchingSocket) {
		OutSocketInfo = MinDistanceSocketInfo;
		OutIndex = MinDistanceIndex;
	}

	return bFoundMatchingSocket;
}

bool ICarryableSocket::GetMatchingSocket(FCarryableSocketInfo& OutSocketInfo, int32& OutIndex, const TScriptInterface<ICarryable>& Carryable) {
	return GetMatchingSocket(Carryable->GetActorFromInterface()->GetActorLocation(), OutSocketInfo, OutIndex, Carryable);
}

bool ICarryableSocket::TurnSocketedCollisionOff() {
	return false;
}

int32 ICarryableSocket::GetSocketsInUse() const {
	return AvailableSockets.Num() - AvailableSockets.FilterByPredicate([](const FCarryableSocketInfo& Info) {
		return Info.Available;
	}).Num();
}
bool ICarryableSocket::IgnoreBounds() const {
	return false;
}
