#pragma once

#include <Containers/UnrealString.h>
#include <CoreTypes.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <UObject/ObjectMacros.h>
#include <UObject/Object.h>
#include <UObject/NameTypes.h>
#include <UObject/SoftObjectPtr.h>

#include "TGameInstance.generated.h"

/** Customized game instance */
UCLASS(config=Game, transient, BlueprintType, Blueprintable)
class HIDEANDSEEKWITHAI_API UTGameInstance : public UGameInstance
{
    GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
    /** Gets the current playing speed. */
    UFUNCTION(Exec, Category = "Hide And Seek With AI")
    void T_GetPlayRate();
#endif  /* WITH_EDITOR */

    /** Used to play the game in slow motion or if it makes sense faster than
     *  usual. */
    UFUNCTION(Exec, Category = "Hide And Seek With AI")
    void T_SetPlayRate(const float Rate);

    /** Load a level by FName. */
    void LoadLevel(const FName& Name,
                   bool bAbsolute = true, FString Options = FString(TEXT("")));

    /** Load a level by name. */
    void LoadLevel(const FString& Name,
                   bool bAbsolute = true, FString Options = FString(TEXT("")));

    /** Load a level by name. */
    void LoadLevel(const char* Name,
                   bool bAbsolute = true, FString Options = FString(TEXT("")));

    /** Load a level by passing its pointer. */
    void LoadLevel(const TSoftObjectPtr<UWorld>& Level,
                   bool bAbsolute = true, FString Options = FString(TEXT("")));

    /** Restart the current level and reload everything. */
    void RestartCurrentLevel();
};
