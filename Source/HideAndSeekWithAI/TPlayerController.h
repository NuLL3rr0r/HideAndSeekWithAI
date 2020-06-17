#pragma once

#include <CoreTypes.h>
#include <GameFramework/PlayerController.h>
#include <Math/UnrealMathUtility.h>
#include <UObject/ObjectMacros.h>

#include "TPlayerController.generated.h"

/** The player's controller class. */
UCLASS(Abstract, BlueprintType, config=Game)
class HIDEANDSEEKWITHAI_API ATPlayerController : public APlayerController
{
    GENERATED_UCLASS_BODY()

protected:
    /** Maximum time player is allowed to hold down the throw button in order to
     *  decrease the throwing distance. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Input")
    float MaxThrowingAccumulationTime;

    /** Minimum item throwing distance. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Input")
    float MinThrowingDistance;

    /** Maximum item throwing distance. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Input")
    float MaxThrowingDistance;

    /** The item throwing arc. Change height of arc between 0.0 to 1.0 where 0.5
     *  is the default medium arc, 0 is up, and 1 is directly toward the target
     *  location.*/
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Input")
    float ThrowingArc;

private:
    /** Indicates whether the player is currently busy throwing an item or
     *  not. */
    UPROPERTY(Transient)
    uint8 bIsThrowingItem : 1;

    /** The time that player did hold the throw button down. */
    UPROPERTY(Transient)
    float ThrowingAccumulatedTime;

public:
    virtual void Tick(float DeltaSeconds) override;

protected:
    /** Controller input for looking around.  */
    void OnInputAddCameraYaw(const float AxisValue);

    /** Controller input for looking up or down.  */
    void OnInputAddCameraPitch(const float AxisValue);

    /** Controller input for moving forward or backward.  */
    void OnInputMoveForward(const float AxisValue);

    /** Controller input for moving to right or left.  */
    void OnInputMoveRight(const float AxisValue);

    /** Controller input when the left mouse button is pressed. */
    void OnInputLMBPressed();

    /** Controller input when the left mouse button is released. */
    void OnInputLMBReleased();

public:
    /** Returns whether the player is currently busy throwing an item or not. */
    FORCEINLINE bool IsThrowingItem() const
    {
        return bIsThrowingItem;
    }

    /** Returns the time that player did hold the throw button down. */
    FORCEINLINE float GetThrowingAccumulatedTime() const
    {
        return FMath::Clamp(ThrowingAccumulatedTime,
                            0.0f, MaxThrowingAccumulationTime);
    }

    /** Returns the item throwing power based on input from the player. */
    FORCEINLINE float GetThrowingPower() const
    {
        return FMath::Clamp(
                    (GetThrowingAccumulatedTime() / MaxThrowingAccumulationTime)
                    * 100.0f, 0.0f, 100.0f);
    }

    /** Estimates the distance the item will be thrown based on the time the
     *  player holds down the throw item button. */
    FORCEINLINE float GetEstimatedThrowingDistance() const
    {
        return FMath::Clamp((GetThrowingPower() / 100.0f) * MaxThrowingDistance,
                    MinThrowingDistance, MaxThrowingDistance);
    }

protected:
    virtual void SetupInputComponent() override;

private:
    /** Determines whether the game is ongoing or it has been ended? */
    bool IsGameOnGoing() const;

    /** Calculations for the best launch velocity in order to land the item
     *  projectile based on the input from the player (the amount of time the
     *  player holds down the throw item button). */
    FVector SuggestThrowingLaunchImpulse() const;
};
