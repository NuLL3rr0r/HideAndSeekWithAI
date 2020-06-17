#include "TPlayerCharacter.h"
#include "HideAndSeekWithAI.h"

#include <Camera/CameraComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Math/Axis.h>
#include <Math/UnrealMathUtility.h>

#include "TLog.h"
#include "TPickup.h"
#include "TPlayerCharacterMovementComponent.h"
#include "TPlayerController.h"
#include "TTeamComponent.h"

static constexpr uint64 TLOG_KEY_INPUT_ADD_CAMERA_YAW = TLOG_KEY_INPUT + 1;
static constexpr uint64 TLOG_KEY_INPUT_ADD_CAMERA_PITCH = TLOG_KEY_INPUT_ADD_CAMERA_YAW + 1;
static constexpr uint64 TLOG_KEY_INPUT_MOVE_FORWARD = TLOG_KEY_INPUT_ADD_CAMERA_PITCH + 1;
static constexpr uint64 TLOG_KEY_INPUT_MOVE_RIGHT = TLOG_KEY_INPUT_MOVE_FORWARD + 1;

ATPlayerCharacter::ATPlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass
            <UTPlayerCharacterMovementComponent>
            (ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    UCapsuleComponent* Capsule = GetCapsuleComponent();
    if (Capsule)
    {
        Capsule->InitCapsuleSize(65.0f, 65.0f);
    }

    GetCharacterMovement()->MaxWalkSpeed = 900.0f;

    CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(
                this, TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetRelativeLocationAndRotation(
                FVector(50.0f, 0.0f, BaseEyeHeight * 0.5f),
                FRotator(0.0f, 0.0f, 0.0f));
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->TargetArmLength = 0.0f;
    CameraBoom->bEnableCameraLag = false;
    CameraBoom->bEnableCameraRotationLag = false;
    CameraBoom->bUseCameraLagSubstepping = true;
    CameraBoom->CameraLagSpeed = 0.0f;
    CameraBoom->bUseAttachParentBound = true;
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;
    CameraBoom->SetCanEverAffectNavigation(false);

    Camera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(
                this, TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom);
    Camera->SetRelativeLocationAndRotation(
                FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
    Camera->FieldOfView = 70.0f;
    Camera->bUsePawnControlRotation = false;
    Camera->SetCanEverAffectNavigation(false);

    BaseLookUpDownRate = 70.0f;
    BaseTurnRate = 50.0f;
    MinCameraPitch = -80.0f;
    MaxCameraPitch = 40.0f;

    Team->SetTeamNumber(0);
}

void ATPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    APlayerController* PlayerController = Cast<APlayerController>(
                GetController());
    checkf(PlayerController, TEXT("FATAL: no player controller is"
                                  " assigned!"));

    if (PlayerController->InputEnabled())
    {
        UpdateCamera(DeltaSeconds);
        UpdateMovement(DeltaSeconds);
    }
}

void ATPlayerCharacter::AddCameraYaw(const float AxisValue)
{
    checkf(GetController(), TEXT("FATAL: no player controller is assigned!"));
    CameraInput.X = AxisValue;
}

void ATPlayerCharacter::AddCameraPitch(const float AxisValue)
{
    checkf(GetController(), TEXT("FATAL: no player controller is assigned!"));
    CameraInput.Y = AxisValue;
}

void ATPlayerCharacter::MoveForward(const float AxisValue)
{
    checkf(GetController(), TEXT("FATAL: no player controller is assigned!"));
    MovementInput.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void ATPlayerCharacter::MoveRight(const float AxisValue)
{
    checkf(GetController(), TEXT("FATAL: no player controller is assigned!"));
    MovementInput.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void ATPlayerCharacter::ThrowItem(const FVector& Impulse)
{
    if (HasAnyItems())
    {
        Item->DetachFromCharacter(this);
        Item->GetMesh()->AddImpulse(Impulse, NAME_None, true);
        Item = nullptr;
    }
}

void ATPlayerCharacter::UpdateCamera(const float DeltaSeconds)
{
    // Set camera yaw
    if (!FMath::IsNearlyZero(CameraInput.X))
    {
        float Yaw = CameraInput.X * BaseTurnRate * DeltaSeconds;

        ATPlayerController* PlayerController = Cast<ATPlayerController>(
                    GetController());
        checkf(PlayerController, TEXT("FATAL: not HideAndSeekWithAI's player"
                                      " controller"));

        TLOG_INPUT_LOG(TLOG_KEY_INPUT_ADD_CAMERA_YAW, "Adding Camera Yaw",
                       Yaw);

        PlayerController->AddYawInput(Yaw);
    }

    // Set camera pitch
    if (!FMath::IsNearlyZero(CameraInput.Y))
    {
        FRotator NewRotation = CameraBoom->GetComponentRotation();
        float NewPitch = NewRotation.Pitch
                - (CameraInput.Y * BaseLookUpDownRate * DeltaSeconds);
        NewPitch = FMath::ClampAngle(NewPitch, MinCameraPitch, MaxCameraPitch);
        NewRotation.Pitch = NewPitch;

        TLOG_INPUT_LOG(TLOG_KEY_INPUT_ADD_CAMERA_PITCH, "New Camera Pitch",
                       NewPitch);

        CameraBoom->SetWorldRotation(NewRotation);
    }
}

void ATPlayerCharacter::UpdateMovement(const float DeltaSeconds)
{
    if (MovementInput.IsNearlyZero())
    {
        return;
    }

    auto Move = [this](EAxis::Type Axis, float Value)
    {
        if (FMath::IsNearlyZero(Value))
        {
            return;
        }

        const FRotator Rotation = GetActorRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(Axis);

        AddMovementInput(Direction, Value, false);
    };

    // Move forward / backward
    if (!FMath::IsNearlyZero(MovementInput.X))
    {
        TLOG_INPUT_LOG(TLOG_KEY_INPUT_MOVE_FORWARD, "Move Forward",
                       MovementInput.X, GetActorLocation());
        Move(EAxis::X, MovementInput.X);
    }

    // Move right / left
    if (!FMath::IsNearlyZero(MovementInput.Y))
    {
        TLOG_INPUT_LOG(TLOG_KEY_INPUT_MOVE_RIGHT, "Move Right",
                       MovementInput.Y, GetActorLocation());
        Move(EAxis::Y, MovementInput.Y);
    }
}
