// Copyright 2020-2023: Jesse J. van Vliet

#include "Carryable.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "FarmionRem/Farm/Storage/CarryableSocket.h"
#include "FarmionRem/Farm/Tools/SellContainer.h"
#include "FarmionRem/Interfaces/Interactable/Sellable.h"
#include "FarmionRem/Util/CommonLibrary.h"
#include "Kismet/GameplayStatics.h"

// Things should always call Super::MeetsCarryRequirements() first.
int32 ICarryable::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	int32 Callback = (int32)EInteractCallback::NO_CALLBACK;
	if (Character && !Character->HeldObject) {
		UCommonLibrary::SetFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED);
	}

	if (bIsBeingCarriedEarly || bIsBeingCarried) {
		UCommonLibrary::ClearFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED); // Note: It's clearFlag
		return Callback;
	}

	if (CarryableSocket && !CarryableSocket->IsAllowedToPickFromSocket()) { 
		UCommonLibrary::ClearFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED); // Note: It's clearFlag
		return Callback;
	}

	if (TickHandle.IsValid()) {
		UCommonLibrary::ClearFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED); // Note: It's clearFlag
		return Callback;
	}

	const AInteractableObject* AsInteractable = Cast<AInteractableObject>(GetActorFromInterface());
	if (AsInteractable && AsInteractable->bNetworkKill) {
		UCommonLibrary::ClearFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_CARRIED); // Note: It's clearFlag
		return Callback;
	}

	return Callback;
}

void ICarryable::StartCarryTick() {
	if (const UWorld* World = this->_getUObject()->GetWorld()) {
		CarryAlpha = 0.f;
		CarryCurrentTransformIndex = 1;

		for (int32 i = 0; i < CarryTransforms.Num() - 1; i++) {
			CarryTotalDistance += FVector::Dist(CarryTransforms[i].GetLocation(), CarryTransforms[i + 1].GetLocation());
		}
		
		World->GetTimerManager().SetTimer(TickHandle, [this]() {
		  this->CarryTick();
		}, World->GetDeltaSeconds(), true);
	}
}

void ICarryable::StopCarryTick() {
	if (const UWorld* World = this->_getUObject()->GetWorld()) {
		World->GetTimerManager().ClearTimer(TickHandle);

		if (bMustWeld && CarryTarget) {
			GetActorFromInterface()->AttachToComponent(CarryTarget.Get(), FAttachmentTransformRules::KeepWorldTransform);
		}

		CarryTarget = nullptr;
		bMustWeld = false;

		const AInteractableObject* AsInteractable = Cast<AInteractableObject>(GetActorFromInterface());
		if (AsInteractable && AsInteractable->bNetworkKill) {
			this->GetActorFromInterface()->Destroy(true);
		}
	}
}

void ICarryable::CarryTick() {
	CarryDeltaSeconds = this->_getUObject()->GetWorld()->GetDeltaSeconds();

	if (!IsValid(GetActorFromInterface())) {
		UE_LOG(LogTemp, Error, TEXT("INVALID ACTOR: %s"), *GetActorFromInterface()->GetName());
		StopCarryTick();
		return;
	}

	if (CarryTarget) {
		FVector UpdateLocation = CarryTarget->GetComponentLocation();
		if (CarryableSocket) {
			const auto& SocketInfo = CarryableSocket->AvailableSockets[CarryableSocketIndex];
			if (SocketInfo.Pivot == nullptr) {
				UpdateLocation -= FVector(0, 0, OffsetZ);
			} else {
				UpdateLocation = SocketInfo.Pivot->GetComponentLocation();
			}
		}

		const FTransform UpdatedTransform = FTransform(CarryTarget->GetComponentRotation(), UpdateLocation, CarryTarget->GetComponentScale());
		
		// Update last Transform to match the Target we want to follow.
		CarryTransforms.Last() = UpdatedTransform;
	}

	if (CarryCurrentTransformIndex < CarryTransforms.Num()) {
		CarryAlpha = FMath::Clamp(CarryAlpha + (9.81f * CarryDeltaSeconds), 0.f, 1.f);
	}

	if (FMath::IsNearlyEqual(CarryAlpha, 0.f)) {
		GetActorFromInterface()->SetActorTransform(CarryTransforms[CarryCurrentTransformIndex - 1]);

		OnCarryTransformCallback.Broadcast(ETransformCallback::ETransformCallback_Initialize);
		return;
	}

	// When CarryAlpha reaches 1, we have arrived at the current end transform
	if (FMath::IsNearlyEqual(CarryAlpha, 1.f)) {
		// If we've reached the end of the transform sequence, stop the tick
		if (CarryCurrentTransformIndex >= CarryTransforms.Num() - 1) {
			GetActorFromInterface()->SetActorTransform(CarryTransforms.Last());

			OnCarryTransformCallback.Broadcast(ETransformCallback::ETransformCallback_Finalize);
			StopCarryTick();

			if (bCarry) {
				bIsBeingCarried = true;
				return;
			}

			SetCollisions((CarryableSocket && CarryableSocket->TurnSocketedCollisionOff()) ? ECollisionEnabled::QueryOnly : ECollisionEnabled::Type::QueryAndPhysics);
			bIsBeingCarriedEarly = false;
			bIsBeingCarried = false;
			return;
		}

		// Move to the next transform in the sequence
		CarryCurrentTransformIndex++;
		CarryAlpha = 0.f;  // Reset CarryAlpha
	}

	FTransform InterpolatedTransform;
	InterpolatedTransform.Blend(CarryTransforms[CarryCurrentTransformIndex - 1], CarryTransforms[CarryCurrentTransformIndex], CarryAlpha);

	GetActorFromInterface()->SetActorTransform(InterpolatedTransform);
}

FHitAndPoint ICarryable::TryHit(UWorld* World, const FVector& Bounds, const FQuat& BoundsRotation, const FVector& InitialTraceStart, const FVector& Velocity, const TArray<AActor*>& IgnoredActors) {
	TArray<AActor*> IgnoredActorsInstance = IgnoredActors;

	FPredictProjectilePathParams Params;
	Params.LaunchVelocity = Velocity;
	Params.bTraceComplex = true;
	Params.bTraceWithCollision = true;
	Params.SimFrequency = 5.f;
	Params.MaxSimTime = 10.f;
	Params.DrawDebugType = EDrawDebugTrace::None;
	//Params.DrawDebugTime = 5.f;

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	bool bToSocket = false;

	FHitAndPoint HitPoint;
	FPredictProjectilePathResult Result;

	bool bDoLineTrace;
	do {
		bDoLineTrace = false;
		Params.StartLocation = InitialTraceStart;
		Params.ActorsToIgnore = IgnoredActorsInstance;

		TraceParams.ClearIgnoredActors();
		TraceParams.AddIgnoredActors(IgnoredActorsInstance);
		
		UGameplayStatics::PredictProjectilePath(World, Params, Result);

		TArray<FVector> FullPath; // Store full path here
		
		// Perform line trace for each segment
		for (int32 i = 0; i < Result.PathData.Num() - 1; i++) {
			FVector SegmentTraceStart = Result.PathData[i].Location;
			FVector SegmentTraceEnd = Result.PathData[i+1].Location;

			FullPath.Add(SegmentTraceStart); // Add each segment's start point to the path

			FHitResult BoxTraceResult;
			World->SweepSingleByChannel(BoxTraceResult, SegmentTraceStart, SegmentTraceEnd, BoundsRotation, ECC_Visibility, FCollisionShape::MakeBox(Bounds), TraceParams);
			
			//DrawDebugLine(World, SegmentTraceStart, SegmentTraceEnd, FColor::Red, false, 5.f, 0, 5.f);
			//DrawDebugBox(World, SegmentTraceStart, Bounds, BoundsRotation, FColor::Red, false, 5.f, 0, 5.f);
			//DrawDebugBox(World, SegmentTraceEnd, Bounds, BoundsRotation, FColor::Red, false, 5.f, 0, 5.f);

			if (BoxTraceResult.bBlockingHit) {
				AActor* HitActor = BoxTraceResult.GetActor();
				if (BoxTraceResult.GetComponent()->ComponentHasTag(GClip_Through_Object_Tag_Name)) {

					IgnoredActorsInstance.Add(HitActor);
					bDoLineTrace = true;
					break;
				}

				TScriptInterface<ICarryableSocket> CarryableSocketInt = TScriptInterface<ICarryableSocket>(HitActor);
				if (CarryableSocketInt) {
					FCarryableSocketInfo Info;
					int32 Index;
					if (CarryableSocketInt->GetSocketsInUse() < CarryableSocketInt->AvailableSockets.Num()
						&& CarryableSocketInt->GetMatchingSocket(((SegmentTraceStart + SegmentTraceEnd)/2), Info, Index, this->GetActorFromInterface())) {

						HitPoint.Hit = true;
						if (Info.Pivot == nullptr) {
							FullPath.Add(Info.Component->GetComponentLocation()); // Add impact point to path when blocking hit is found
							HitPoint.Point = Info.Component->GetComponentLocation();
							HitPoint.Normal = BoxTraceResult.ImpactNormal;
						} else {
							FullPath.Add(Info.Pivot->GetComponentLocation());
							HitPoint.Point = Info.Pivot->GetComponentLocation();
							HitPoint.Normal = Info.Pivot->GetForwardVector();
						}

						HitPoint.Path = FullPath;
						HitPoint.Target = HitActor;

						bToSocket = true;
						break;  // Break the loop as we have hit something.
					} 
				}

				FullPath.Add(SegmentTraceEnd); // Add impact point to path when blocking hit is found
				HitPoint.Hit = true;
				HitPoint.Point = SegmentTraceEnd;
				HitPoint.Normal = BoxTraceResult.ImpactNormal;
				HitPoint.Path = FullPath;
				HitPoint.Target = HitActor;
				UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *HitActor->GetName());
				break;  // Break the loop as we have hit something.
			}
		}

		if (!bDoLineTrace && !HitPoint.Hit) { 
			// If no blocking hit was found, assign the whole predicted path
			HitPoint.Hit = false; // There was no hit
			HitPoint.Path = FullPath; // Full path is assigned even when no hit occurs
		}
	} while (bDoLineTrace);

	HitPoint.Path.Last() = Result.PathData.Last().Location; // Replace last point with the last point of the predicted path

	if (!bToSocket) {
		FHitResult ToGroundHitResult;
		if (World->LineTraceSingleByChannel(ToGroundHitResult, HitPoint.Path.Last() + FVector(0.f, 0.f, 10.f), HitPoint.Path.Last() - FVector(0.f, 0.f, 1000.f), ECC_Visibility, TraceParams)) {
			AActor* HitActor = ToGroundHitResult.GetActor();

			TScriptInterface<ICarryableSocket> CarryableSocketInt = TScriptInterface<ICarryableSocket>(HitActor);
			if (CarryableSocketInt) {
				FCarryableSocketInfo Info;
				int32 Index;
				if (CarryableSocketInt->GetSocketsInUse() < CarryableSocketInt->AvailableSockets.Num()
					&& CarryableSocketInt->GetMatchingSocket(Info, Index, this->GetActorFromInterface())) {
					HitPoint.Path.Add(Info.Component->GetComponentLocation()); // Add impact point to path when blocking hit is found
					HitPoint.Hit = true;
					HitPoint.Point = Info.Component->GetComponentLocation();
					HitPoint.Normal = ToGroundHitResult.ImpactNormal;
					HitPoint.Target = HitActor;

					bToSocket = true;
					return HitPoint;  // Break the loop as we have hit something.
				} 
			}

			HitPoint.Path.Add(ToGroundHitResult.ImpactPoint);
			HitPoint.Normal = ToGroundHitResult.ImpactNormal;
		}
	}

	return HitPoint;
}

FHitAndPoint ICarryable::PerformLineTracesFromActorBounds(const FVector& Velocity, const TArray<AActor*>& IgnoreActors) {
	AActor* ActorToTraceFrom = GetActorFromInterface();

	FBoxSphereBounds CarryableBounds;
	GetCarryableBounds(CarryableBounds);
	
	TArray<AActor*> IgnoredActors = IgnoreActors;
	IgnoredActors.Add(ActorToTraceFrom);

	const FQuat ActorRotation = ActorToTraceFrom->GetActorRotation().Quaternion();
	FQuat RotationQuat(FVector::UpVector, PI / 2.0f);

	return TryHit(ActorToTraceFrom->GetWorld(), CarryableBounds.BoxExtent, ActorRotation * RotationQuat, CarryableBounds.Origin, Velocity, IgnoredActors);
}

int32 ICarryable::MeetsDropRequirements(const AFarmionCharacter* Character) {
	int32 Callback = (int32)EInteractCallback::NO_CALLBACK;
	
	if (Character) {
		if (Character->HeldObject) {
			UCommonLibrary::SetFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_DROPPED);
		}
	}

	return Callback;;
}


int32 ICarryable::Server_MeetsCarryRequirements(const AFarmionCharacter* Character) {
	const int32 Callback = MeetsCarryRequirements(Character);

	if (UCommonLibrary::HasFlag(Callback, EInteractCallback::CARRYABLE_CAN_BE_CARRIED) && CarryableSocket) {
		CarryableSocket->SetSocketAvailability(GetActorFromInterface(), CarryableSocketIndex, true);
		CarryableSocket = nullptr;
		CarryableSocketIndex = 0;
	}

	return Callback;
}

int32 ICarryable::Server_MeetsDropRequirements(const AFarmionCharacter* Character, TArray<FVector>& DropPath, FRotator& DropRotation) {
	int32 Callback = static_cast<int32>(EInteractCallback::NO_CALLBACK);

	if (Character) {
		if (Character->HeldObject) {
			AActor* Actor = Cast<AActor>(Character->HeldObject->_getUObject());

			const FHitAndPoint HitResult = PerformLineTracesFromActorBounds(Character->ThrowVelocity, ExtraIgnoreActors());
			DropPath = HitResult.Path;

			if (const TScriptInterface<ICarryableSocket> CarryableSocketInt = ContainsInterface<ICarryableSocket>(HitResult)) {
				FCarryableSocketInfo Info;
				int32 Index;

				const TScriptInterface<ICarryable> CarryableInt = TScriptInterface<ICarryable>(static_cast<AInteractableObject*>(Character->HeldObject.Get()));
				if (CarryableSocketInt->GetMatchingSocket(DropPath.Last(), Info, Index, CarryableInt)) {
					if (this->CarryableSocket) {
						// Leaking Sockets here... Ducttape :-)
						this->CarryableSocket->SetSocketAvailability(CarryableInt, this->CarryableSocketIndex, true);
					}

					CarryableSocketInt->SetSocketAvailability(CarryableInt, Index, false);
					FVector SocketOrigin, SocketBoundsExtent;
					Info.GetSocketBounds(SocketOrigin, SocketBoundsExtent);

					this->CarryableSocket = CarryableSocketInt;
					this->CarryableSocketIndex = Index;
					this->CarryTarget = Info.Component;
					this->bMustWeld = true;

					DropPath.Last() = SocketOrigin;

					if (Info.Pivot == nullptr) {
						this->OffsetZ = SocketBoundsExtent.Z;
						DropPath.Last().Z -= this->OffsetZ;
					} else {
						this->OffsetZ = 0.f;
						DropPath.Last() = Info.Pivot->GetComponentLocation();
					}

					DropRotation = Info.Component->GetComponentRotation();
					
					//UE_LOG(LogTemp, Log, TEXT("SocketBoundsExtent: (%f, %f, %f)"), SocketBoundsExtent.X, SocketBoundsExtent.Y, SocketBoundsExtent.Z);
					//UE_LOG(LogTemp, Log, TEXT("DropPoint: (%f, %f, %f)"), DropPoint.X, DropPoint.Y, DropPoint.Z);

					UCommonLibrary::SetFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_DROPPED);
					return Callback;
				}
			}

			if (CarryableSocket) { // HACK
				return Callback;
			}

			ASellContainer* SellContainerInt = ContainsClass<ASellContainer>(HitResult);
			if (SellContainerInt
				&& SellContainerInt->CanSellItem()
				&& Cast<ISellable>(Actor)) {

				this->SellContainer = SellContainerInt;
				UCommonLibrary::SetFlag(Callback, (int32)EInteractCallback::INTERACTABLE_CAN_BE_SOLD);
			}

			DropRotation = HitResult.Normal.Rotation();

			const float CurrentYaw = Actor->GetActorRotation().Yaw;

			DropRotation.Yaw = CurrentYaw;

			// Assume the maximum tilt for both Pitch and Roll is 15 degrees
			constexpr float MinTilt = -15.0f;
			constexpr float MaxTilt = 15.0f;

			// Adjust Pitch and Roll, then clamp them
			DropRotation.Pitch = FMath::Clamp(DropRotation.Pitch - 90.0f, MinTilt, MaxTilt);
			DropRotation.Roll = FMath::Clamp(DropRotation.Roll, MinTilt, MaxTilt);
			
			this->CarryableSocket = nullptr;
			this->CarryableSocketIndex = 0;
			this->CarryTarget = nullptr;
			this->bMustWeld = false;
			this->OffsetZ = 0.f;

			UCommonLibrary::SetFlag(Callback, (int32)EInteractCallback::CARRYABLE_CAN_BE_DROPPED);
			return Callback;
		}
	}
	return Callback;
}

AActor* ICarryable::GetActorFromInterface() const {
	return Cast<AActor>(this->_getUObject());
}

void ICarryable::GetRotatedCarryableBounds(FBoxSphereBounds& OutBounds) {
	AActor* Actor = GetActorFromInterface();
	const FRotator Rotation = Actor->GetActorRotation();
	Actor->SetActorRotation(FRotator(0, 0, 0));
	GetCarryableBounds(OutBounds);
	Actor->SetActorRotation(Rotation);
}

void ICarryable::GetCarryableBounds(FBoxSphereBounds& OutBounds) {
	AActor* Actor = GetActorFromInterface();

	if (!Actor) {
		return;
	}

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

	if (StaticMeshComponents.Num() == 0 && SkeletalMeshComponents.Num() == 0) {
		return;
	}

	FBox ActorBounds(EForceInit::ForceInitToZero);

	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents) {
		const FRotator OldRot = StaticMeshComponent->GetComponentRotation();
		if (!StaticMeshComponent || !StaticMeshComponent->GetStaticMesh()) {
			continue;
		}

		StaticMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
		FTransform ComponentTransform = StaticMeshComponent->GetComponentTransform();
		FBoxSphereBounds ComponentBounds = StaticMeshComponent->GetStaticMesh()->GetBounds();
		const FBox TransformedBounds = ComponentBounds.GetBox().TransformBy(ComponentTransform);
		StaticMeshComponent->SetWorldRotation(OldRot);

		ActorBounds += TransformedBounds;
	}

	for (USkeletalMeshComponent* SkeletalMeshComponent : SkeletalMeshComponents) {
		const FRotator OldRot = SkeletalMeshComponent->GetComponentRotation();
		if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetSkinnedAsset()) {
			continue;
		}

		SkeletalMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
		FTransform ComponentTransform = SkeletalMeshComponent->GetComponentTransform();
		FBoxSphereBounds ComponentBounds = SkeletalMeshComponent->GetSkinnedAsset()->GetBounds();
		const FBox TransformedBounds = ComponentBounds.GetBox().TransformBy(ComponentTransform);
		SkeletalMeshComponent->SetWorldRotation(OldRot);

		ActorBounds += TransformedBounds;
	}

	OutBounds = FBoxSphereBounds(ActorBounds);
}

void ICarryable::SetCollisions(const ECollisionEnabled::Type Mode) {
	AActor* Actor = GetActorFromInterface();

	ForEachMesh(Actor, [&](UStaticMeshComponent* StaticMeshComponent) {
		UE_LOG(LogTemp, Log, TEXT("[%s] ToggleCollisions to to: %hs on %s"), *GET_STR_ROLE(Actor->GetRemoteRole()), Mode == ECollisionEnabled::Type::NoCollision ? "No Collision" : "QueryAndPhysics", *StaticMeshComponent->GetFullName());

		switch (Mode) {
			case ECollisionEnabled::Type::NoCollision:
			{
				StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
				StaticMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
			} break;
			case ECollisionEnabled::Type::QueryAndPhysics:
			{
				StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			} break;
			default:
			{
				UE_LOG(LogTemp, Error, TEXT("Unhandled ECollisionEnabled::Type: %d"), Mode);
			} break;
		} 
	}, [&](USkeletalMeshComponent* SkeletalMeshComponent) {
		UE_LOG(LogTemp, Log, TEXT("[%s] ToggleCollisions to to: %hs on %s"), *GET_STR_ROLE(Actor->GetRemoteRole()), Mode == ECollisionEnabled::Type::NoCollision ? "No Collision" : "QueryAndPhysics", *SkeletalMeshComponent->GetFullName());

		switch (Mode) {
			case ECollisionEnabled::Type::NoCollision:
			{
				SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
				SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
			} break;
			case ECollisionEnabled::Type::QueryAndPhysics:
			{
				SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			} break;
			default:
			{
				UE_LOG(LogTemp, Error, TEXT("Unhandled ECollisionEnabled::Type: %d"), Mode);
			} break;
		} 
	});
}

TArray<AActor*> ICarryable::ExtraIgnoreActors() const {
	return TArray<AActor*>();
}