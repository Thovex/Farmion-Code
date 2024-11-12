// Copyright 2020-2023: Jesse J. van Vliet

#include "SellContainer.h"

#include "FarmionRem/Callbacks.h"
#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Interactables/Money.h"
#include "FarmionRem/Interfaces/Interactable/Sellable.h"
#include "Net/UnrealNetwork.h"

UShapeComponent* ASellContainer::GetCollisionShape_Implementation() {
	return nullptr;
}

int32 ASellContainer::MeetsCarryRequirements(const AFarmionCharacter* Character) {
	return (int32)EInteractCallback::NO_CALLBACK;
}

void ASellContainer::PerformCycle() {
	while (!MoneyQueue.IsEmpty())
	{
		FMoneyQueueItem MoneyItem;
		MoneyQueue.Dequeue(MoneyItem);

		if (MoneyItem.SpawnMoney) {
			SpawnMoney(MoneyItem.MonetaryValue);
		} else {
			AddMoney(MoneyItem.MonetaryValue);
		}
	}

	ResetSoldItems();
}


void ASellContainer::SpawnMoney(const int32& MonetaryValue) {
	const auto& SpawnPts = GetSpawnPoints();
	const FTransform& SpawnPt = SpawnPts[FMath::RandRange(0, SpawnPts.Num() - 1)];

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByChannel(Overlaps, SpawnPt.GetLocation(), SpawnPt.GetRotation(), ECC_WorldDynamic, FCollisionShape::MakeSphere(50.0f), QueryParams);

	for (auto& Overlap : Overlaps) {
		AMoney* OverlappingMoney = Cast<AMoney>(Overlap.GetActor());
		if (OverlappingMoney) {
			OverlappingMoney->Money += MonetaryValue;
			OverlappingMoney->OnRep_Money();
			return;
		}
	}

	AMoney* Money = GetWorld()->SpawnActorDeferred<AMoney>(MoneyClass, SpawnPt);
	Money->Money = MonetaryValue;
	Money->FinishSpawning(SpawnPt);
}

void ASellContainer::AddMoney(const int32& MonetaryValue) {
	GAME_STATE
				
	GameState->Money += MonetaryValue;
	GameState->OnRep_Money();
}

bool ASellContainer::Sell(TScriptInterface<ISellable> Sellable) {
	bool bSold = false;
	
	if (!CanSellItem()) {
		bSold = false;
	}
	
	if (Sellable->CanSell()) {
		UE_LOG(LogTemp, Warning, TEXT("Selling %s for %d"), *Sellable.GetObject()->GetName(), Sellable->GetFullSellPrice());
		IncrementSoldItems(Sellable->GetFullSellPrice());
		bSold = true;
		Sellable->Sell();
	}
	
	if (bSold) {
		const int32 MonetaryValue = Sellable->GetFullSellPrice();
		const int32 SellMethod = GetSellMethod();

		UE_LOG(LogTemp, Log, TEXT("Sell method: %d"), SellMethod);
		if (SellMethod & static_cast<int32>(ESellMethodCallback::SpawnMoney) && SellMethod & static_cast<int32>(ESellMethodCallback::Immediate)) {
			UE_LOG(LogTemp, Log, TEXT("Spawning money immediately..."));
			SpawnMoney(MonetaryValue);
			return bSold;
		}

		if (SellMethod & static_cast<int32>(ESellMethodCallback::SpawnMoney) && SellMethod & static_cast<int32>(ESellMethodCallback::NextDay)) {
			UE_LOG(LogTemp, Log, TEXT("Spawning money tomorrow..."));
			MoneyQueue.Enqueue(FMoneyQueueItem{ MonetaryValue, true });
			return bSold;
		}

		if (SellMethod & static_cast<int32>(ESellMethodCallback::Add) && SellMethod & static_cast<int32>(ESellMethodCallback::Immediate)) {
			UE_LOG(LogTemp, Log, TEXT("Adding money immediately..."));
			AddMoney(MonetaryValue);
			return bSold;
		}

		if (SellMethod & static_cast<int32>(ESellMethodCallback::Add) && SellMethod & static_cast<int32>(ESellMethodCallback::NextDay)) {
			UE_LOG(LogTemp, Log, TEXT("Adding money tomorrow..."));
			MoneyQueue.Enqueue(FMoneyQueueItem{ MonetaryValue, false });
			return bSold;
		}
	}
	return bSold;
}

int32 ASellContainer::GetCurrentSoldItemsToday() const {
	return CurrentSoldItemsToday;
}

void ASellContainer::IncrementSoldItems(int32 SellValue) {
	CurrentSoldItemsToday++;
	Multicast_OnSellItem(SellValue);

	if (GetCurrentSoldItemsToday() >=GetMaxSoldItemsPerDay()) {
		SellAvailability = false;
		OnRep_SellAvailability();
	}
}

void ASellContainer::ResetSoldItems() {
	CurrentSoldItemsToday = 0;

	SellAvailability = true;
	OnRep_SellAvailability();
}

bool ASellContainer::CanSellItem() {
	if (GetCurrentSoldItemsToday() + 1 > GetMaxSoldItemsPerDay()) {
		return false;
	}

	return true;
}

void ASellContainer::Multicast_OnSellItem_Implementation(int32 SellValue) {
	OnSellItem(SellValue);
}

void ASellContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASellContainer, SellAvailability);
}
