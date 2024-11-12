// Copyright 2020-2023: Jesse J. van Vliet

#pragma once

#include "FarmionRem/FarmionRem.h"
#include "UObject/Interface.h"
#include "Carryable.generated.h"

class ICarryableSocket;
class AFarmionCharacter;
class ASellContainer;

struct FHitAndPoint {
	bool Hit = false;
	TArray<FVector> Path;
	FVector Point = FVector::ZeroVector;
	FVector Normal = FVector::ZeroVector;
	AActor* Target = nullptr;
};

UENUM(BlueprintType)
enum class ESoundEffectType : uint8 {
	ESoundEffectType_None,
	ESoundEffectType_Wood,
	ESoundEffectType_Organic,
	ESoundEffectType_Stone,
	ESoundEffectType_Metal,
	ESoundEffectType_Glass,
	ESoundEffectType_Plastic,
	ESoundEffectType_Concrete,
	ESoundEffectType_Dirt,
	ESoundEffectType_Sand,
	ESoundEffectType_Gravel,
	ESoundEffectType_Brick,
	ESoundEffectType_Ceramic,
	ESoundEffectType_Paper,
	ESoundEffectType_Cloth,
	ESoundEffectType_Rubber,
	ESoundEffectType_Slime,
};

UENUM(BlueprintType)
enum class ETransformCallback : uint8 {
	ETransformCallback_None,
	ETransformCallback_Initialize,
	ETransformCallback_Finalize,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTransformCallback, ETransformCallback, Callback);

UINTERFACE()
class UCarryable : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class FARMIONREM_API ICarryable {
	GENERATED_BODY()

public:
	virtual AActor* GetActorFromInterface() const;

	virtual int32 MeetsCarryRequirements(const AFarmionCharacter* Character);
	virtual int32 Server_MeetsCarryRequirements(const AFarmionCharacter* Character);

	virtual int32 MeetsDropRequirements(const AFarmionCharacter* Character); // Prediction only.
	virtual int32 Server_MeetsDropRequirements(const AFarmionCharacter* Character, TArray<FVector>& DropPath, FRotator& DropRotation);

	//virtual void TogglePhysics(const bool SimulationMode);
	//virtual void ToggleGravity(bool SimulationMode);
	virtual void SetCollisions(ECollisionEnabled::Type Mode);
	virtual TArray<AActor*> ExtraIgnoreActors() const;

	void GetRotatedCarryableBounds(FBoxSphereBounds& OutBounds);
	void GetCarryableBounds(FBoxSphereBounds& OutBounds);

	bool bIsBeingCarried;
	bool bIsBeingCarriedEarly;

	TSoftObjectPtr<ASellContainer> SellContainer = nullptr;

	TScriptInterface<ICarryableSocket> CarryableSocket;
	uint32 CarryableSocketIndex;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	ESoundEffectType GetCarryEffectType();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	ESoundEffectType GetDropEffectType();

	float CarryTotalDistance;
	float CarryAlpha;
	float CarryDeltaSeconds;
	bool bMustWeld = false;
	bool bCarry;

	float OffsetZ = 0.f;

	int32 CarryCurrentTransformIndex;
	TArray<FTransform> CarryTransforms;
	TSoftObjectPtr<USceneComponent> CarryTarget = nullptr;

	FTimerHandle TickHandle;
	FTransformCallback OnCarryTransformCallback;
	virtual void StartCarryTick();
	virtual void StopCarryTick();
	virtual void CarryTick();

private:
	FHitAndPoint TryHit(UWorld* World, const FVector& Bounds, const FQuat& BoundsRotation, const FVector& InitialTraceStart, const FVector& Velocity, const TArray<AActor*>& IgnoredActors);
	FHitAndPoint PerformLineTracesFromActorBounds(const FVector& Velocity, const TArray<AActor*>& IgnoreActors);

	template <typename SMFunction, typename SKFunction>
	static void ForEachMesh(AActor* Target, SMFunction SmFunc, SKFunction SkFunc) {
		UE_LOG(LogTemp, Log, TEXT("Doing ForEachStaticMesh on %s"), *Target->GetFullName());

		TArray<UStaticMeshComponent*> StaticMeshComponents;
		Target->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		for (auto& StaticMeshComponent : StaticMeshComponents) {
			if (!StaticMeshComponent->ComponentTags.Contains(GCarryable_Mesh_Ignore_Tag_Name) && StaticMeshComponent->GetStaticMesh() != nullptr) {
				SmFunc(StaticMeshComponent);
			}
		}

		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		Target->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

		for (auto& SkeletalMeshComponent : SkeletalMeshComponents) {
			if (!SkeletalMeshComponent->ComponentTags.Contains(GCarryable_Mesh_Ignore_Tag_Name) && SkeletalMeshComponent->GetSkinnedAsset() != nullptr) {
				SkFunc(SkeletalMeshComponent);
			}
		}
	}

	template <typename Interface>
	TScriptInterface<Interface> ContainsInterface(const FHitAndPoint& HitResult) {
		TScriptInterface<Interface> ActorInterface = TScriptInterface<Interface>(HitResult.Target);
		if (ActorInterface) {
			UE_LOG(LogTemp, Log, TEXT("Target is Implementing: %hs"), (typeid(Interface).name()));
			return ActorInterface;
		}

		//UE_LOG(LogTemp, Log, TEXT("Target is Not Implementing Checked Interface: %hs"), (typeid(Interface).name()));
		return nullptr;
	}

	template <typename Class>
	Class* ContainsClass(const FHitAndPoint& HitResult) {
		Class* ActorClass = Cast<Class>(HitResult.Target);
		if (ActorClass) {
			UE_LOG(LogTemp, Log, TEXT("Target is of class: %hs"), (typeid(Class).name()));
			return ActorClass;
		}

		//UE_LOG(LogTemp, Log, TEXT("Target is Not Implementing Checked Class: %hs"), (typeid(Class).name()));
		return nullptr;
	}
};