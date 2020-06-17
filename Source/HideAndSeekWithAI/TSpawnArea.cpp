#include "TSpawnArea.h"
#include "HideAndSeekWithAI.h"

#include <Components/BillboardComponent.h>
#include <Components/BoxComponent.h>
#include <Engine/Texture2D.h>
#include <UObject/ConstructorHelpers.h>

ATSpawnArea::ATSpawnArea(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;

    Area = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(
                this, TEXT("Area"));
    Area->SetBoxExtent(FVector(5995.0f, 1495.0f, 1.0f), true);
    Area->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Area->SetCollisionResponseToAllChannels(ECR_Ignore);
    Area->SetCanEverAffectNavigation(false);

    SetRootComponent(Area);

    Icon = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(
                this, TEXT("Icon"));
    Icon->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UTexture2D> SpawnIcon(
                TEXT("Texture2D'/Game/HideAndSeekWithAI/Art/Textures/"
                     "T_SpawnArea.T_SpawnArea'"));
    if (SpawnIcon.Succeeded() && SpawnIcon.Object)
    {
        Icon->Sprite = SpawnIcon.Object;
    }
}
