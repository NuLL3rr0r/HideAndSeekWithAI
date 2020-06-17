#include "TTeamComponent.h"
#include "HideAndSeekWithAI.h"

#include <GameFramework/Actor.h>
#include <Templates/Casts.h>

static constexpr uint8 NEUTRAL_TEAM_NUMBER = 255;

bool UTTeamComponent::IsNeutral(AActor* Actor)
{
    checkf(Actor, TEXT("%s"), TEXT("FATAL: Actor is NULL!"));

    UTTeamComponent* TeamComponent = Cast<UTTeamComponent>(
                Actor->GetComponentByClass(UTTeamComponent::StaticClass()));

    if (!TeamComponent)
    {
        return true;
    }

    return TeamComponent->TeamNumber == NEUTRAL_TEAM_NUMBER;
}

bool UTTeamComponent::AreFriendly(AActor* ActorA, AActor* ActorB)
{
    checkf(ActorA, TEXT("%s"), TEXT("FATAL: Actor A is NULL!"));
    checkf(ActorB, TEXT("%s"), TEXT("FATAL: Actor B is NULL!"));

    checkf(ActorA != ActorB, TEXT("%s"),
           TEXT("FATAL: Actor A and B are the same!"));

    UTTeamComponent* TeamComponentA = Cast<UTTeamComponent>(
                ActorA->GetComponentByClass(UTTeamComponent::StaticClass()));
    UTTeamComponent* TeamComponentB = Cast<UTTeamComponent>(
                ActorB->GetComponentByClass(UTTeamComponent::StaticClass()));

    if (!TeamComponentA || !TeamComponentB)
    {
        return true;
    }

    return TeamComponentA->TeamNumber == TeamComponentB->TeamNumber;
}

UTTeamComponent::UTTeamComponent(
        const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetCanEverAffectNavigation(false);

    TeamNumber = NEUTRAL_TEAM_NUMBER;
    TeamColor = FColor::White;
}
