#include "TWinSpot.h"
#include "HideAndSeekWithAI.h"

#include <Components/BillboardComponent.h>
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/Texture2D.h>
#include <Kismet/GameplayStatics.h>
#include <Materials/Material.h>
#include <Templates/Casts.h>
#include <UObject/ConstructorHelpers.h>

#include "TGameMode.h"
#include "TPlayerCharacter.h"

ATWinSpot::ATWinSpot(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(
                this, TEXT("Trigger"));
    Trigger->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f), true);
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Trigger->SetCollisionResponseToAllChannels(ECR_Block);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Trigger->SetCanEverAffectNavigation(false);

    SetRootComponent(Trigger);

    Icon = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(
                this, TEXT("Icon"));
    Icon->SetupAttachment(RootComponent);
    Icon->SetHiddenInGame(false, false);

    static ConstructorHelpers::FObjectFinder<UTexture2D> SpawnIcon(
                TEXT("Texture2D'/Game/HideAndSeekWithAI/Art/Textures/"
                     "T_WinSpot.T_WinSpot'"));
    if (SpawnIcon.Succeeded() && SpawnIcon.Object)
    {
        Icon->Sprite = SpawnIcon.Object;
    }

    GreenPlane = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(
                this, TEXT("GreenPlane"));
    GreenPlane->SetupAttachment(RootComponent);
    GreenPlane->SetCanEverAffectNavigation(true);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> GreenPlaneMesh(
                TEXT("StaticMesh'/Game/StarterContent/Shapes/"
                     "Shape_Plane.Shape_Plane'"));
    if (GreenPlaneMesh.Succeeded() && GreenPlaneMesh.Object)
    {
        GreenPlane->SetStaticMesh(GreenPlaneMesh.Object);
        GreenPlane->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
    }

    static ConstructorHelpers::FObjectFinder<UMaterial> GreenPlaneMaterial(
                TEXT("Material'/Game/HideAndSeekWithAI/Art/Materials/"
                     "M_WinSpot_GreenPlane.M_WinSpot_GreenPlane'"));
    if (GreenPlaneMaterial.Succeeded() && GreenPlaneMaterial.Object)
    {
        GreenPlane->SetMaterial(0, GreenPlaneMaterial.Object);
    }
}

void ATWinSpot::BeginPlay()
{
    Super::BeginPlay();

    Trigger->OnComponentBeginOverlap.AddDynamic(
                this, &ATWinSpot::OnOverlapBegins);
}

void ATWinSpot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Trigger->OnComponentBeginOverlap.RemoveDynamic(
                this, &ATWinSpot::OnOverlapBegins);
}

void ATWinSpot::OnOverlapBegins(
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

    ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(OtherActor);
    if (PlayerCharacter) {
        UWorld* World = this->GetWorld();
        ATGameMode *GameMode = Cast<ATGameMode>(
                    UGameplayStatics::GetGameMode(World));

        checkf(GameMode, TEXT("FATAL: not HideAndSeekWithAI's game mode!"));

        GameMode->PlayerWon(PlayerCharacter);
    }
}
