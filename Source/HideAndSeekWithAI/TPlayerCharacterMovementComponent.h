#pragma once

#include <UObject/ObjectMacros.h>

#include "TCharacterMovementComponent.h"

#include "TPlayerCharacterMovementComponent.generated.h"

/** Used to customize the player's character movement component. */
UCLASS()
class HIDEANDSEEKWITHAI_API UTPlayerCharacterMovementComponent
    : public UTCharacterMovementComponent
{
    GENERATED_UCLASS_BODY()
};
