#pragma once

#include <GameFramework/CharacterMovementComponent.h>
#include <UObject/ObjectMacros.h>

#include "TCharacterMovementComponent.generated.h"

/** Base class for all customized character movement components in the game. */
UCLASS()
class HIDEANDSEEKWITHAI_API UTCharacterMovementComponent
    : public UCharacterMovementComponent
{
    GENERATED_UCLASS_BODY()
};
