#pragma once

#include "GlobalValues.h"

#define GET_STR_ROLE(Role) FString(TEXT("TODO: ENetRole to Str"))
#define WILL_EXPIRE(Actor) (Actor->IsPendingKill() || !GetValid(Actor) || (Actor->GetLifeSpan() != 0.f && Actor->GetLifeSpan() < 1.f))

#define GET_LINKER_COMPONENT(Owner, LinkerComponent) \
{ \
UActorComponent* TempActorLinkerComp = Owner->GetComponentByClass(ULinkerComponent::StaticClass()); \
if (TempActorLinkerComp) \
{ \
LinkerComponent = Cast<ULinkerComponent>(TempActorLinkerComp); \
} \
else \
{ \
LinkerComponent = nullptr; \
} \
}
#define DEBUG_JSON JsonObject->SetBoolField(TEXT("PrintJson"), true);
#define GET_DEFAULT_QUALITY_MODIFIER(Quality) (Quality == EItemQuality::Adept ? GDefault_Adept_Mod : Quality == EItemQuality::Arcane ? GDefault_Arcane_Mod : GDefault_Eldar_Mod)
#define GAME_STATE AFarmionGameState* GameState = GetWorld()->GetGameState<AFarmionGameState>(); check(GameState)
