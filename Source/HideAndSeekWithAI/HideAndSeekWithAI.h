// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Fonts/SlateFontInfo.h>
#include <Misc/Paths.h>

#define TTF_FONT( RelativePath, ... ) (FSlateFontInfo ( FPaths::ProjectContentDir() / "HideAndSeekWithAI/Art/Fonts" / RelativePath + TEXT(".ttf"), __VA_ARGS__ ))

#define T_NOISE "NOISE_STIMULUS"

/** The AI states. */
UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle            UMETA(DisplayName="Idle"),
    Suspicious      UMETA(DisplayName="Suspicious"),
    Alerted         UMETA(DisplayName="Alerted"),
    Investigating   UMETA(DisplayName="Investigating"),
    CarryingItem    UMETA(DisplayName="CarryingItem"),
    GoingBack       UMETA(DisplayName="GoingBack"),
};

/** The match results used to determine the game state both by UI and the game
 *  mode. */
UENUM(BlueprintType)
enum class EMatchResults : uint8
{
    OnGoing     UMETA(DisplayName="On Going"),
    Caught      UMETA(DisplayName="Caught"),
    Won         UMETA(DisplayName="Won"),
};
