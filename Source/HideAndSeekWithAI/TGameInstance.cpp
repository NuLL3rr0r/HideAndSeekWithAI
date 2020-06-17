#include "TGameInstance.h"
#include "HideAndSeekWithAI.h"

#include <Kismet/GameplayStatics.h>

#include "TLog.h"

UTGameInstance::UTGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

#if WITH_EDITOR
void UTGameInstance::T_GetPlayRate()
{
    TLOG_DISPLAY(TLOG_KEY_GENERIC,
                 UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
}
#endif  /* WITH_EDITOR */

void UTGameInstance::T_SetPlayRate(const float Rate)
{
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Rate);

    TLOG_DISPLAY(TLOG_KEY_GENERIC,
                 TEXT("The play rate has been set!"),
                 UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
}

void UTGameInstance::LoadLevel(
        const FName& Name,
        bool bAbsolute, FString Options)
{
    UGameplayStatics::OpenLevel((UObject*)this, Name,
                                bAbsolute, Options);
}

void UTGameInstance::LoadLevel(
        const FString& Name,
        bool bAbsolute, FString Options)
{
    LoadLevel(FName(*Name), bAbsolute, Options);
}

void UTGameInstance::LoadLevel(
        const char* Name,
        bool bAbsolute, FString Options)
{
    LoadLevel(FName(UTF8_TO_TCHAR(Name)),
                    bAbsolute, Options);
}

void UTGameInstance::LoadLevel(
        const TSoftObjectPtr<UWorld>& Level,
        bool bAbsolute, FString Options)
{
    if (Level.IsNull()) {
        return;
    }

    LoadLevel(FName(*(Level.GetAssetName())),
                    bAbsolute, Options);
}

void UTGameInstance::RestartCurrentLevel()
{
    LoadLevel(GetWorld()->GetName(), true);
}
