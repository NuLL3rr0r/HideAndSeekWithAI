#include "TGameWidget.h"
#include "HideAndSeekWithAI.h"

#include <Engine/World.h>
#include <SlateBasics.h>
#include <SlateOptMacros.h>
#include <Styling/SlateBrush.h>
#include <Styling/SlateColor.h>
#include <Styling/SlateTypes.h>
#include <Templates/Casts.h>
#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SConstraintCanvas.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/SCanvas.h>

#include "THUD.h"

#define LOCTEXT_NAMESPACE "STGameWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STGameWidget::Construct(
        const FArguments& InArgs)
{
    OwnerHUD = InArgs._OwnerHUD;

    this->ChildSlot
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
            [
                SNew(SOverlay)
                + SOverlay::Slot()
                .VAlign(VAlign_Top)
                .HAlign(HAlign_Center)
                .Padding(FMargin(100.0f))
                [
                    SNew(STextBlock)
                    .TextStyle(GetTextStyle())
                    .Text(TAttribute<FText>::Create(
                              TAttribute<FText>::FGetter::CreateUObject(
                                  OwnerHUD.Get(),
                                  &ATHUD::GetMatchResultsText)
                              )
                          )
                ]
                + SOverlay::Slot()
                .VAlign(VAlign_Top)
                .HAlign(HAlign_Center)
                .Padding(FMargin(200.0f))
                [
                    SNew(STextBlock)
                    .TextStyle(GetTextStyle())
                    .Text(TAttribute<FText>::Create(
                              TAttribute<FText>::FGetter::CreateUObject(
                                  OwnerHUD.Get(),
                                  &ATHUD::GetGameMessageText)
                              )
                          )
                ]
            ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

const FTextBlockStyle* STGameWidget::GetTextStyle() const
{
    FTextBlockStyle* Style = new FTextBlockStyle();

    Style->SetFont(TTF_FONT("LiberationSans-Bold", 40));
    Style->SetColorAndOpacity(FLinearColor::Yellow);

    return Style;
}

#undef LOCTEXT_NAMESPACE
