#pragma once

#include <Containers/Array.h>
#include <GameFramework/Actor.h>
#include <UObject/ObjectMacros.h>

#include "TPickup.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class UStaticMeshComponent;

class ATCharacter;

/** The base class for all item pickups inside the game. */
UCLASS()
class HIDEANDSEEKWITHAI_API ATPickup : public AActor
{
    GENERATED_UCLASS_BODY()

protected:
    /** The mesh that visually represents this obstacle. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
    UStaticMeshComponent* Mesh;

    /** The trigger area to notify the game and let the player know that they
     *  can pick up the item. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
    UBoxComponent* Trigger;

    /** Maximum noise range when this item is thrown. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    float MaxThrowingNoiseRange;

private:
    /** The original spawn point of this pickup item. */
    UPROPERTY(Transient)
    FVector SpawnPoint;

    /** The character this pickup item is attached to when it gets picked up. */
    UPROPERTY(Transient)
    ATCharacter* AttachedCharacter;

    /** The Previous owner of this pickup item used internally as a noise
     *  instigator. */
    UPROPERTY(Transient)
    ATCharacter* PreviousOwner;

    /** An array of colors used for drawing the traverse path of this pickup
     *  item when it is thrown. */
    UPROPERTY(Transient)
    TArray<FColor> TraceColors;

    /** Caches the current color used for drawing the traverse path of this pickup
     *  item when it is thrown. */
    UPROPERTY(Transient)
    int32 CurrentTraceColorIndex;

public:
    virtual void NotifyHit(UPrimitiveComponent* MyComp,
                           AActor* Other,
                           UPrimitiveComponent* OtherComp,
                           bool bSelfMoved,
                           FVector HitLocation,
                           FVector HitNormal,
                           FVector NormalImpulse,
                           const FHitResult& hit) override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void OnConstruction(const FTransform& Transform) override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Fires when the player enters this pickup items trigger and the pickup is
     *  available to the player in order to be picked up. */
    UFUNCTION()
    virtual void OnOverlapBegins(
            UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

    /** Fires when the player leaves this pickup items trigger and the pickup
     *  is not available to the player anymore. */
    UFUNCTION()
    virtual void OnOverlapEnds(
            UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex);

public:
    /** Returns the original spawn point of the bot. */
    FORCEINLINE const FVector& GetSpawnPoint() const
    {
        return SpawnPoint;
    }

    /** Sets the spawn point of the pickup item. */
    FORCEINLINE void SetSpawnPoint(const FVector& Location)
    {
        SpawnPoint = Location;
    }

    /** Returns the mesh representing the pickup item */
    FORCEINLINE UStaticMeshComponent* GetMesh() const
    {
        return Mesh;
    }

    /** Returns whether this pickup item is attached to a character or not. */
    FORCEINLINE bool IsAttachedToACharacter() const
    {
        return (AttachedCharacter != nullptr);
    }

    /** Returns the character this pickup item is attached to. */
    FORCEINLINE ATCharacter* GetAttachedCharacter() const
    {
        return AttachedCharacter;
    }

    /** Attaches this pickup item to a character. */
    void AttachToCharacter(ATCharacter* Character);

    /** Detaches this pickup item to a character. */
    void DetachFromCharacter(ATCharacter* Character);

private:
    /** Sets the visibility of this item inside the game. */
    void SetVisibility(const bool bVisibility);

    /** Determines whether this object is moving or not used internally for
     *  drawing the path this item is moving on. */
    bool IsMoving() const;
};
