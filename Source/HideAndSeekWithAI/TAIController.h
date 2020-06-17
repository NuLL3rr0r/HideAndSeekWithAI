#pragma once

#include <AIController.h>
#include <Containers/Array.h>
#include <GameFramework/Pawn.h>
#include <Math/Color.h>
#include <Perception/AIPerceptionTypes.h>
#include <UObject/ObjectMacros.h>

#include "TAIController.generated.h"

class AActor;
class APawn;
class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;

class ATCharacter;
class ATPickup;

/** Base AI controller used for all bots in the game. */
UCLASS()
class HIDEANDSEEKWITHAI_API ATAIController : public AAIController
{
    GENERATED_UCLASS_BODY()

protected:
    /** The perception component of the bot used to detect sight and hearing
     * stimulis. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* Perception;

    /** The horizontal field of view angle ratio used for drawing the FOV
     *  isnide the game.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float FOVDrawAngleHeightRatio;

    /** The number of cone sides to draw in order to visualaze the field of
     *  view. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    int32 FOVDrawNumSides;

    /** The drawing color of the field of view. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    FLinearColor FOVDrawColor;

    /** The thickness of draw lines for field of view visualization. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float FOVDrawThickness;

    /** Base turn left/right rate for the possesed bot character. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float BaseTurnRate;

    /** Minimum idle state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float MinIdleTickInterval;

    /** Maximum idle state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float MaxIdleTickInterval;

    /** Suspicious state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float SuspiciousTickInterval;

    /** Alerted state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float AlertedTickInterval;

    /** Minimum investigation state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float MinInvestigationTickInterval;

    /** Maximum investigation state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float MaxInvestigationTickInterval;

    /** Carrying item state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float CarryingItemTickInterval;

    /** Going back state tick interval in seconds.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float GoingBackTickInterval;

    /** Minimum number of looking around times during the investigation state. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    int32 MinInvestigationTimes;

    /** Maximum number of looking around times during the investigation state. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    int32 MaxInvestigationTimes;

    /** The bot walking speed during the player chase (alerted state). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float ChasingWalkSpeed;

    /** The walking speed ratio to chasing walk speed when the bot is suspicious
     *  of something. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float SuspiciousWalkSpeedRatio;

    /** The walking speed ratio to chasing walk speed when the bot is going back
     *  to its own original spawn point. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float GoingBackWalkSpeedRatio;

    /** The walking speed ratio to chasing walk speed when the bot is carrying
     *  an item to the item's original spawn point. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float CarryingItemWalkSpeedRatio;

private:
    /** The hearing sense. */
    UPROPERTY(Transient)
    UAISenseConfig_Hearing* HearingSense;

    /** The sight sense. */
    UPROPERTY(Transient)
    UAISenseConfig_Sight* SightSense;

    /** Holds the current target pawn. */
    UPROPERTY(Transient)
    ATCharacter* TargetPawn;

    /** Holds the last seen location of the current target pawn. */
    UPROPERTY(Transient)
    FVector TargetPawnLastSeenLocation;

    /** Points to the item that made this bot suspicious or the bot is carrying. */
    UPROPERTY(Transient)
    ATPickup* TargetItem;

    /** Holds the last impact location of the the current target item that
     *  disturbed the bot. */
    UPROPERTY(Transient)
    FVector TargetItemLastHeardLocation;

    /** The desired control rotation. */
    UPROPERTY(Transient)
    FRotator TargetControlRotation;

    /** The number of ramaining investigation times. */
    UPROPERTY(Transient)
    int32 RemainingInvestigationTimes;

    /** This timer ticks only when the bot is in idle state. */
    FTimerHandle IdleTimer;

    /** This timer ticks only when the bot is in suspicious state. */
    FTimerHandle SuspiciousTimer;

    /** This timer ticks only when the bot is in alerted state. */
    FTimerHandle AlertedTimer;

    /** This timer ticks only when the bot is in investigation state. */
    FTimerHandle InvestigationTimer;

    /** This timer ticks only when the bot is in carrying item state. */
    FTimerHandle CarryingItemTimer;

    /** This timer ticks only when the bot is in going back state. */
    FTimerHandle GoingBackTimer;

protected:
    /** The event fires when the target perception gets updated. */
    UFUNCTION()
    virtual void OnTargetPerceptionUpdated(
            AActor* Actor, FAIStimulus Stimulus);

    /** This event fires when the perception component gets updated. */
    UFUNCTION()
    virtual void OnPerceptionUpdated(
            const TArray<AActor*>& UpdatedActors);

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void OnMoveCompleted(
            FAIRequestID RequestId,
            const FPathFollowingResult& Result) override;

    /** When this function ticks the bot is in idle state. */
    void OnIdleTimerTick();

    /** When this function ticks the bot is in idle state. */
    void OnSuspiciousTimerTick();

    /** When this function ticks the bot is in alerted state. */
    void OnAlertedTimerTick();

    /** When this function ticks the bot is in investigation state. */
    void OnInvestigationTick();

    /** When this function ticks the bot is in carrying item state. */
    void OnCarryingItemTick();

    /** When this function ticks the bot is in going back state. */
    void OnGoingBackTick();

    /** This event fires when the current possesed bot is touched by another
     *  actor. */
    UFUNCTION()
    virtual void OnTouchedByActor(AActor* InstigatorActor);

public:
    /** Determines whether the player is in the bot's sight or not.
     * if the player is in a safe distance the bot cannot see them. */
    bool IsPlayerInSight() const;

    /** Determines whether the player is in a safe distance from the bot or not;
     *  if the player is in a safe distance the bot cannot see them. */
    bool IsPlayerInSafeDistance() const;

protected:
    /** Sets the current target pawn to track. */
    void SetTargetPawn(ATCharacter* OtherCharacter);

    /** Sets the current target item to tack or carry. */
    void SetTargetItem(ATPickup* Pickup);

    /** Is bot in idle state? */
    bool IsIdle() const;

    /** Is bot in suspicious state? */
    bool IsSuspicious() const;

    /** Is bot in alerted state? */
    bool IsAlerted() const;

    /** Is bot in investigation state? */
    bool IsInvestigating() const;

    /** Is bot in carrying item state? */
    bool IsCarryingItem() const;

    /** Is bot in going back state? */
    bool IsGoingBack() const;

    /** Go to idle state. */
    void BeIdle();

    /** Go to suspicious state. */
    void GetSuspicious();

    /** Go to alerted state. */
    void BeAlerted();

    /** Go to investigation state. */
    void Investigate();

    /** Go to item carrying state. */
    void CarryItem();

    /** Go to going back state. */
    void GoBack();

private:
    /** Draw the bot's current FOV. */
    void DrawFOV();

    /** Update the bot's focus based on the game's situations. */
    void UpdateFocus();

    /** Set the desired control rotation to gradually turn to that rotation
     *  during the tick event. */
    void SetTargetControlRotation(const FRotator& Rotation);

    /** Make the bot turn around gradually during the tick event in order to
     *  look at a specific location. */
    void SetTargetControlRotation(const FVector& Location);

    /** Adjusts to the desired control rotation gradually. Best called during
     *  tick event. */
    void AdjustControlRotation(const float DeltaSeconds);

    /** Randomly choose a direction in order to look. */
    void DoRandomTurn();

    /** Move to a desired location. */
    EPathFollowingRequestResult::Type MoveToTargetLocation(
            const FVector& Location);

    /** Clear all the states' tick timers. */
    void ClearAllTimers();

    /** Reset the idle timer and set a new random interval. */
    void ResetIdleTimer();

    /** Reset the investigation timer and set a new random interval. */
    void ResetInvestigationTimer();

    /** Determines whether the game is ongoing or it has been ended? */
    bool IsGameOnGoing() const;
};
