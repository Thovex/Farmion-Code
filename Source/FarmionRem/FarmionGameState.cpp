// Copyright 2020-2023: Jesse J. van Vliet


#include "FarmionGameState.h"

#include "Characters/Player/FarmionCharacter.h"
#include "Farm/Tools/SellContainer.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/Interactable/Sellable.h"
#include "Net/UnrealNetwork.h"

AFarmionGameState::AFarmionGameState() {
	PrimaryActorTick.bCanEverTick = true;

	Money = 10000;
	OnRep_Money();
}

void AFarmionGameState::OnRep_BonusFarmLevel_Implementation() {}
void AFarmionGameState::OnRep_FarmExperience_Implementation() {}
void AFarmionGameState::OnRep_Money_Implementation() {}
void AFarmionGameState::OnRep_Buffs_Implementation() {}

void AFarmionGameState::Sell(const TScriptInterface<ISellable> Sellable, TSoftObjectPtr<ASellContainer> SellContainer) {
	if (SellContainer->Sell(Sellable)) {
		Cast<AInteractableObject>(Sellable.GetObject())->bNetworkKill = true;
	}
}

void AFarmionGameState::Multicast_LoadScreen_Implementation() {
	LoadScreenBP();
}

void AFarmionGameState::AddPlayerState(APlayerState* PlayerState) {
	Super::AddPlayerState(PlayerState);
	OnUpdateNameplates.Broadcast();
}

void AFarmionGameState::RemovePlayerState(APlayerState* PlayerState) {
	Super::RemovePlayerState(PlayerState);
	OnUpdateNameplates.Broadcast();
}

void AFarmionGameState::EvaluateBuff(const FBuff& Buff, bool bAdditive) {
	switch (Buff.BuffType) {
		case EBuffType::MovementSpeed: {
			switch (Buff.Modifier) {
				case EBuffModifier::Flat: {
					FlatMovementSpeedBonus += bAdditive ? Buff.BuffValue : -Buff.BuffValue;
				} break;
				case EBuffModifier::Percentage: {
					PercentageMovementSpeedBonus += bAdditive ? Buff.BuffValue : -Buff.BuffValue;
				} break;
				default: {
					UE_LOG(LogTemp, Error, TEXT("BuffModifier %d not implemented!"), Buff.Modifier);
				}
				break;
			}
		} break;
		case EBuffType::JumpHeight: {} break;
		case EBuffType::ThrowStrength: {} break;
		case EBuffType::SlowdownDay: {
			// Only supports Percentages.
			check(Buff.Modifier == EBuffModifier::Percentage);
		} break;
		case EBuffType::ExtraMoneyGain: {} break;
		case EBuffType::FarmLevelIncreased: {
			// Only supports Flat.
			check(Buff.Modifier == EBuffModifier::Flat);
			BonusFarmLevel += bAdditive ? Buff.BuffValue : -Buff.BuffValue;
		} break;
		case EBuffType::XPGainIncreased: {} break;
		case EBuffType::Luck: {} break;
		case EBuffType::SnakeBiteResistance: {} break;
		default: {
			UE_LOG(LogTemp, Error, TEXT("BuffType %d not implemented!"), Buff.BuffType);
		} break;
	}
}

AFarmionCharacter* AFarmionGameState::FindCharacterFromPlayerId(int32 PlayerID) {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* PC = Iterator->Get();
		if (PC && PC->PlayerState && PC->PlayerState->GetPlayerId() == PlayerID) {
			return Cast<AFarmionCharacter>(PC->GetPawn());
		}
	}
	return nullptr;
}

void AFarmionGameState::AddBuff(FBuff& Buff) {
	EvaluateBuff(Buff, true);

	if (Buff.BuffId == -1) {
		Buff.BuffId = ++BuffIdGen;

		if (BuffIdGen >= MAX_uint32) {
			BuffIdGen = 0;
		}
	}

	Buffs.Add(Buff);
	BuffsDurations.Add(Buff.BuffDuration);

	OnRep_Buffs();
}

float AFarmionGameState::GetBuffValue(const float BaseValue, const EBuffType& BuffType, bool& HasTypedBuff) const {
	float BuffFlat = 0.f, BuffPerc = 1.f;

	HasTypedBuff = false;
	for (const FBuff& Buff : Buffs) {
		if (Buff.BuffType == BuffType) {
			HasTypedBuff = true;

			if (Buff.Modifier == EBuffModifier::Flat) {
				BuffFlat += Buff.BuffValue;
			} else if (Buff.Modifier == EBuffModifier::Percentage) {
				BuffPerc += Buff.BuffValue;
			}
		}
	}

	return (BaseValue + BuffFlat) * (1 + (BuffPerc / 100.f));
}

void AFarmionGameState::OnRep_RenovateHouseMode() {
	OnRenovateHouseMode.Broadcast(bRenovateHouseMode);
}

int32 AFarmionGameState::CalculateFarmLevel() {
	return FMath::Clamp(FMath::RoundToInt(LevelCurve->GetFloatValue(static_cast<float>(FarmExperience))) + BonusFarmLevel, 1, 100);
}

void AFarmionGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFarmionGameState, Money);
	DOREPLIFETIME(AFarmionGameState, FarmExperience);
	DOREPLIFETIME(AFarmionGameState, CycleDeathCount);
	
	DOREPLIFETIME(AFarmionGameState, Day);

	DOREPLIFETIME(AFarmionGameState, Buffs);

	DOREPLIFETIME(AFarmionGameState, FlatMovementSpeedBonus);
	DOREPLIFETIME(AFarmionGameState, PercentageMovementSpeedBonus);

	DOREPLIFETIME(AFarmionGameState, BonusFarmLevel);
	DOREPLIFETIME(AFarmionGameState, bRenovateHouseMode);
}

void AFarmionGameState::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (HasAuthority()) {
		TArray<int32> IdsToRemove;

		for (int32 i = 0; i < BuffsDurations.Num(); i++) {
			BuffsDurations[i] -= DeltaSeconds;
			if (BuffsDurations[i] <= 0.f) {
				EvaluateBuff(Buffs[i], false);
				IdsToRemove.Add(Buffs[i].BuffId);
			}
		}

		if (!IdsToRemove.IsEmpty()) {
			for (const int32 Id : IdsToRemove) {
				for (int32 i = 0; i < Buffs.Num(); i++) {
					if (Buffs[i].BuffId == Id) {
						BuffsDurations.RemoveAt(i);
						Buffs.RemoveAt(i);
						break;
					}
				}
			}
			OnRep_Buffs();
		}
	} else {
		SetActorTickEnabled(false);
	}
}