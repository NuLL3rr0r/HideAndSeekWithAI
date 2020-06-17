#include "TPickup.h"
#include "HideAndSeekWithAI.h"

#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/StaticMeshComponent.h>
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include <Math/UnrealMathUtility.h>
#include <Templates/Casts.h>

#include "TCharacter.h"
#include "TGameMode.h"
#include "TLog.h"
#include "TPlayerCharacter.h"

static constexpr uint64 TLOG_KEY_ITEM_THROW = TLOG_KEY_GENERIC + 3000;
static constexpr uint64 TLOG_KEY_ITEM_THROW_BOUNCE =
        TLOG_KEY_ITEM_THROW + 1;
static constexpr uint64 TLOG_KEY_ITEM_THROW_BOUNCE_COLOR =
        TLOG_KEY_ITEM_THROW_BOUNCE + 1;
static constexpr uint64 TLOG_KEY_ITEM_THROW_BOUNCE_NOISE =
        TLOG_KEY_ITEM_THROW_BOUNCE_COLOR + 1;

ATPickup::ATPickup(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(
                this, TEXT("Mesh"));
    Mesh->SetNotifyRigidBodyCollision(true);
    Mesh->SetCanEverAffectNavigation(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PickupMesh(
                TEXT("StaticMesh'/Game/StarterContent/Shapes/"
                     "Shape_Cube.Shape_Cube'"));
    if (PickupMesh.Succeeded() && PickupMesh.Object)
    {
        Mesh->SetStaticMesh(PickupMesh.Object);
        Mesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
    }

    SetRootComponent(Mesh);

    Trigger = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(
                this, TEXT("Trigger"));
    Trigger->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f), true);
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Trigger->SetCanEverAffectNavigation(true);
    Trigger->SetupAttachment(RootComponent);

    const UStaticMesh* StaticMesh(Mesh->GetStaticMesh());
    if (StaticMesh)
    {
        FVector RelativeScale3D(
                    1.0f / Mesh->GetRelativeScale3D().X,
                    1.0f / Mesh->GetRelativeScale3D().Y,
                    1.0f / Mesh->GetRelativeScale3D().Z
                    );
        Trigger->SetRelativeScale3D(RelativeScale3D);
        Trigger->SetRelativeLocation(
                    FVector(0.0f, 0.0f,
                            (Trigger->GetUnscaledBoxExtent().Z)
                            * RelativeScale3D.Z));
    }

    MaxThrowingNoiseRange = 2000.0f;

    AttachedCharacter = nullptr;

    CurrentTraceColorIndex = -1;
}

void ATPickup::NotifyHit(UPrimitiveComponent* MyComp,
                          AActor* Other,
                          UPrimitiveComponent* OtherComp,
                          bool bSelfMoved,
                          FVector HitLocation,
                          FVector HitNormal,
                          FVector NormalImpulse,
                          const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved,
                     HitLocation, HitNormal, NormalImpulse, Hit);

    if (Other)
    {
        TLOG_WARNING(TLOG_KEY_ITEM_THROW_BOUNCE,
                     TEXT("Thrown item hit!"),
                     Other, bSelfMoved,
                     HitLocation, HitNormal, NormalImpulse);
    }
    else
    {
        TLOG_WARNING(TLOG_KEY_ITEM_THROW_BOUNCE,
                     TEXT("Thrown item hit!"),
                     bSelfMoved,
                     HitLocation, HitNormal, NormalImpulse);
    }

    int32 Color = CurrentTraceColorIndex;
    while (Color == CurrentTraceColorIndex)
    {
        Color = FMath::RandRange(0, TraceColors.Num() - 1);
    }
    CurrentTraceColorIndex = Color;

    if (PreviousOwner && PreviousOwner->IsA<ATPlayerCharacter>())
    {
        FName Tag(*FString::Printf(TEXT("%s_%s"), TEXT(T_NOISE),
                                   *UKismetSystemLibrary::GetDisplayName(this)));

        TLOG_WARNING(TLOG_KEY_ITEM_THROW_BOUNCE_NOISE,
                     TEXT("Making noise!"),
                     Cast<AActor>(PreviousOwner), GetActorLocation(),
                     MaxThrowingNoiseRange, Tag);

        MakeNoise(1, PreviousOwner, GetActorLocation(),
                  MaxThrowingNoiseRange, Tag);
    }
}

void ATPickup::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (IsMoving())
    {
        TLOG_WARNING(TLOG_KEY_ITEM_THROW_BOUNCE_COLOR,
                     TEXT("Current bounce color index!"),
                     CurrentTraceColorIndex);

        DrawDebugLine(GetWorld(), GetActorLocation(),
                      GetActorLocation() + GetVelocity() * DeltaSeconds,
                      TraceColors[CurrentTraceColorIndex], false, 4.0f, 0, 1.0f);
    }
}

void ATPickup::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    TraceColors.AddUnique(FColor::White);
    TraceColors.AddUnique(FColor::Black);
    TraceColors.AddUnique(FColor::Transparent);
    TraceColors.AddUnique(FColor::Red);
    TraceColors.AddUnique(FColor::Green);
    TraceColors.AddUnique(FColor::Blue);
    TraceColors.AddUnique(FColor::Yellow);
    TraceColors.AddUnique(FColor::Cyan);
    TraceColors.AddUnique(FColor::Magenta);
    TraceColors.AddUnique(FColor::Orange);
    TraceColors.AddUnique(FColor::Purple);
    TraceColors.AddUnique(FColor::Turquoise);
    TraceColors.AddUnique(FColor::Silver);
    TraceColors.AddUnique(FColor::Emerald);

    CurrentTraceColorIndex = FMath::RandRange(0, TraceColors.Num() - 1);
}

void ATPickup::BeginPlay()
{
    Super::BeginPlay();

    Trigger->OnComponentBeginOverlap.AddDynamic(
                this, &ATPickup::OnOverlapBegins);
    Trigger->OnComponentEndOverlap.AddDynamic(
                this, &ATPickup::OnOverlapEnds);
}

void ATPickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Trigger->OnComponentBeginOverlap.RemoveDynamic(
                this, &ATPickup::OnOverlapBegins);
    Trigger->OnComponentEndOverlap.RemoveDynamic(
                this, &ATPickup::OnOverlapEnds);
}

void ATPickup::OnOverlapBegins(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult)
{
    (void)OverlappedComponent;
    (void)OtherComp;
    (void)OtherBodyIndex;
    (void)bFromSweep;
    (void)SweepResult;

    if (IsAttachedToACharacter())
    {
        return;
    }

    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(OtherActor);
    if (PlayerCharacter) {
        UWorld* World = this->GetWorld();
        ATGameMode *GameMode = Cast<ATGameMode>(
                    UGameplayStatics::GetGameMode(World));

        checkf(GameMode, TEXT("FATAL: not HideAndSeekWithAI's game mode!"));

        GameMode->NotifyPickupAvailable(this);
    }
}

void ATPickup::OnOverlapEnds(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex)
{
    (void)OverlappedComponent;
    (void)OtherComp;
    (void)OtherBodyIndex;

    if (IsAttachedToACharacter())
    {
        return;
    }

    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(OtherActor);
    if (PlayerCharacter) {
        UWorld* World = this->GetWorld();
        ATGameMode *GameMode = Cast<ATGameMode>(
                    UGameplayStatics::GetGameMode(World));

        checkf(GameMode, TEXT("FATAL: not HideAndSeekWithAI's game mode!"));

        GameMode->NotifyPickupAvailable(nullptr);
    }
}

void ATPickup::AttachToCharacter(ATCharacter* Character)
{
    if (!Character)
    {
        return;
    }

    if (IsAttachedToACharacter())
    {
        return;
    }

    if (RootComponent->IsAttachedTo(Character->GetItemAttachPoint()))
    {
        return;
    }

    UWorld* World = this->GetWorld();
    ATGameMode *GameMode = Cast<ATGameMode>(
                UGameplayStatics::GetGameMode(World));
    checkf(GameMode, TEXT("FATAL: not HideAndSeekWithAI's game mode!"));

    if (Character->IsA<ATPlayerCharacter>())
    {
        SetVisibility(false);
    }

    Mesh->SetEnableGravity(false);
    Mesh->SetSimulatePhysics(false);
    SetActorEnableCollision(false);

    RootComponent->AttachToComponent(
                Character->GetItemAttachPoint(),
                FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
                                          EAttachmentRule::SnapToTarget,
                                          EAttachmentRule::SnapToTarget,
                                          true));

    if (Character->IsA<ATPlayerCharacter>())
    {
        GameMode->NotifyPickupAvailable(nullptr);
    }

    AttachedCharacter = Character;
}

void ATPickup::DetachFromCharacter(ATCharacter* Character)
{
    if (!Character)
    {
        return;
    }

    if (!IsAttachedToACharacter())
    {
        return;
    }

    if (!RootComponent->IsAttachedTo(Character->GetItemAttachPoint()))
    {
        return;
    }

    PreviousOwner = AttachedCharacter;

    RootComponent->DetachFromComponent(
                FDetachmentTransformRules(EDetachmentRule::KeepWorld,
                                          EDetachmentRule::KeepWorld,
                                          EDetachmentRule::KeepWorld,
                                          true));

    ATPickup* DefaultObject = GetClass()->GetDefaultObject<ATPickup>();
    Mesh->SetRelativeScale3D(DefaultObject->GetMesh()->GetRelativeScale3D());

    AttachedCharacter = nullptr;

    if (Character->IsA<ATPlayerCharacter>())
    {
        SetVisibility(true);
    }

    SetActorEnableCollision(true);
    Mesh->SetEnableGravity(true);
    Mesh->SetSimulatePhysics(true);
}

void ATPickup::SetVisibility(const bool bVisibility)
{
    /* It makes sense to disable tick and collisions in addition to hiding the
     * pickup item in order to avoid unwanted behaviors and bugs. */
    SetActorHiddenInGame(!bVisibility);
    SetActorEnableCollision(bVisibility);
    SetActorTickEnabled(bVisibility);
}

bool ATPickup::IsMoving() const
{
    const float Speed = FVector::DotProduct(
                GetVelocity(), GetActorRotation().Vector());

    return !FMath::IsNearlyZero(Speed);
}
