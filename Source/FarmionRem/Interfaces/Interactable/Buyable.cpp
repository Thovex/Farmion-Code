// Copyright 2020-2023: Jesse J. van Vliet


#include "Buyable.h"

bool IBuyable::CanBuy(const int32 Money) {
	UE_LOG(LogTemp, Log, TEXT("(Money >= Price?): %d >= %d"), Money, IBuyable::Execute_GetBuyPrice(this->_getUObject()));
	return Money >= IBuyable::Execute_GetBuyPrice(this->_getUObject());
}

void IBuyable::Buy() {
	UE_LOG(LogTemp, Error, TEXT("IBuyable::Buy() is not implemented!"));
}

