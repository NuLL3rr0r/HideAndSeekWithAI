#include "TObstacle.h"
#include "HideAndSeekWithAI.h"

#include <Components/StaticMeshComponent.h>

ATObstacle::ATObstacle(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(
                this, TEXT("Mesh"));
    Mesh->SetCanEverAffectNavigation(true);
    Mesh->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ObstacleMesh(
                TEXT("StaticMesh'/Game/StarterContent/Shapes/"
                     "Shape_Cube.Shape_Cube'"));
    if (ObstacleMesh.Succeeded() && ObstacleMesh.Object)
    {
        Mesh->SetStaticMesh(ObstacleMesh.Object);
    }

    SetRootComponent(Mesh);
}
