#pragma once

#include <UObject/ObjectMacros.h>

#include "TCharacterMovementComponent.h"

#include "TAICharacterMovementComponent.generated.h"

/** Used to customize the character movement component for the AI bots. */
UCLASS()
class HIDEANDSEEKWITHAI_API UTAICharacterMovementComponent
    : public UTCharacterMovementComponent
{
    GENERATED_UCLASS_BODY()
};
