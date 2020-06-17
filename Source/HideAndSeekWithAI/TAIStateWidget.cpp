#include "TAIStateWidget.h"
#include "HideAndSeekWithAI.h"

#include <Blueprint/WidgetTree.h>
#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>
#include <Components/PanelWidget.h>
#include <Components/TextBlock.h>

#include "TAICharacter.h"

UTAIStateWidget::UTAIStateWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsVolatile = true;

    IdleText = FText::FromString(".");
    IdleTextColor = FLinearColor::Green;
    IdleTextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.2f);

    SuspiciousText = FText::FromString("?");
    SuspiciousTextColor = FLinearColor::Yellow;
    SuspiciousTextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.2f);

    AlertedTextColor = FLinearColor::Red;
    AlertedTextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.2f);
    AlertedText = FText::FromString("!");

    InvestigatingTextColor = FLinearColor::FromSRGBColor(FColor::Orange);
    InvestigatingTextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.2f);
    InvestigatingText = FText::FromString("#");

    CarryingItemTextColor = FLinearColor::Blue;
    CarryingItemTextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.2f);
    CarryingItemText = FText::FromString("*");

    GoingBackTextColor = FLinearColor::FromSRGBColor(FColor::Cyan);
    GoingBackTextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.2f);
    GoingBackText = FText::FromString(">>");

    Font = TTF_FONT("LiberationSans-Bold", 50);
}

TSharedRef<SWidget> UTAIStateWidget::RebuildWidget()
{
    UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

    if (!RootWidget && WidgetTree)
    {
        RootWidget = WidgetTree->ConstructWidget<UCanvasPanel>(
                    UCanvasPanel::StaticClass(), TEXT("RootWidget"));
        WidgetTree->RootWidget = RootWidget;
    }

    TSharedRef<SWidget> Widget = Super::RebuildWidget();

    if (RootWidget && WidgetTree)
    {
        TextBlock = WidgetTree->ConstructWidget<UTextBlock>(
                    UTextBlock::StaticClass(), TEXT("TextBlock"));
        if (TextBlock)
        {
            FTextBlockStyle* Style = new FTextBlockStyle();
            TextBlock->SetFont(Font);
            TextBlock->SetJustification(ETextJustify::Center);
            TextBlock->SetVisibility(ESlateVisibility::Visible);
            TextBlock->SynchronizeProperties();
        }

        UCanvasPanelSlot* TextBlockSlot = Cast<UCanvasPanelSlot>(
                    RootWidget->AddChild(TextBlock));
        if (TextBlockSlot) {
            TextBlockSlot->SetAutoSize(true);
            TextBlockSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
            TextBlockSlot->SetAlignment(FVector2D(0.0f, 0.0f));
            TextBlockSlot->ZOrder = 0;
            TextBlockSlot->SynchronizeProperties();
        }
    }

    return Widget;
}

void UTAIStateWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (AICharacter && TextBlock)
    {
        const EAIState& AIState = AICharacter->GetAIState();
        if (AIState == EAIState::Idle)
        {
            TextBlock->SetColorAndOpacity(IdleTextColor);
            TextBlock->SetShadowColorAndOpacity(IdleTextShadowColor);
            TextBlock->SetText(IdleText);
        }
        else if (AIState == EAIState::Suspicious)
        {
            TextBlock->SetColorAndOpacity(SuspiciousTextColor);
            TextBlock->SetShadowColorAndOpacity(SuspiciousTextShadowColor);
            TextBlock->SetText(SuspiciousText);
        }
        else if (AIState == EAIState::Alerted)
        {
            TextBlock->SetColorAndOpacity(AlertedTextColor);
            TextBlock->SetShadowColorAndOpacity(AlertedTextShadowColor);
            TextBlock->SetText(AlertedText);
        }
        else if (AIState == EAIState::Investigating)
        {
            TextBlock->SetColorAndOpacity(InvestigatingTextColor);
            TextBlock->SetShadowColorAndOpacity(InvestigatingTextShadowColor);
            TextBlock->SetText(InvestigatingText);
        }
        else if (AIState == EAIState::CarryingItem)
        {
            TextBlock->SetColorAndOpacity(CarryingItemTextColor);
            TextBlock->SetShadowColorAndOpacity(CarryingItemTextShadowColor);
            TextBlock->SetText(CarryingItemText);
        }
        else if (AIState == EAIState::GoingBack)
        {
            TextBlock->SetColorAndOpacity(GoingBackTextColor);
            TextBlock->SetShadowColorAndOpacity(GoingBackTextShadowColor);
            TextBlock->SetText(GoingBackText);
        }
        else
        {
            TextBlock->SetText(FText::FromString(""));
        }

        TextBlock->SynchronizeProperties();
    }
}
