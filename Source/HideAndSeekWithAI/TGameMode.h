#pragma once

#include <CoreTypes.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/GameMode.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>

#include "TGameMode.generated.h"

class ATAICharacter;
class ATPickup;
class ATPlayerCharacter;
class ATObstacle;
class ATSpawnArea;

/** The game's main game mode. */
UCLASS()
class HIDEANDSEEKWITHAI_API ATGameMode : public AGameMode
{
    GENERATED_UCLASS_BODY()

protected:
    /** Bot class to spawn. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    TSubclassOf<ATAICharacter> BotClass;

    /** Number of bots to spawns. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    uint8 NumberOfBots;

    /** Obstacle class to spawn. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    TSubclassOf<ATObstacle> ObstacleClass;

    /** Number of obstacles to spawns. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    uint8 NumberOfObstacles;

    /** Pickup class to spawn. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    TSubclassOf<ATPickup> PickupClass;

    /** Number of pickups to spawns. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    uint8 NumberOfPickups;

    /** Seconds before a new match starts after winning or losing the game. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    uint8 MatchRestartInterval;

    /** Holds the number of ticks (passed seconds) for the match restart
     *  timer. */
    UPROPERTY(Transient)
    uint8 MatchRestartTimerTicks;

private:
    /** The timer used to restart the match automatically when the game ends. */
    FTimerHandle MatchRestartTimer;

public:
    /** If a pick item is near the player this function gets called by the
     *  pickup item in order to notify the game to show a message to the
     *  player. */
    void NotifyPickupAvailable(ATPickup* Pickup);

    /** If the player get caught this function is getting called by the AI
     * controller responsible for catching the player. */
    void PlayerCaught(ATPlayerCharacter* PlayerCharacter);

    /** If the player enters the win spot trigger this function gets call by
     *  the win spot instance. */
    void PlayerWon(ATPlayerCharacter* PlayerCharacter);

    /** Returns the match restart elapsed time in order to be used in the game's
     *  HUD. */
    FORCEINLINE uint8 GetMatchRestartElapsedTime() const
    {
        return MatchRestartTimerTicks;
    }

    /** Returns the match restart remaning time in order to be used in the
     *  game's HUD. */
    FORCEINLINE uint8 GetMatchRestartRemainingTime() const
    {
        return MatchRestartInterval - MatchRestartTimerTicks;
    }

protected:
    virtual void BeginPlay() override;

    /** This function only gets called during the match restart window. */
    void OnMatchRestartTimerTick();

    /** Restarts the match and resets everything. */
    void RestartMatch();

private:
    /** Find the spawn area of the current level in order to spawn obstacles,
     *  pickup items, and bots. */
    const ATSpawnArea* FindSpawnArea() const;

    /** Spawns all the obstacles. */
    void SpawnObstacles(const ATSpawnArea* SpawnArea);

    /** Spawns all the pickup items. */
    void SpawnPickups(const ATSpawnArea* SpawnArea);

    /** Spawns all the bots. */
    void SpawnBots(const ATSpawnArea* SpawnArea);
};
