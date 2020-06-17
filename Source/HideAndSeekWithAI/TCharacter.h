#pragma once

#include <GameFramework/Character.h>
#include <UObject/ObjectMacros.h>

#include "TCharacter.generated.h"

class UStaticMeshComponent;

class TArrowComponent;
class UTTeamComponent;

class ATPickup;

/** Base class for all characters in this game, e.g. players and bots. */
UCLASS(Abstract, BlueprintType, config=Game)
class HIDEANDSEEKWITHAI_API ATCharacter : public ACharacter
{
    GENERATED_UCLASS_BODY()

protected:
    /** The team component in order to detect enemies, friendlies, or neutrals. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTTeamComponent* Team;

    /** Static mesh used as a visual representation of this character. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
    UStaticMeshComponent* StaticMesh;

    /** The attach point for pickup items. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
    UArrowComponent* ItemAttachPoint;

    /** The pickup item this character carries. */
    UPROPERTY(Transient)
    ATPickup* Item;

public:
    /** Get the attach point for pickup items. */
    FORCEINLINE UArrowComponent* GetItemAttachPoint() const
    {
        return ItemAttachPoint;
    }

    /** Does this character carrying any pickup item at the moment or not? */
    FORCEINLINE bool HasAnyItems() const
    {
        return (Item != nullptr);
    }

    /** Can the character pickup any new pickup items or the current carrying
     *  slot is filled? */
    FORCEINLINE bool CanPickupItems() const
    {
        return (Item == nullptr);
    }

    /** Get the current pickup item the character is carrying. */
    FORCEINLINE ATPickup* GetCarryingItem() const
    {
        return Item;
    }

    /** Pick an item to carry. */
    void PickupItem(ATPickup* Pickup);

    /** Drop the currently picked up item. */
    void DropItem();

    /** Determines whether the character is moving or not. */
    bool IsMoving() const;
};
