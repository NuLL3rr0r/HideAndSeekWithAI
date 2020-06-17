#include "TCharacterMovementComponent.h"
#include "HideAndSeekWithAI.h"

UTCharacterMovementComponent::UTCharacterMovementComponent(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetCanEverAffectNavigation(false);
}
