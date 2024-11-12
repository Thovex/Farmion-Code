// Copyright 2020-2023: Jesse J. van Vliet


#include "Collectable.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Characters/Player/FarmionCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

int32 ACollectable::Interact(AFarmionCharacter* InteractInstigator, const FHitResult& InteractionHit) {
	if (!bCollected) {
		return (int32) EInteractCallback::START_USING_OBJECT;
	}

	return (int32)EInteractCallback::NO_CALLBACK;
}

void ACollectable::Server_Use(AFarmionCharacter* User, bool bUse) {
	if (!bCollected) {
		
		bCollected = true;
		OnRep_Collected();

		TArray<AActor*> AllCollectables;
		UGameplayStatics::GetAllActorsOfClass(this, StaticClass(), AllCollectables);

		int32 TotalCount = 0;
		int32 Count = 0;
		for (AActor* Collectable : AllCollectables) {
			const ACollectable* CollectableCast = Cast<ACollectable>(Collectable);
			if (CollectableCast && CollectableCast->CollectableName == CollectableName) {
				if (CollectableCast->bCollected) {
					Count++;
				}
				TotalCount++;
			}
		}

		GAME_STATE
		GameState->ReceiveMessage_Tunnel(User, FText::FromString(User->GetPlayerState()->GetPlayerName()),
			FText::FromString(FString::Printf(TEXT("Collected a %s. Acquired: %d/%d"), *CollectableName.ToString(), Count, TotalCount)));

		Multicast_Use(bUse);
	}
}

void ACollectable::Multicast_Use(bool bUse) {
	Collected_BP();
}

void ACollectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectable, bCollected);
}

FText ACollectable::GetInformationText_Implementation() const {
	return FText::FromString(FString::Printf(TEXT("%s\n<Sub>a Collectable</>"), *CollectableName.ToString()));
}