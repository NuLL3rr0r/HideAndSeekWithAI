#include "TAICharacter.h"
#include "HideAndSeekWithAI.h"

#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/WidgetComponent.h>
#include <UObject/Class.h>
#include <UObject/ConstructorHelpers.h>

#include "TAICharacterMovementComponent.h"
#include "TAIStateWidget.h"
#include "TTeamComponent.h"

ATAICharacter::ATAICharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass
            <UTAICharacterMovementComponent>
            (ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->UpdatedComponent = GetCapsuleComponent();
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    TouchSenseTrigger = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(
                this, TEXT("TouchSenseTrigger"));
    TouchSenseTrigger->SetupAttachment(RootComponent);
    TouchSenseTrigger->SetBoxExtent(FVector(60.0f, 60.0f, 60.0f), true);
    TouchSenseTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TouchSenseTrigger->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
    TouchSenseTrigger->SetCanEverAffectNavigation(false);

    /*static ConstructorHelpers::FObjectFinder<UClass> ControllerBP(
                TEXT("Blueprint'/Game/HideAndSeekWithAI/Core/Characters/NPC/"
                     "AIC_Bot.AIC_Bot'"));
    if (ControllerBP.Succeeded() && ControllerBP.Object)
    {
        AIControllerClass = ControllerBP.Object;
    }*/

    Team->SetTeamNumber(1);

    AIState = EAIState::Idle;

    AIStateWidget = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(
                this, TEXT("AIStateWidget"));
    AIStateWidget->SetupAttachment(RootComponent);
    AIStateWidget->SetCanEverAffectNavigation(false);
    AIStateWidget->SetWidgetSpace(EWidgetSpace::Screen);
    AIStateWidget->SetWidgetClass(UTAIStateWidget::StaticClass());
    AIStateWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 125.0f));
}

void ATAICharacter::BeginPlay()
{
    Super::BeginPlay();

    UTAIStateWidget* AIStateWidgetObject =
            Cast<UTAIStateWidget>(AIStateWidget->GetUserWidgetObject());
    checkf(AIStateWidgetObject, TEXT("FATAL: not an UTAIStateWidget instance!"));

    AIStateWidgetObject->SetAICharacter(this);

    TouchSenseTrigger->OnComponentBeginOverlap.AddDynamic(
                this, &ATAICharacter::OnOverlapBegins);
}

void ATAICharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    TouchSenseTrigger->OnComponentBeginOverlap.RemoveDynamic(
                this, &ATAICharacter::OnOverlapBegins);
}

void ATAICharacter::OnOverlapBegins(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult & SweepResult)
{
    (void)OverlappedComponent;
    (void)OtherComp;
    (void)OtherBodyIndex;
    (void)bFromSweep;
    (void)SweepResult;

    if (OtherActor)
    {
        if (this->OnTouchedByActor.IsBound()) {
            this->OnTouchedByActor.Broadcast(OtherActor);
        }
    }
}
