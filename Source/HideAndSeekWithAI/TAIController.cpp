#include "TAIController.h"
#include "HideAndSeekWithAI.h"

#include <DrawDebugHelpers.h>
#include <EngineUtils.h>
#include <GameFramework/Actor.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Pawn.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Math/UnrealMathUtility.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AISenseConfig_Hearing.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Templates/Casts.h>
#include <TimerManager.h>

#include "TAICharacter.h"
#include "TGameMode.h"
#include "TGameState.h"
#include "TLog.h"
#include "TPickup.h"
#include "TPlayerCharacter.h"
#include "TTeamComponent.h"

static constexpr uint64 TLOG_KEY_AI_TARGET_PERCEPTION_UPDATED = TLOG_KEY_AI + 1;
static constexpr uint64 TLOG_KEY_AI_PERCEPTION_UPDATED = TLOG_KEY_AI_TARGET_PERCEPTION_UPDATED + 1;
static constexpr uint64 TLOG_KEY_AI_SET_TARGET_PAWN = TLOG_KEY_AI_PERCEPTION_UPDATED + 1;
static constexpr uint64 TLOG_KEY_AI_SET_TARGET_PICKUP = TLOG_KEY_AI_SET_TARGET_PAWN + 1;
static constexpr uint64 TLOG_KEY_AI_TOUCHED_BY_ACTOR = TLOG_KEY_AI_SET_TARGET_PICKUP + 1;
static constexpr uint64 TLOG_KEY_AI_MOVEMENT = TLOG_KEY_AI_TOUCHED_BY_ACTOR + 1;
static constexpr uint64 TLOG_KEY_AI_SIGHT_SENSE = TLOG_KEY_AI_MOVEMENT + 1;
static constexpr uint64 TLOG_KEY_AI_HEARNING_SENSE = TLOG_KEY_AI_SIGHT_SENSE + 1;

ATAIController::ATAIController(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Perception = ObjectInitializer.CreateDefaultSubobject<
            UAIPerceptionComponent>(this, FName(TEXT("Perception")));

    HearingSense = ObjectInitializer.CreateDefaultSubobject<
            UAISenseConfig_Hearing>(this, FName(TEXT("HearingSense")));
    HearingSense->HearingRange = 2000.0f;
    HearingSense->bUseLoSHearing = true;
    HearingSense->LoSHearingRange = 2000.0f;
    HearingSense->DetectionByAffiliation.bDetectEnemies = true;
    HearingSense->DetectionByAffiliation.bDetectFriendlies = true;
    HearingSense->DetectionByAffiliation.bDetectNeutrals = true;
    HearingSense->SetMaxAge(10.0f);
    Perception->ConfigureSense(*HearingSense);

    SightSense = ObjectInitializer.CreateDefaultSubobject<
            UAISenseConfig_Sight>(this, FName(TEXT("SightSense")));
    SightSense->SightRadius = 2000.0f;
    SightSense->LoseSightRadius = 2000.0f;
    SightSense->PeripheralVisionAngleDegrees = 70.0f;
    SightSense->DetectionByAffiliation.bDetectEnemies = true;
    SightSense->DetectionByAffiliation.bDetectFriendlies = true;
    SightSense->DetectionByAffiliation.bDetectNeutrals = true;
    SightSense->AutoSuccessRangeFromLastSeenLocation = -1.0f;
    SightSense->SetMaxAge(10.0f);
    Perception->ConfigureSense(*SightSense);

    Perception->SetDominantSense(SightSense->GetSenseImplementation());

    FOVDrawAngleHeightRatio = 5.0f;
    FOVDrawNumSides = 32;
    FOVDrawColor = FLinearColor::Red;
    FOVDrawThickness = 1.0f;

    BaseTurnRate = 4.0f;

    MinIdleTickInterval = 3.0f;
    MaxIdleTickInterval = 12.0f;
    SuspiciousTickInterval= 0.25f;
    AlertedTickInterval= 0.25f;
    MinInvestigationTickInterval = 0.75;
    MaxInvestigationTickInterval = 3.0;
    CarryingItemTickInterval = 0.25;
    GoingBackTickInterval = 0.25;

    MinInvestigationTimes = 2;
    MaxInvestigationTimes = 4;
    RemainingInvestigationTimes = 0;

    ChasingWalkSpeed = 1800.0f;
    SuspiciousWalkSpeedRatio = 0.5f;
    GoingBackWalkSpeedRatio = 0.2f;
    CarryingItemWalkSpeedRatio = 0.5f;

    TargetPawn = nullptr;
}

void ATAIController::OnTargetPerceptionUpdated(
        AActor* Actor, FAIStimulus Stimulus)
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    TLOG_AI_LOG(TLOG_KEY_AI_TARGET_PERCEPTION_UPDATED,
                TEXT("OnTargetPerceptionUpdated"), Cast<AActor>(AICharacter));

    if (!Stimulus.WasSuccessfullySensed())
    {
        return;
    }

    if (!Actor)
    {
        TLOG_AI_LOG(TLOG_KEY_AI_TARGET_PERCEPTION_UPDATED,
                    TEXT("OnTargetPerceptionUpdated"),
                    Cast<AActor>(AICharacter), Actor);

        return;
    }

    if (!Stimulus.Tag.GetPlainNameString().ToUpper().Contains(
                FString(T_NOISE).ToUpper()))
    { /// Visual
        if (!Actor->IsA<ATCharacter>())
        {
            return;
        }

        if (!UTTeamComponent::AreEnemies(AICharacter, Actor))
        {
            return;
        }

        ATCharacter* OtherCharacter = Cast<ATCharacter>(Actor);
        if (TargetPawn == OtherCharacter)
        {
            return;
        }

        TLOG_AI_WARNING(TLOG_KEY_AI_SIGHT_SENSE, TEXT("Sight sense"),
                        Cast<AActor>(AICharacter), Actor);

        SetTargetPawn(OtherCharacter);
    }
    else { /// Auditory
        if (TargetPawn)
        {
            return;
        }

        if (!Actor->IsA<ATPlayerCharacter>())
        {
            TLOG_AI_WARNING(TLOG_KEY_AI_SIGHT_SENSE, TEXT("Hearing actor!"),
                            Cast<AActor>(AICharacter), Actor);

            return;
        }

        FString PickupItemName(
                    Stimulus.Tag.ToString().Replace(
                        *FString::Printf(TEXT("%s_"), TEXT(T_NOISE)),
                        TEXT(""), ESearchCase::IgnoreCase));
        ATPickup* PickupItem = nullptr;

        UWorld* World = this->GetWorld();
        if (World)
        {
            for (TActorIterator<AActor> ActorIter(World); ActorIter;
                 ++ActorIter)
            {
                ATPickup* Item = Cast<ATPickup>(*ActorIter);
                if (Item)
                {
                    FString ItemName(UKismetSystemLibrary::GetDisplayName(
                                         Item));

                    TLOG_AI_WARNING(TLOG_KEY_AI_SIGHT_SENSE,
                                    TEXT("Hearing sense; comparing..."),
                                    Cast<AActor>(AICharacter),
                                    ItemName, PickupItemName);

                    if (ItemName == PickupItemName)
                    {
                        PickupItem = Item;
                        break;
                    }
                }
            }
        }

        if (!PickupItem)
        {
            TLOG_AI_WARNING(TLOG_KEY_AI_SIGHT_SENSE,
                            TEXT("Hearing sense failed to find item!"),
                            Cast<AActor>(AICharacter), Actor, PickupItemName);

            return;
        }

        TLOG_AI_WARNING(TLOG_KEY_AI_SIGHT_SENSE, TEXT("Hearing sense"),
                        Cast<AActor>(AICharacter), Actor, PickupItemName);

        SetTargetItem(PickupItem);
    }
}

void ATAIController::OnPerceptionUpdated(
        const TArray<AActor*>& UpdatedActors)
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    TLOG_AI_LOG(TLOG_KEY_AI_PERCEPTION_UPDATED, TEXT("OnPerceptionUpdated"),
                Cast<AActor>(AICharacter));

    if (!TargetPawn)
    {
        return;
    }

    for (AActor* Actor : UpdatedActors)
    {
        if (!Actor || !Actor->IsA<ATPlayerCharacter>())
        {
            continue;
        }

        FActorPerceptionBlueprintInfo Info;
        this->Perception->GetActorsPerception(Actor, Info);

        if (Info.LastSensedStimuli.Num() < 2)
        {
            continue;
        }

        const FAIStimulus& StimulusSight = Info.LastSensedStimuli[1];
        if (StimulusSight.IsActive())
        {
            continue;
        }

        const FAIStimulus& StimulusHearing = Info.LastSensedStimuli[0];
        if (StimulusHearing.IsActive())
        {
            continue;
        }

        if (TargetPawn)
        {
            SetTargetPawn(nullptr);
        }
    }
}

void ATAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ATAICharacter* AICharacter = Cast<ATAICharacter>(InPawn);
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    SetTargetControlRotation(GetControlRotation());

    Perception->OnTargetPerceptionUpdated.AddDynamic(
                this, &ATAIController::OnTargetPerceptionUpdated);
    Perception->OnPerceptionUpdated.AddDynamic(
                this, &ATAIController::OnPerceptionUpdated);

    AICharacter->OnTouchedByActor.AddDynamic(
                this, &ATAIController::OnTouchedByActor);

    BeIdle();
    ResetIdleTimer();
}

void ATAIController::OnUnPossess()
{
    Super::OnUnPossess();

    Perception->OnTargetPerceptionUpdated.RemoveDynamic(
                this, &ATAIController::OnTargetPerceptionUpdated);
    Perception->OnPerceptionUpdated.RemoveDynamic(
                this, &ATAIController::OnPerceptionUpdated);

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    if (AICharacter)
    {
        AICharacter->OnTouchedByActor.RemoveDynamic(
                        this, &ATAIController::OnTouchedByActor);
    }
}

void ATAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ATAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void ATAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    DrawFOV();

    if (TargetPawn)
    {
        SetTargetControlRotation(TargetPawn->GetActorLocation());
    }

    AdjustControlRotation(DeltaSeconds);
}

void ATAIController::OnMoveCompleted(
        FAIRequestID RequestId,
        const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestId, Result);
}

void ATAIController::OnIdleTimerTick()
{
    if (!IsIdle())
    {
        return;
    }

    DoRandomTurn();
    ResetIdleTimer();
}

void ATAIController::OnSuspiciousTimerTick()
{
    if (!IsGameOnGoing())
    {
        StopMovement();
        BeIdle();

        return;
    }

    if (!IsSuspicious())
    {
        return;
    }

    if (TargetItem && !TargetItem->IsAttachedToACharacter())
    {
        TargetItemLastHeardLocation = TargetItem->GetActorLocation();

    }
    else
    {
        SetTargetItem(nullptr);
    }

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    EPathFollowingRequestResult::Type Result =
            MoveToTargetLocation(TargetItemLastHeardLocation);

    if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        Investigate();
    }
}

void ATAIController::OnAlertedTimerTick()
{
    if (!IsGameOnGoing())
    {
        StopMovement();
        BeIdle();

        return;
    }

    if (!IsAlerted())
    {
        return;
    }

    if (TargetPawn)
    {
        TargetPawnLastSeenLocation = TargetPawn->GetActorLocation();
    }

    EPathFollowingRequestResult::Type Result =
            MoveToTargetLocation(TargetPawnLastSeenLocation);

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    /* If bot lost sight of the player, he has to reach the point where he last
     * saw player and look around there. If the player is disappeared, the bot
     * should go back to his spawn position with 20% of chasing speed. */
    if (!TargetPawn && Result == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        StopMovement();
        Investigate();
    }
}

void ATAIController::OnInvestigationTick()
{
    if (!IsGameOnGoing())
    {
        StopMovement();
        BeIdle();

        return;
    }

    if (!IsInvestigating())
    {
        return;
    }

    if (RemainingInvestigationTimes <= 0)
    {
        ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
        checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

        if (TargetItem && !TargetItem->IsAttachedToACharacter())
        {
            CarryItem();
        }
        else
        {
            GoBack();
        }

        return;
    }

    DoRandomTurn();
    --RemainingInvestigationTimes;
    ResetInvestigationTimer();
}

void ATAIController::OnCarryingItemTick()
{
    if (!IsGameOnGoing())
    {
        StopMovement();
        BeIdle();

        return;
    }

    if (!IsCarryingItem())
    {
        return;
    }

    if (!TargetItem)
    {
        return;
    }

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    EPathFollowingRequestResult::Type Result =
            MoveToTargetLocation(TargetItem->GetSpawnPoint());
    if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        SetTargetItem(nullptr);
        GoBack();
    }
}

void ATAIController::OnGoingBackTick()
{
    if (!IsGameOnGoing())
    {
        StopMovement();
        BeIdle();

        return;
    }

    if (!IsGoingBack())
    {
        return;
    }

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    EPathFollowingRequestResult::Type Result =
            MoveToTargetLocation(AICharacter->GetSpawnPoint());
    if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        BeIdle();
    }
}

void ATAIController::OnTouchedByActor(AActor* InstigatorActor)
{
    if (!InstigatorActor)
    {
        return;
    }

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    TLOG_AI_WARNING(TLOG_KEY_AI_TOUCHED_BY_ACTOR, TEXT("OnTouchedByActor"),
                    Cast<AActor>(AICharacter), InstigatorActor);

    if (TargetPawn && IsAlerted())
    {
        UWorld* World = this->GetWorld();
        ATGameMode *GameMode = Cast<ATGameMode>(
                    UGameplayStatics::GetGameMode(World));
        checkf(GameMode, TEXT("FATAL: not HideAndSeekWithAI's game mode!"));

        ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(InstigatorActor);
        if (PlayerCharacter)
        {
            GameMode->PlayerCaught(PlayerCharacter);
        }
    }
    else
    {
        if (InstigatorActor->IsA<ATCharacter>() || InstigatorActor->IsA<ATPickup>())
        {
            SetTargetControlRotation(InstigatorActor->GetActorLocation());
        }
    }
}

bool ATAIController::IsPlayerInSight() const
{
    if (TargetPawn)
    {
        return true;
    }

    ATPlayerCharacter* PlayerCharacter =
            Cast<ATPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    checkf(PlayerCharacter, TEXT("FATAL: not HideAndSeekWithAI's player character!"));

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    const float MaxSightDistance = SightSense->SightRadius;
    const FVector ViewLocation(AICharacter->GetPawnViewLocation());
    const FVector PlayerLocation(PlayerCharacter->GetActorLocation());
    const FVector PlayerDirection(PlayerLocation - ViewLocation);
    const float PlayerDistance = PlayerDirection.Size();

    if (PlayerDistance > MaxSightDistance)
    {
        return false;
    }

    const FVector ViewDirection(AICharacter->GetViewRotation().Vector());
    const FVector ViewDirectionNormal(ViewDirection.GetSafeNormal());
    const FVector PlayerDirectionNormal = PlayerDirection.GetSafeNormal();

    const float DotProduct  =
            FVector::DotProduct(ViewDirectionNormal, PlayerDirectionNormal);
    bool bLookingAtPlayerDirection = DotProduct > 0.0f;

    if (!bLookingAtPlayerDirection)
    {
        return false;
    }

    const float PeripheralVisionAngle = SightSense->PeripheralVisionAngleDegrees;
    float MinViewingAngle = 90.0f - PeripheralVisionAngle;
    float MaxViewingAngle = 90.0f + PeripheralVisionAngle;

    const float CurrentViewingAngle = FMath::RadiansToDegrees(
                FMath::Acos(
                    FVector::DotProduct(ViewDirectionNormal,
                                        PlayerDirectionNormal)
                    )
                );

    if (CurrentViewingAngle <= MinViewingAngle
            && CurrentViewingAngle >= MaxViewingAngle) {
        return false;
    }

    FCollisionQueryParams TraceParams(TEXT("PlayerTrace"), true, this);
    TraceParams.bIgnoreTouches = false;
    TraceParams.bReturnPhysicalMaterial = false;

    FHitResult HitResult(ForceInit);
    GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, PlayerLocation,
                                         ECollisionChannel::ECC_Visibility, TraceParams);

    if (!Cast<ATPlayerCharacter>(HitResult.Actor))
    {
        return false;
    }

    return true;
}

bool ATAIController::IsPlayerInSafeDistance() const
{
    ATPlayerCharacter* PlayerCharacter =
            Cast<ATPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (!PlayerCharacter)
    {
        return false;
    }

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    const float MaxSightDistance = SightSense->SightRadius;
    const FVector ViewLocation(AICharacter->GetPawnViewLocation());
    const FVector PlayerLocation(PlayerCharacter->GetActorLocation());
    const FVector PlayerDirection(PlayerLocation - ViewLocation);
    const float PlayerDistance = PlayerDirection.Size();

    return (PlayerDistance > MaxSightDistance);
}

void ATAIController::SetTargetPawn(ATCharacter *OtherCharacter)
{
    if (TargetPawn == OtherCharacter)
    {
        return;
    }

    TargetPawn = OtherCharacter;

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (TargetPawn)
    {
        TLOG_AI_WARNING(TLOG_KEY_AI_SET_TARGET_PAWN,
                        TEXT("Target pawn is set!"),
                        Cast<AActor>(AICharacter), Cast<AActor>(TargetPawn));

        if (TargetItem)
        {
            SetTargetItem(nullptr);
        }

        TargetPawnLastSeenLocation = TargetPawn->GetActorLocation();

        if (!IsAlerted())
        {
            BeAlerted();
        }
    }
    else
    {
        TLOG_AI_WARNING(TLOG_KEY_AI_SET_TARGET_PAWN,
                        TEXT("Target pawn set to null!"),
                        Cast<AActor>(AICharacter), Cast<AActor>(AICharacter));
    }
}

void ATAIController::SetTargetItem(ATPickup* Item)
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (TargetItem == Item)
    {
        return;
    }

    if (Item && Item->IsAttachedToACharacter())
    {
        return;
    }

    if (TargetItem && IsCarryingItem())
    {
        AICharacter->DropItem();
    }

    TargetItem = Item;

    if (TargetItem)
    {
        TLOG_AI_WARNING(TLOG_KEY_AI_SET_TARGET_PICKUP,
                        TEXT("Target pickup is set!"),
                        Cast<AActor>(AICharacter), Cast<AActor>(TargetItem));

        TargetItemLastHeardLocation = TargetItem->GetActorLocation();

        if (!IsAlerted())
        {
            GetSuspicious();
        }
    }
    else
    {
        TLOG_AI_WARNING(TLOG_KEY_AI_SET_TARGET_PICKUP,
                        TEXT("Target pickup set to null!"),
                        Cast<AActor>(AICharacter));
    }
}

bool ATAIController::IsIdle() const
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    return (AICharacter->GetAIState() == EAIState::Idle);
}

bool ATAIController::IsSuspicious() const
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    return (AICharacter->GetAIState() == EAIState::Suspicious);
}

bool ATAIController::IsAlerted() const
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    return (AICharacter->GetAIState() == EAIState::Alerted);
}

bool ATAIController::IsInvestigating() const
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    return (AICharacter->GetAIState() == EAIState::Investigating);
}

bool ATAIController::IsCarryingItem() const
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    return (AICharacter->GetAIState() == EAIState::CarryingItem);
}

bool ATAIController::IsGoingBack() const
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    return (AICharacter->GetAIState() == EAIState::GoingBack);
}

void ATAIController::BeIdle()
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (IsIdle())
    {
        return;
    }

    ClearAllTimers();

    AICharacter->SetAIState(EAIState::Idle);

    ResetIdleTimer();
}

void ATAIController::GetSuspicious()
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (IsSuspicious())
    {
        return;
    }

    ClearAllTimers();

    AICharacter->SetAIState(EAIState::Suspicious);
    AICharacter->GetCharacterMovement()->MaxWalkSpeed =
            ChasingWalkSpeed * SuspiciousWalkSpeedRatio;

    GetWorldTimerManager().SetTimer(
                SuspiciousTimer,
                this, &ATAIController::OnSuspiciousTimerTick,
                SuspiciousTickInterval, true, -1.0f);
}

void ATAIController::BeAlerted()
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (IsAlerted())
    {
        return;
    }

    ClearAllTimers();

    AICharacter->SetAIState(EAIState::Alerted);
    AICharacter->GetCharacterMovement()->MaxWalkSpeed = ChasingWalkSpeed;

    GetWorldTimerManager().SetTimer(
                AlertedTimer,
                this, &ATAIController::OnAlertedTimerTick,
                AlertedTickInterval, true, -1.0f);
}

void ATAIController::Investigate()
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (IsInvestigating())
    {
        return;
    }

    ClearAllTimers();

    AICharacter->SetAIState(EAIState::Investigating);
    RemainingInvestigationTimes = UKismetMathLibrary::RandomIntegerInRange(
                MinInvestigationTimes, MaxInvestigationTimes);

    ResetInvestigationTimer();
}

void ATAIController::CarryItem()
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (IsCarryingItem())
    {
        return;
    }

    if (!TargetItem)
    {
        return;
    }

    if (TargetItem->IsAttachedToACharacter())
    {
        SetTargetItem(nullptr);
        return;
    }

    ClearAllTimers();

    AICharacter->PickupItem(TargetItem);

    AICharacter->SetAIState(EAIState::CarryingItem);
    AICharacter->GetCharacterMovement()->MaxWalkSpeed =
            ChasingWalkSpeed * CarryingItemWalkSpeedRatio;

    GetWorldTimerManager().SetTimer(
                CarryingItemTimer,
                this, &ATAIController::OnCarryingItemTick,
                CarryingItemTickInterval, true, -1.0f);
}

void ATAIController::GoBack()
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    if (IsGoingBack())
    {
        return;
    }

    ClearAllTimers();

    AICharacter->SetAIState(EAIState::GoingBack);
    AICharacter->GetCharacterMovement()->MaxWalkSpeed =
            ChasingWalkSpeed * GoingBackWalkSpeedRatio;

    GetWorldTimerManager().SetTimer(
                GoingBackTimer,
                this, &ATAIController::OnGoingBackTick,
                GoingBackTickInterval, true, -1.0f);
}

void ATAIController::DrawFOV()
{
    ATAICharacter *AICharacter = Cast<ATAICharacter>(this->GetCharacter());
    if (!AICharacter)
    {
        return;
    }

    const UWorld* InWorld = this->GetWorld();
    const FVector Origin(AICharacter->GetPawnViewLocation());
    const FVector Direction(AICharacter->GetViewRotation().Vector());
    const float Length = SightSense->SightRadius;
    const float AngleWidth =
            FMath::DegreesToRadians(SightSense->PeripheralVisionAngleDegrees);
    const float AngleHeight = AngleWidth / FOVDrawAngleHeightRatio;
    const int32 NumSides = FOVDrawNumSides;
    const FColor Color = FOVDrawColor.ToFColor(true);
    const bool bPersistentLines = false;
    const float LifeTime = 0.0f;
    const uint8 DepthPriority = 0;
    const float Thickness = FOVDrawThickness;

    DrawDebugCone(InWorld, Origin, Direction, Length,
                  AngleWidth, AngleHeight, NumSides,
                  Color, bPersistentLines, LifeTime,
                  DepthPriority, Thickness);
}

void ATAIController::UpdateFocus()
{
    if (TargetPawn)
    {
        SetFocus(TargetPawn, EAIFocusPriority::Default);
        SetFocus(TargetPawn, EAIFocusPriority::Gameplay);
        SetFocus(TargetPawn, EAIFocusPriority::Move);
    }
    else if (TargetItem && !IsInvestigating())
    {
        SetFocus(TargetItem, EAIFocusPriority::Default);
        SetFocus(TargetItem, EAIFocusPriority::Gameplay);
        SetFocus(TargetItem, EAIFocusPriority::Move);
    }
    else
    {
        if (GetFocusActor())
        {
            ClearFocus(EAIFocusPriority::Default);
            ClearFocus(EAIFocusPriority::Gameplay);
            ClearFocus(EAIFocusPriority::Move);
        }
    }
}

void ATAIController::SetTargetControlRotation(const FRotator& Rotation)
{
    if (!GetControlRotation().Equals(Rotation))
    {
        TargetControlRotation = Rotation;
    }
}

void ATAIController::SetTargetControlRotation(const FVector& Location)
{
    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    FVector Direction(Location - AICharacter->GetActorLocation());
    Direction.Normalize();
    FRotator Rotation(FRotationMatrix::MakeFromX(Direction).Rotator());

    SetTargetControlRotation(Rotation);
}

void ATAIController::AdjustControlRotation(const float DeltaSeconds)
{
    UpdateFocus();

    const FRotator& Rotation = GetControlRotation();

    if (!Rotation.Equals(TargetControlRotation))
    {
        const FRotator AdjustedRotation = FMath::RInterpTo(
                    Rotation, TargetControlRotation,
                    DeltaSeconds, BaseTurnRate);

        SetControlRotation(AdjustedRotation);
    }
}

void ATAIController::DoRandomTurn()
{
    FRotator RandomRotation(GetControlRotation());
    RandomRotation.Yaw = FMath::FRand() * 360.0f;
    SetTargetControlRotation(RandomRotation);
}

EPathFollowingRequestResult::Type ATAIController::MoveToTargetLocation(
        const FVector& Location)
{
    SetTargetControlRotation(Location);

    EPathFollowingRequestResult::Type Result =
            MoveToLocation(Location, -1.0f,
                           true, true, true, true, nullptr, true);

    ATAICharacter* AICharacter = Cast<ATAICharacter>(GetCharacter());
    checkf(AICharacter, TEXT("FATAL: not HideAndSeekWithAI's AI character!"));

    switch (Result)
    {
    case EPathFollowingRequestResult::RequestSuccessful:
        TLOG_AI_WARNING(TLOG_KEY_AI_MOVEMENT, TEXT("Moving toward location!"),
                        Cast<AActor>(AICharacter), Location);
        break;
    case EPathFollowingRequestResult::AlreadyAtGoal:
        TLOG_AI_WARNING(TLOG_KEY_AI_MOVEMENT, TEXT("Already at goal!"),
                        Cast<AActor>(AICharacter), Location);
        break;
    case EPathFollowingRequestResult::Failed:
        TLOG_AI_WARNING(TLOG_KEY_AI_MOVEMENT,
                        TEXT("Moving toward location has failed!"),
                        Cast<AActor>(AICharacter), Location);
        break;
    }

    return Result;
}

void ATAIController::ClearAllTimers()
{
    if (GetWorldTimerManager().IsTimerActive(IdleTimer))
    {
        GetWorldTimerManager().ClearTimer(IdleTimer);
    }

    if (GetWorldTimerManager().IsTimerActive(SuspiciousTimer))
    {
        GetWorldTimerManager().ClearTimer(SuspiciousTimer);
    }

    if (GetWorldTimerManager().IsTimerActive(AlertedTimer))
    {
        GetWorldTimerManager().ClearTimer(AlertedTimer);
    }

    if (GetWorldTimerManager().IsTimerActive(InvestigationTimer))
    {
        GetWorldTimerManager().ClearTimer(InvestigationTimer);
    }

    if (GetWorldTimerManager().IsTimerActive(CarryingItemTimer))
    {
        GetWorldTimerManager().ClearTimer(CarryingItemTimer);
    }
}

void ATAIController::ResetIdleTimer()
{
    float Timeout = FMath::FRandRange(MinIdleTickInterval, MaxIdleTickInterval);
    GetWorldTimerManager().SetTimer(
                IdleTimer,
                this, &ATAIController::OnIdleTimerTick,
                Timeout, false, -1.0f);
}

void ATAIController::ResetInvestigationTimer()
{
    float Timeout = FMath::FRandRange(
                MinInvestigationTickInterval, MaxInvestigationTickInterval);
    GetWorldTimerManager().SetTimer(
                InvestigationTimer,
                this, &ATAIController::OnInvestigationTick,
                Timeout, false, -1.0f);
}

bool ATAIController::IsGameOnGoing() const
{
    UWorld* World = this->GetWorld();
    ATGameState *GameState = Cast<ATGameState>(
                UGameplayStatics::GetGameState(World));
    checkf(GameState, TEXT("FATAL: not HideAndSeekWithAI's game state!"));

    return GameState->IsGameOnGoing();
}
