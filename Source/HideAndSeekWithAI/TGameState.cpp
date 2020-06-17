#include "TGameState.h"
#include "HideAndSeekWithAI.h"

#include "TPickup.h"

ATGameState::ATGameState(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AvailablePickup = nullptr;
}
