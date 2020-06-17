#pragma once

#include <CoreTypes.h>
#include <GameFramework/GameState.h>
#include <UObject/ObjectMacros.h>

#include "HideAndSeekWithAI.h"

#include "TGameState.generated.h"

class ATPickup;

/** The game's game state used to store the game data. */
UCLASS()
class HIDEANDSEEKWITHAI_API ATGameState : public AGameState
{
    GENERATED_UCLASS_BODY()

protected:
    UPROPERTY(Transient)
    EMatchResults MatchResults;

    /** The pickup instance available to the current player. In an ideal
     *  situation this belongs to player state not game mode. But, since this is
     *  not a multiplayer game and a prototype, for the sake of simplicity we
     *  keep it here. */
    UPROPERTY(Transient)
    ATPickup* AvailablePickup;

public:
    /** Returns the match result. */
    FORCEINLINE const EMatchResults& GetMatchResults() const
    {
        return MatchResults;
    }

    /** Determines whether the game ongoing or has it been ended. */
    FORCEINLINE bool IsGameOnGoing() const
    {
        return (MatchResults == EMatchResults::OnGoing);
    }

    /** Determines whether the player has been caught by one of the bots. */
    FORCEINLINE bool IsPlayerCaught() const
    {
        return (MatchResults == EMatchResults::Caught);
    }

    /** Determines whether the player has won or not. */
    FORCEINLINE bool IsGameWon() const
    {
        return (MatchResults == EMatchResults::Won);
    }

    /** Updates the match results. */
    FORCEINLINE void UpdateMatchResults(const EMatchResults& Results)
    {
        MatchResults = Results;
    }

    /** Is any pickup item available in player's nearby? */
    FORCEINLINE bool IsPickupAvailable() const
    {
        return (AvailablePickup != nullptr);
    }

    /** Returns the available pickup item instance if they are available near
     *  the player. */
    FORCEINLINE ATPickup* GetAvailablePickup() const
    {
        return AvailablePickup;
    }

    /** Used to save the pickup item near the player in order to get picked up
     * if the player wishes to. */
    FORCEINLINE void SetAvailablePickup(ATPickup* Pickup)
    {
        AvailablePickup = Pickup;
    }
};
