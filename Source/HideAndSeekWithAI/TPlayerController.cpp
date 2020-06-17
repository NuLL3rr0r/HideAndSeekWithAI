#include "TPlayerController.h"
#include "HideAndSeekWithAI.h"

#include <Kismet/GameplayStatics.h>
#include <Templates/Casts.h>

#include "TGameState.h"
#include "TLog.h"
#include "TPickup.h"
#include "TPlayerCharacter.h"

ATPlayerController::ATPlayerController(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    MaxThrowingAccumulationTime = 2.0f;
    MinThrowingDistance = 100.0f;
    MaxThrowingDistance = 1500.0f;
    ThrowingArc = 0.75;
}

void ATPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsThrowingItem)
    {
        ThrowingAccumulatedTime += DeltaSeconds;
    }
}

void ATPlayerController::OnInputAddCameraYaw(const float AxisValue)
{
    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetCharacter());
    checkf(PlayerCharacter, TEXT("FATAL: no player character is assigned!"));

    if (!InputEnabled())
    {
        return;
    }

    PlayerCharacter->AddCameraYaw(AxisValue);
}

void ATPlayerController::OnInputAddCameraPitch(const float AxisValue)
{
    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetCharacter());
    checkf(PlayerCharacter, TEXT("FATAL: no player character is assigned!"));

    if (!InputEnabled())
    {
        return;
    }

    PlayerCharacter->AddCameraPitch(AxisValue);
}

void ATPlayerController::OnInputMoveForward(const float AxisValue)
{
    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetCharacter());
    checkf(PlayerCharacter, TEXT("FATAL: no player character is assigned!"));

    if (!InputEnabled())
    {
        return;
    }

    PlayerCharacter->MoveForward(AxisValue);
}

void ATPlayerController::OnInputMoveRight(const float AxisValue)
{
    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetCharacter());
    checkf(PlayerCharacter, TEXT("FATAL: no player character is assigned!"));

    if (!InputEnabled())
    {
        return;
    }

    PlayerCharacter->MoveRight(AxisValue);
}

void ATPlayerController::OnInputLMBPressed()
{
    TLOG_INPUT_LOG(TLOG_KEY_INPUT, TEXT("LMB pressed!"));

    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetCharacter());
    checkf(PlayerCharacter, TEXT("FATAL: no player character is assigned!"));

    if (PlayerCharacter->HasAnyItems())
    {
        if (!bIsThrowingItem)
        {
            ThrowingAccumulatedTime = 0.0f;
            bIsThrowingItem = true;
        }

        return;
    }

    UWorld* World = this->GetWorld();
    ATGameState *GameState = Cast<ATGameState>(
                UGameplayStatics::GetGameState(World));
    checkf(GameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    if (PlayerCharacter->CanPickupItems() && GameState->IsPickupAvailable())
    {
        PlayerCharacter->PickupItem(GameState->GetAvailablePickup());
    }
}

void ATPlayerController::OnInputLMBReleased()
{
    TLOG_INPUT_LOG(TLOG_KEY_INPUT, TEXT("LMB released!"));

    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetCharacter());
    checkf(PlayerCharacter, TEXT("FATAL: no player character is assigned!"));

    if (PlayerCharacter->HasAnyItems() && bIsThrowingItem)
    {
        FVector Impulse(SuggestThrowingLaunchImpulse());
        PlayerCharacter->ThrowItem(Impulse);

        ThrowingAccumulatedTime = 0.0f;
        bIsThrowingItem = false;
    }
}

void ATPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    checkf(InputComponent, TEXT("FATAL: the input component has not been"
                                " initialized!"));

    InputComponent->BindAxis(
                "AddCameraYaw",
                this,
                &ATPlayerController::OnInputAddCameraYaw);
    InputComponent->BindAxis(
                "AddCameraPitch",
                this,
                &ATPlayerController::OnInputAddCameraPitch);

    InputComponent->BindAxis(
                "MoveForward",
                this,
                &ATPlayerController::OnInputMoveForward);
    InputComponent->BindAxis(
                "MoveRight",
                this,
                &ATPlayerController::OnInputMoveRight);

    InputComponent->BindAction(
                "LMB", EInputEvent::IE_Pressed,
                this, &ATPlayerController::OnInputLMBPressed);
    InputComponent->BindAction(
                "LMB", EInputEvent::IE_Released,
                this, &ATPlayerController::OnInputLMBReleased);
}

bool ATPlayerController::IsGameOnGoing() const
{
    UWorld* World = this->GetWorld();
    ATGameState *GameState = Cast<ATGameState>(
                UGameplayStatics::GetGameState(World));
    checkf(GameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    return GameState->IsGameOnGoing();
}

FVector ATPlayerController::SuggestThrowingLaunchImpulse() const
{
    FVector LaunchImpulse(FVector::ZeroVector);

    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetCharacter());
    checkf(PlayerCharacter, TEXT("FATAL: no player character is assigned!"));

    ATPickup* Item = PlayerCharacter->GetCarryingItem();

    if (!Item)
    {
        return LaunchImpulse;
    }

    FVector CameraLocation;
    FRotator CameraRotation;
    GetPlayerViewPoint(CameraLocation, CameraRotation);

    const FVector& ItemLocation(Item->GetActorLocation());

    const FVector ViewPoint(CameraRotation.Vector().GetSafeNormal());
    const float Distance = GetEstimatedThrowingDistance();
    const FVector TargetLocation(ItemLocation + (ViewPoint * Distance));

    UGameplayStatics::SuggestProjectileVelocity_CustomArc(
                this->GetWorld(), LaunchImpulse, ItemLocation, TargetLocation,
                0, ThrowingArc);

    return LaunchImpulse;
}
