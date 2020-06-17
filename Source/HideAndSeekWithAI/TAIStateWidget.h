#pragma once

#include <Blueprint/UserWidget.h>
#include <CoreTypes.h>
#include <Fonts/SlateFontInfo.h>
#include <Templates/SharedPointer.h>
#include <UObject/ObjectMacros.h>

#include "TAIStateWidget.generated.h"

class SWidget;
class UTextBlock;

class ATAICharacter;

/** In-game UMG widget to indicate the AI state for bots shown on their
 *  heads. */
UCLASS(editinlinenew, BlueprintType, Blueprintable, meta=( DontUseGenericSpawnObject="True") )
class HIDEANDSEEKWITHAI_API UTAIStateWidget : public UUserWidget
{
    GENERATED_UCLASS_BODY()

protected:
    /** Displayed text on bot's head when they are in idle state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FText IdleText;

    /** Displayed text color on bot's head when they are in idle state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor IdleTextColor;

    /** Displayed text shadow color on bot's head when they are in idle
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor IdleTextShadowColor;

    /** Displayed text on bot's head when they are in suspicious state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FText SuspiciousText;

    /** Displayed text color on bot's head when they are in suspicious state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor SuspiciousTextColor;

    /** Displayed text shadow color on bot's head when they are in suspicious
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor SuspiciousTextShadowColor;

    /** Displayed text on bot's head when they are in alerted state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FText AlertedText;

    /** Displayed text color on bot's head when they are in alerted state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor AlertedTextColor;

    /** Displayed text shadow color on bot's head when they are in alerted
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor AlertedTextShadowColor;

    /** Displayed text on bot's head when they are in investigating state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FText InvestigatingText;

    /** Displayed text color on bot's head when they are in investigating
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor InvestigatingTextColor;

    /** Displayed text shadow color on bot's head when they are in investigating
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor InvestigatingTextShadowColor;

    /** Displayed text on bot's head when they are in carrying item state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FText CarryingItemText;

    /** Displayed text color on bot's head when they are in carrying item
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor CarryingItemTextColor;

    /** Displayed text shadow color on bot's head when they are in carrying item
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor CarryingItemTextShadowColor;

    /** Displayed text on bot's head when they are in going back state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FText GoingBackText;

    /** Displayed text color on bot's head when they are in going back state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor GoingBackTextColor;

    /** Displayed text shadow color on bot's head when they are in going back
     *  state. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FLinearColor GoingBackTextShadowColor;

    /** The font to use for all texts in this widget. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FSlateFontInfo Font;

private:
    /** The UMG text block widget that this widget uses in order to display
     *  text. */
    UTextBlock* TextBlock;

    /** The bot character this widget belongs to. */
    ATAICharacter* AICharacter;

public:
    /** Sets the bot character this widget belongs to. */
    FORCEINLINE void SetAICharacter(ATAICharacter* Character)
    {
        AICharacter = Character;
    }

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
