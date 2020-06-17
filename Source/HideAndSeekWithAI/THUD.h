#pragma once

#include <GameFramework/HUD.h>
#include <UObject/ObjectMacros.h>

#include "THUD.generated.h"

class STGameWidget;

/** The main game head-up display */
UCLASS(Abstract, Blueprintable, BlueprintType, config=Game)
class HIDEANDSEEKWITHAI_API ATHUD : public AHUD
{
    GENERATED_UCLASS_BODY()

protected:
    /** Refrence to main game's widget for displaying game results and
     *  messages. */
    TSharedPtr<STGameWidget> GameWidget;

public:
    /** Returns the game results or title message to be displayed inside the
     *  game's main widget.*/
    FText GetMatchResultsText() const;

    /** Returns the game messages (e.g. available pickup items notification,
     *  timed item throwing feed back, etc) to be displayed inside the game's
        .main widget. */
    FText GetGameMessageText() const;

protected:
    virtual void BeginPlay() override;
};
