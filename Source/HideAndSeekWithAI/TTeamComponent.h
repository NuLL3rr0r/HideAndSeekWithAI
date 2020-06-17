#pragma once

#include <Components/ActorComponent.h>
#include <CoreTypes.h>
#include <Math/Color.h>
#include <UObject/ObjectMacros.h>

#include "TTeamComponent.generated.h"

/** The team component in order to detect enemies, friendlies, or neutrals. */
UCLASS(ClassGroup=(HIDEANDSEEKWITHAI), meta=(BlueprintSpawnableComponent))
class HIDEANDSEEKWITHAI_API UTTeamComponent : public UActorComponent
{
    GENERATED_UCLASS_BODY()

public:
    /** Determines whether an actor is neutral or not. */
    static bool IsNeutral(AActor* Actor);

    /** Determines whether actors A and B are friendly or not. */
    static bool AreFriendly(AActor* ActorA, AActor* ActorB);

    /** Determines whether actors A and B are enemies or not. */
    FORCEINLINE static bool AreEnemies(AActor* ActorA, AActor* ActorB)
    {
        return (!IsNeutral(ActorA) && !IsNeutral(ActorB)
                && !AreFriendly(ActorA, ActorB));
    }

protected:
    /** The team number */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team Component")
    uint8 TeamNumber;

    /** The team color */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team Component")
    FColor TeamColor;

public:
    /** Returns the team number */
    FORCEINLINE uint8 GetTeamNumber() const
    {
        return TeamNumber;
    }

    /** Sets the team number */
    FORCEINLINE void SetTeamNumber(const uint8 Number)
    {
        TeamNumber = Number;
    }

    /** Returns the team color */
    FORCEINLINE const FColor& GetTeamColor() const
    {
        return TeamColor;
    }

    /** Sets the team color */
    FORCEINLINE void SetTeamColor(const FColor& Color)
    {
        TeamColor = Color;
    }
};
