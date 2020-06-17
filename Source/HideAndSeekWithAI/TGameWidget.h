#pragma once

#include <Slate/SlateGameResources.h>
#include <Styling/SlateTypes.h>
#include <UObject/WeakObjectPtrTemplates.h>
#include <Widgets/SCompoundWidget.h>

class ATHUD;

/** The main game's slate widget which is used to show the match results or the
 *  game messages based on the game situations. */
class HIDEANDSEEKWITHAI_API STGameWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STGameWidget)
    {  }

    SLATE_ARGUMENT(TWeakObjectPtr<ATHUD>, OwnerHUD);

    SLATE_END_ARGS (  )

    /* Constructs the widget. */
    void Construct(const FArguments& InArgs);

protected:
    /** The game HUD that owns this widget. */
    TWeakObjectPtr<ATHUD> OwnerHUD;

protected:
    /** Provides the style for all the text shown in this widget. */
    const FTextBlockStyle* GetTextStyle() const;
};
