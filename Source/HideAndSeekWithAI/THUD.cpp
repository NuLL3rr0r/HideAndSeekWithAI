#include "THUD.h"
#include "HideAndSeekWithAI.h"

#include <Kismet/GameplayStatics.h>
#include <Widgets/SWeakWidget.h>

#include "TGameMode.h"
#include "TGameState.h"
#include "TGameWidget.h"
#include "TPlayerCharacter.h"
#include "TPlayerController.h"

ATHUD::ATHUD(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

void ATHUD::BeginPlay()
{
    Super::BeginPlay();

    GameWidget = SNew(STGameWidget).OwnerHUD(this);

    GEngine->GameViewport->AddViewportWidgetContent(
        SNew(SWeakWidget)
        .PossiblyNullContent(GameWidget.ToSharedRef())
    );

    GameWidget->SetVisibility(EVisibility::Visible);
}

FText ATHUD::GetMatchResultsText() const
{
    ATGameState *GameState = Cast<ATGameState>(
                UGameplayStatics::GetGameState(GetWorld()));
    checkf(GameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    switch (GameState->GetMatchResults())
    {
    case EMatchResults::OnGoing:
        return FText::FromString(FString("Don't Get Caught Challenge!"));
    case EMatchResults::Caught:
        return FText::FromString(FString("You lost!"));
    case EMatchResults::Won:
        return FText::FromString(FString("You won!"));
    }

    return FText::FromString(FString(""));
}

FText ATHUD::GetGameMessageText() const
{
    ATGameState *GameState = Cast<ATGameState>(
                UGameplayStatics::GetGameState(GetWorld()));
    checkf(GameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    ATGameMode *GameMode = Cast<ATGameMode>(
                UGameplayStatics::GetGameMode(GetWorld()));
    checkf(GameMode, TEXT("FATAL: not HideAndSeekWithAI's game mode!"));

    if (GameState->GetMatchResults() != EMatchResults::OnGoing)
    {
        FString Message(FString::Printf(
                            TEXT("The match will restart in %d seconds..."),
                            GameMode->GetMatchRestartRemainingTime()));
        return FText::FromString(Message);
    }

    if (!PlayerOwner)
    {
        return FText::FromString("");
    }

    ATPlayerController *PlayerController = Cast<ATPlayerController>(
                PlayerOwner);
    if (!PlayerController)
    {
        return FText::FromString("");
    }

    ATPlayerCharacter *PlayerCharacter = Cast<ATPlayerCharacter>(
                PlayerController->GetCharacter());
    if (!PlayerCharacter)
    {
        return FText::FromString("");
    }

    if (PlayerCharacter->HasAnyItems())
    {
        if (PlayerController->IsThrowingItem())
        {
            return FText::FromString(
                        FString::Printf(
                            TEXT("Thworing Time: %.2f; Power: %.0f%%; Distance: %.1fcm;"),
                            PlayerController->GetThrowingAccumulatedTime(),
                            PlayerController->GetThrowingPower(),
                            PlayerController->GetEstimatedThrowingDistance()));
        }
        else
        {
            return FText::FromString(
                        FString("In order to distract the guards,"
                                " throw your picked up item!"));
        }
    }
    else if (GameState->IsPickupAvailable())
    {
        return FText::FromString(
                    FString("Pickup Available! Press LMB to pick it up!"));
    }
    else
    {
        return FText::FromString(FString(""));
    }
}
