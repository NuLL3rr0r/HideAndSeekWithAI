#pragma once

#include <GameFramework/Actor.h>
#include <UObject/ObjectMacros.h>

#include "TObstacle.generated.h"

class UStaticMeshComponent;

/** The base class for all obstacles inside the game. */
UCLASS()
class HIDEANDSEEKWITHAI_API ATObstacle : public AActor
{
    GENERATED_UCLASS_BODY()

protected:
    /** The mesh that visually represents this obstacle. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
    UStaticMeshComponent* Mesh;
};
