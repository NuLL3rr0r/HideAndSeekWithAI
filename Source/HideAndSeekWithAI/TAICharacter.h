#pragma once

#include <UObject/ObjectMacros.h>

#include "HideAndSeekWithAI.h"
#include "TCharacter.h"

#include "TAICharacter.generated.h"

class UBoxComponent;
class UWidgetComponent;

class UTTeamComponent;

/** Base AI character used for all bots in the game. */
UCLASS(BlueprintType, config=Game)
class HIDEANDSEEKWITHAI_API ATAICharacter : public ATCharacter
{
    GENERATED_UCLASS_BODY()

public:
    /** This delegate fires when the bot gets touched by another actor. */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnTouchedByActorDelegate, AActor*, InstigatorActor );

public:
    /** This delegate fires when the bot gets touched by another actor. */
    UPROPERTY(BlueprintAssignable, Category = "Perception")
    FOnTouchedByActorDelegate OnTouchedByActor;

protected:
    /** This box trigger is used in order to detect touch events. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    UBoxComponent* TouchSenseTrigger;

    /** This in game UMG-based widget is used to indicate the current state of
     *  the bot. e.g., suspicious, alerted, invesitgating, etc. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    UWidgetComponent* AIStateWidget;

private:
    /** Holds the original bot's spawn point. */
    UPROPERTY(Transient)
    FVector SpawnPoint;

    /** Holds the current state.of the AI. */
    UPROPERTY(Transient)
    EAIState AIState;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** When bot gets touched by another actor this event fires. */
    UFUNCTION()
    virtual void OnOverlapBegins(
            UPrimitiveComponent* overlappedComponent,
            AActor* otherActor,
            UPrimitiveComponent* otherComp,
            int32 otherBodyIndex,
            bool bFromSweep,
            const FHitResult & sweepResult);

public:
    /** Returns the original spawn point of the bot. */
    FORCEINLINE const FVector& GetSpawnPoint() const
    {
        return SpawnPoint;
    }

    /** Sets the bot spawn point. */
    FORCEINLINE void SetSpawnPoint(const FVector& Location)
    {
        SpawnPoint = Location;
    }

    /** Returns the bot's current state. */
    FORCEINLINE const EAIState& GetAIState() const
    {
        return AIState;
    }

    /** Sets the bot's current state. */
    FORCEINLINE void SetAIState(const EAIState& State)
    {
        AIState = State;
    }
};
