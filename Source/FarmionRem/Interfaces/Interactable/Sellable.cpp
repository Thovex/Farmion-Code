// Copyright 2020-2023: Jesse J. van Vliet


#include "Sellable.h"

#include "FarmionRem/FarmionGameState.h"
#include "FarmionRem/Macros.h"
#include "FarmionRem/Interfaces/Interactable/Quality.h"


int32 ISellable::GetFullSellPrice() const {
	const TScriptInterface<IIQuality> QualityInt = TScriptInterface<IIQuality>(this->_getUObject());

	int32 BaseSellPrice = Execute_BaseSellPrice(this->_getUObject());
	UE_LOG(LogTemp, Log, TEXT("Base Sell Price of %s: %d"), *this->_getUObject()->GetName(), BaseSellPrice);

	AFarmionGameState* GameState = _getUObject()->GetWorld()->GetGameState<AFarmionGameState>();
	check(GameState)

	bool bHasExtraMoneyGainBuff;
	BaseSellPrice = GameState->GetBuffValue(static_cast<float>(BaseSellPrice), EBuffType::ExtraMoneyGain, bHasExtraMoneyGainBuff);
	UE_LOG(LogTemp, Log, TEXT("Sell Price after ExtraMoneyGain Buff: %d"), BaseSellPrice);
	
	if (QualityInt) {
		const TMap<EItemQuality, float>& QualitySellPriceModifier = Execute_GetQualitySellPriceModifier(this->_getUObject());
		const EItemQuality Quality = QualityInt->GetQuality();
		
		if (QualitySellPriceModifier.Contains(Quality)) {
			return BaseSellPrice * QualitySellPriceModifier[Quality];
		}

		UE_LOG(LogTemp, Error, TEXT("ISellable::GetFullSellPrice() - QualitySellPriceModifier does not contain the quality of this object! [Fallback: Returning BaseSellPrice]"));
	}

	return BaseSellPrice;
}

void ISellable::Sell() {
	UE_LOG(LogTemp, Error, TEXT("ISellable::Sell() is not implemented!"));
}

bool ISellable::CanSell() {
	return true;
}