#pragma once

#include <GameFramework/Actor.h>
#include <UObject/ObjectMacros.h>

#include "TSpawnArea.generated.h"

class UBillboardComponent;
class UBoxComponent;

/** The spawn are for the current level in order to spawn obstacles, pickup
 *  items, and bots. */
UCLASS()
class HIDEANDSEEKWITHAI_API ATSpawnArea : public AActor
{
    GENERATED_UCLASS_BODY()

protected:
    /** The spawn area to spawn everything inside. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Area")
    UBoxComponent* Area;

    /** An editor only billboard for easy selection of this component. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Area")
    UBillboardComponent* Icon;

public:
    /** Returns the spawn area component. */
    FORCEINLINE const UBoxComponent* GetArea() const
    {
        return Area;
    }
};
