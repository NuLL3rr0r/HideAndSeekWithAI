#include "TGameMode.h"
#include "HideAndSeekWithAI.h"

#include <Components/BoxComponent.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Templates/Casts.h>
#include <TimerManager.h>
#include <UObject/Class.h>
#include <UObject/ConstructorHelpers.h>

#include "TAICharacter.h"
#include "TAIController.h"
#include "TGameInstance.h"
#include "TGameState.h"
#include "TLog.h"
#include "TObstacle.h"
#include "TPickup.h"
#include "TPlayerCharacter.h"
#include "TPlayerController.h"
#include "TSpawnArea.h"

static constexpr uint8 MAX_SPAWN_RETRIES = static_cast<uint8>(100);

static constexpr uint64 TLOG_KEY_GENERIC_BOTS = TLOG_KEY_GENERIC + 1;
static constexpr uint64 TLOG_KEY_GENERIC_OBSTACLES = TLOG_KEY_GENERIC_BOTS + 1;
static constexpr uint64 TLOG_KEY_GENERIC_PICKUPS = TLOG_KEY_GENERIC_OBSTACLES + 1;

ATGameMode::ATGameMode(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UClass> MyGameState(
                TEXT("Class'/Game/HideAndSeekWithAI/Core/"
                     "BP_GameState.BP_GameState_C'"));
    if (MyGameState.Succeeded() && MyGameState.Object)
    {
        GameStateClass = MyGameState.Object;
    }

    static ConstructorHelpers::FObjectFinder<UClass> PlayerController(
                TEXT("Class'/Game/HideAndSeekWithAI/Core/Characters/Player/"
                     "BP_PlayerController.BP_PlayerController_C'"));
    if (PlayerController.Succeeded() && PlayerController.Object)
    {
        PlayerControllerClass = PlayerController.Object;
    }

    static ConstructorHelpers::FObjectFinder<UClass> HUD(
                TEXT("Class'/Game/HideAndSeekWithAI/Core/HUDs/"
                     "BP_GameHUD.BP_GameHUD_C'"));
    if (HUD.Succeeded() && HUD.Object)
    {
        HUDClass = HUD.Object;
    }

    static ConstructorHelpers::FObjectFinder<UClass> DefaultPawn(
                TEXT("Class'/Game/HideAndSeekWithAI/Core/Characters/Player/"
                     "BP_PlayerCharacter.BP_PlayerCharacter_C'"));
    if (DefaultPawn.Succeeded() && DefaultPawn.Object)
    {
        DefaultPawnClass = DefaultPawn.Object;
    }

    static ConstructorHelpers::FObjectFinder<UClass> BotBP(
                TEXT("Class'/Game/HideAndSeekWithAI/Core/Characters/NPC/"
                     "BP_Bot.BP_Bot_C'"));
    if (BotBP.Succeeded() && BotBP.Object)
    {
        BotClass = BotBP.Object;
    }

    NumberOfBots = 6;

    static ConstructorHelpers::FObjectFinder<UClass> ObstacleBP(
                TEXT("Class'/Game/HideAndSeekWithAI/Core/Editor/"
                     "BP_Obstacle.BP_Obstacle_C'"));
    if (ObstacleBP.Succeeded() && ObstacleBP.Object)
    {
        ObstacleClass = ObstacleBP.Object;
    }

    NumberOfObstacles = 24;

    static ConstructorHelpers::FObjectFinder<UClass> PickupBP(
                TEXT("Class'/Game/HideAndSeekWithAI/Core/Editor/"
                     "BP_Pickup.BP_Pickup_C'"));
    if (PickupBP.Succeeded() && PickupBP.Object)
    {
        PickupClass = PickupBP.Object;
    }

    NumberOfPickups = 24;

    MatchRestartInterval = 10;
    MatchRestartTimerTicks = 0;
}

void ATGameMode::NotifyPickupAvailable(ATPickup *Pickup)
{
    ATGameState* MyGameState = GetGameState<ATGameState>();
    checkf(MyGameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    MyGameState->SetAvailablePickup(Pickup);
}

void ATGameMode::PlayerCaught(ATPlayerCharacter* PlayerCharacter)
{
    checkf(PlayerCharacter, TEXT("FATAL: invalid player character!"));

    ATPlayerController* PlayerController = Cast<ATPlayerController>(
            PlayerCharacter->GetController());
    checkf(PlayerController, TEXT("FATAL: invalid player controller!"));

    PlayerController->DisableInput(PlayerController);

    TLOG_PLAYER(TLOG_KEY_PLAYER, "Player got caught!");

    ATGameState* MyGameState = GetGameState<ATGameState>();
    checkf(MyGameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    MyGameState->UpdateMatchResults(EMatchResults::Caught);

    RestartMatch();
}

void ATGameMode::PlayerWon(ATPlayerCharacter* PlayerCharacter)
{
    checkf(PlayerCharacter, TEXT("FATAL: invalid player character!"));

    ATPlayerController* PlayerController = Cast<ATPlayerController>(
            PlayerCharacter->GetController());
    checkf(PlayerController, TEXT("FATAL: invalid player controller!"));

    PlayerController->DisableInput(PlayerController);

    TLOG_PLAYER(TLOG_KEY_PLAYER, "Player Won!");

    ATGameState* MyGameState = GetGameState<ATGameState>();
    checkf(MyGameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    MyGameState->UpdateMatchResults(EMatchResults::Won);

    RestartMatch();
}

void ATGameMode::BeginPlay()
{
    Super::BeginPlay();

    const ATSpawnArea* SpawnArea = FindSpawnArea();
    if (SpawnArea)
    {
        if (ObstacleClass.GetDefaultObject())
        {
            SpawnObstacles(SpawnArea);
        }
        else
        {
            TLOG_ERROR(TLOG_KEY_GENERIC_OBSTACLES,
                       "ERROR: obstacle class has not been set!");
        }

        if (PickupClass.GetDefaultObject())
        {
            SpawnPickups(SpawnArea);
        }
        else
        {
            TLOG_ERROR(TLOG_KEY_GENERIC_PICKUPS,
                       "ERROR: pickup class has not been set!");
        }

        if (BotClass.GetDefaultObject())
        {
            SpawnBots(SpawnArea);
        }
        else
        {
            TLOG_ERROR(TLOG_KEY_GENERIC_BOTS,
                       "ERROR: bot class has not been set!");
        }
    }
    else
    {
        TLOG_ERROR(TLOG_KEY_GENERIC, "ERROR: cannot find an instance of spawn"
                                     " area in the current level!")
    }

    ATGameState* MyGameState = GetGameState<ATGameState>();
    checkf(MyGameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    MyGameState->UpdateMatchResults(EMatchResults::OnGoing);
}

const ATSpawnArea* ATGameMode::FindSpawnArea() const
{
    for (TActorIterator<ATSpawnArea> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        const ATSpawnArea* SpawnArea = Cast<ATSpawnArea>(*ActorItr);
        if (SpawnArea)
        {
            return SpawnArea;
        }
    }

    return nullptr;
}

void ATGameMode::OnMatchRestartTimerTick()
{
    MatchRestartTimerTicks += 1;
    if (MatchRestartTimerTicks >= MatchRestartInterval)
    {
        if (GetWorldTimerManager().IsTimerActive(MatchRestartTimer))
        {
            GetWorldTimerManager().ClearTimer(MatchRestartTimer);
        }

        UTGameInstance* GameInstance =
                Cast<UTGameInstance>(UGameplayStatics::GetGameInstance(this));
        checkf(GameInstance, TEXT("FATAL: not HideAndSeekWithAI's game instance!"));

        GameInstance->RestartCurrentLevel();
    }
}

void ATGameMode::RestartMatch()
{
    if (GetWorldTimerManager().IsTimerActive(MatchRestartTimer))
    {
        GetWorldTimerManager().ClearTimer(MatchRestartTimer);
    }

    MatchRestartTimerTicks = 0;

    GetWorldTimerManager().SetTimer(
                MatchRestartTimer,
                this, &ATGameMode::OnMatchRestartTimerTick,
                1.0f, true, -1.0f);
}

void ATGameMode::SpawnObstacles(const ATSpawnArea* SpawnArea)
{
    checkf(SpawnArea, TEXT("FATAL: cannot find an instance of spawn area in the"
                           " current level!"));
    checkf(ObstacleClass.GetDefaultObject(),
           TEXT("FATAL: obstacle class has not been set!"));

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

    FVector Origin(SpawnArea->GetActorLocation());
    FVector Bounds(SpawnArea->GetArea()->GetScaledBoxExtent());

    uint8 Count = 0;
    uint8 SpawnRetries = 0;

    while (Count < NumberOfObstacles) {
        const FVector SpawnLocation(
                    UKismetMathLibrary::RandomPointInBoundingBox(
                        Origin, Bounds));
        const FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

        ATObstacle* Obstacle = this->GetWorld()->SpawnActor<ATObstacle>(
                    ObstacleClass.GetDefaultObject()->GetClass(),
                    SpawnLocation,
                    SpawnRotation,
                    SpawnInfo);

        if (Obstacle)
        {
            ++Count;
        }
        else
        {
            ++SpawnRetries;
            if (SpawnRetries == 100)
            {
                if (Count == 0)
                {
                    TLOG_ERROR(TLOG_KEY_GENERIC_OBSTACLES,
                               "ERROR: failed to spawn any obstacles!");
                }
                else
                {
                    TLOG_ERROR(TLOG_KEY_GENERIC_OBSTACLES,
                               "ERROR: failed to spawn some obstacles!");
                }

                break;
            }
        }
    }
}

void ATGameMode::SpawnPickups(const ATSpawnArea* SpawnArea)
{
    checkf(SpawnArea, TEXT("FATAL: cannot find an instance of spawn area in the"
                           " current level!"));
    checkf(PickupClass.GetDefaultObject(),
           TEXT("FATAL: obstacle class has not been set!"));

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

    FVector Origin(SpawnArea->GetActorLocation());
    FVector Bounds(SpawnArea->GetArea()->GetScaledBoxExtent());

    uint8 Count = 0;
    uint8 SpawnRetries = 0;

    while (Count < NumberOfPickups) {
        const FVector SpawnLocation(
                    UKismetMathLibrary::RandomPointInBoundingBox(
                        Origin, Bounds));
        const FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

        ATPickup* Pickup = this->GetWorld()->SpawnActor<ATPickup>(
                    PickupClass.GetDefaultObject()->GetClass(),
                    SpawnLocation,
                    SpawnRotation,
                    SpawnInfo);

        if (Pickup)
        {
            Pickup->SetSpawnPoint(SpawnLocation);
            ++Count;
        }
        else
        {
            ++SpawnRetries;
            if (SpawnRetries == 100)
            {
                if (Count == 0)
                {
                    TLOG_ERROR(TLOG_KEY_GENERIC_PICKUPS,
                               "ERROR: failed to spawn any pickups!");
                }
                else
                {
                    TLOG_ERROR(TLOG_KEY_GENERIC_PICKUPS,
                               "ERROR: failed to spawn some pickups!");
                }

                break;
            }
        }
    }
}

void ATGameMode::SpawnBots(const ATSpawnArea* SpawnArea)
{
    checkf(SpawnArea, TEXT("FATAL: cannot find an instance of spawn area in the"
                           "current level!"));
    checkf(BotClass.GetDefaultObject(),
           TEXT("FATAL: obstacle class has not been set!"));

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

    FVector Origin(SpawnArea->GetActorLocation());
    FVector Bounds(SpawnArea->GetArea()->GetScaledBoxExtent());

    uint8 Count = 0;
    uint8 SpawnRetries = 0;

    while (Count < NumberOfBots) {
        const FVector SpawnLocation(
                    UKismetMathLibrary::RandomPointInBoundingBox(
                        Origin, Bounds));
        const FRotator SpawnRotation(0.0f, FMath::FRand() * 360.0f, 0.0f);

        ATAICharacter* Bot = this->GetWorld()->SpawnActor<ATAICharacter>(
                    BotClass.GetDefaultObject()->GetClass(),
                    SpawnLocation,
                    SpawnRotation,
                    SpawnInfo);

        if (Bot)
        {
            Bot->SetSpawnPoint(SpawnLocation);
            Bot->SpawnDefaultController();

            /** According to the design
             Expected gameplay: Player placed on start spot. Bots randomly
             spawned on the map in such a way that provides a safe start
             position for the player. If the start position isn't safe - restart
             the entire level automatically.
            */
            ATAIController* Controller = Cast<ATAIController>(Bot->GetController());
            checkf(Controller, TEXT("FATAL: not HideAndSeekWithAI's AI"
                                    " controller!"));
            if (!Controller->IsPlayerInSafeDistance() || Controller->IsPlayerInSight())
            {
                UTGameInstance* GameInstance =
                        Cast<UTGameInstance>(UGameplayStatics::GetGameInstance(this));
                checkf(GameInstance, TEXT("FATAL: not HideAndSeekWithAI's game"
                                          " instance!"));
                GameInstance->RestartCurrentLevel();
            }

            ++Count;
        }
        else
        {
            ++SpawnRetries;
            if (SpawnRetries == 100)
            {
                if (Count == 0)
                {
                    TLOG_ERROR(TLOG_KEY_GENERIC_BOTS,
                               "ERROR: failed to spawn any bots!");
                }
                else
                {
                    TLOG_ERROR(TLOG_KEY_GENERIC_BOTS,
                               "ERROR: failed to spawn some bots!");
                }

                break;
            }
        }
    }
}
