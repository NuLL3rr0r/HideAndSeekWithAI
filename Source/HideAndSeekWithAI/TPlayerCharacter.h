#pragma once

#include <CoreTypes.h>
#include <UObject/ObjectMacros.h>

#include "TCharacter.h"

#include "TPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

class ATPickup;

UCLASS(BlueprintType, config=Game)
class HIDEANDSEEKWITHAI_API ATPlayerCharacter : public ATCharacter
{
    GENERATED_UCLASS_BODY()

protected:
    /** Camera boom which positions the first-person camera in an appropriate
     *  position. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    /** The first-person camera. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* Camera;

    /** The base rate for looking up or down. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float BaseLookUpDownRate;

    /** The base rate for turning around */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float BaseTurnRate;

    /** The minimum allowed camera pitch. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float MinCameraPitch;

    /** The maximum allowed cmaera pitch. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float MaxCameraPitch;

private:
    /** The camera input cache used to cache the camera-related controller
     *  input. */
    UPROPERTY(Transient)
    FVector2D CameraInput;

    /** The movement input cache used to cache the movement-related controller
     *  input. */
    UPROPERTY(Transient)
    FVector2D MovementInput;

public:
    virtual void Tick(float DeltaSeconds) override;

public:
    /** (In/de)creases camera yaw. */
    void AddCameraYaw(const float AxisValue);

    /** (In/de)creases camera pitch. */
    void AddCameraPitch(const float AxisValue);

    /** Moves forward or backward. */
    void MoveForward(const float AxisValue);

    /** Moves to right or left. */
    void MoveRight(const float AxisValue);

    /** Makes the player character throw the currently picked up item (if
     *  there's any) in the desired direction. */
    void ThrowItem(const FVector& Impulse);

private:
    /** Updates camera in an smooth way based the camera input cache values. */
    void UpdateCamera(const float DeltaSeconds);

    /** Updates movement in an smooth way based the movement input cache values. */
    void UpdateMovement(const float DeltaSeconds);
};
