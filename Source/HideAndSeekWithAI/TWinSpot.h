#pragma once

#include <GameFramework/Actor.h>
#include <UObject/ObjectMacros.h>

#include "TWinSpot.generated.h"

class UBoxComponent;
class UBillboardComponent;
class UPrimitiveComponent;

/** The game's winning spot. */
UCLASS()
class HIDEANDSEEKWITHAI_API ATWinSpot : public AActor
{
    GENERATED_UCLASS_BODY()

protected:
    /** The winning spot's trigger in order to detect the player presence on the
      *  winning spot. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win Spot")
    UBoxComponent* Trigger;

    /** A billboard showing a flag icon inside the game for better visibility
     *  by the player. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win Spot")
    UBillboardComponent* Icon;

    /** The mesh that visually represents this object inside the game. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win Spot")
    UStaticMeshComponent* GreenPlane;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** When player enters the winning spot's trigger this event fires. */
    UFUNCTION()
    virtual void OnOverlapBegins(
            UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);
};
