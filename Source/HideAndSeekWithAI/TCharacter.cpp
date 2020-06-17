#include "TCharacter.h"
#include "HideAndSeekWithAI.h"

#include <Components/ArrowComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Math/UnrealMathUtility.h>

#include "TCharacterMovementComponent.h"
#include "TPickup.h"
#include "TTeamComponent.h"

ATCharacter::ATCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass
            <UTCharacterMovementComponent>
            (ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    UCapsuleComponent* Capsule = GetCapsuleComponent();
    if (Capsule)
    {
        Capsule->InitCapsuleSize(50.f, 50.0f);
        Capsule->SetCanEverAffectNavigation(false);
    }

    StaticMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(
                this, TEXT("Mesh"));
    StaticMesh->SetupAttachment(RootComponent);
    StaticMesh->SetOnlyOwnerSee(false);
    StaticMesh->bCastDynamicShadow = true;
    StaticMesh->CastShadow = true;
    StaticMesh->SetCanEverAffectNavigation(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PawnMesh(
                TEXT("StaticMesh'/Game/StarterContent/Shapes/"
                     "Shape_Cylinder.Shape_Cylinder'"));
    if (PawnMesh.Succeeded() && PawnMesh.Object)
    {
        StaticMesh->SetStaticMesh(PawnMesh.Object);
        StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.f));
    }

    ItemAttachPoint = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(
                this, TEXT("ItemAttachPoint"));
    ItemAttachPoint->SetupAttachment(RootComponent);
    ItemAttachPoint->SetRelativeLocation(FVector(70.0f, 0.0f, 0.0f));

    Item = nullptr;

    Team = ObjectInitializer.CreateDefaultSubobject<UTTeamComponent>(
                this, TEXT("Team"));
}

void ATCharacter::PickupItem(ATPickup* Pickup)
{
    if (!Pickup)
    {
        return;
    }

    if (!CanPickupItems())
    {
        return;
    }

    Pickup->AttachToCharacter(this);
    Item = Pickup;
}

void ATCharacter::DropItem()
{
    if (HasAnyItems())
    {
        Item->DetachFromCharacter(this);
        Item->GetMesh()->AddImpulse(
                    GetControlRotation().Vector().GetSafeNormal() * 100.0f,
                    NAME_None, true);
        Item = nullptr;
    }
}

bool ATCharacter::IsMoving() const
{
    const float Speed = FVector::DotProduct(
                GetVelocity(), GetActorRotation().Vector());

    return !FMath::IsNearlyZero(Speed);
}
