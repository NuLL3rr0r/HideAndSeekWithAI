#include "TLog.h"

#include <Containers/Map.h>
#include <Engine/Engine.h>
#include <Logging/LogMacros.h>

DEFINE_LOG_CATEGORY ( Log_AI )
DEFINE_LOG_CATEGORY ( Log_Generic )
DEFINE_LOG_CATEGORY ( Log_Input )
DEFINE_LOG_CATEGORY ( Log_Player )

static constexpr float ON_SCREEN_LOG_DURATION = 8.0f;

struct TLogCore::StaticImpl
{
public:
    struct VerbosityMapper
    {
        FString Tag;
        FColor Color;

        VerbosityMapper(const FString& tag,
                        const FColor& color);
    };

public:
    TMap<TLogCore::EVerbosity, VerbosityMapper> VerbosityMap;

    bool bInitialized;

public:
    StaticImpl();
};

struct TLogCore::Impl
{
public:
    TLogCore::EVerbosity Verbosity;
    TLogCore::ECategory Category;
    uint64 Key;
    FString File;
    FString Function;
    FString Line;
};

TLogCore::StaticImplDeleter TLogCore::SPimplDeleter;
std::unique_ptr<TLogCore::StaticImpl, TLogCore::StaticImplDeleter> TLogCore::SPimpl =
        std::unique_ptr<TLogCore::StaticImpl, TLogCore::StaticImplDeleter>(
        new TLogCore::StaticImpl{}, TLogCore::SPimplDeleter);

TLogCore::TLogCore(const EVerbosity& Verbosity,
                   const ECategory& Category,
                   const uint64 Key,
                   const std::string& File,
                   const std::string& Function,
                   const int32 Line)
    : Pimpl(std::unique_ptr<Impl, ImplDeleter>(new Impl{}, PimplDeleter)),
      bAnyEntries(false)
{
#if defined ( HIDEANDSEEKWITHAI_LOGGING )
    if (!SPimpl->bInitialized)
    {
        SPimpl->VerbosityMap.Add(
                    TLogCore::EVerbosity::Fatal,
                    StaticImpl::VerbosityMapper(TEXT("FATAL"), FColor::White));
        SPimpl->VerbosityMap.Add(
                    TLogCore::EVerbosity::Error,
                    StaticImpl::VerbosityMapper(TEXT("ERROR"), FColor::Red));
        SPimpl->VerbosityMap.Add(
                    TLogCore::EVerbosity::Warning,
                    StaticImpl::VerbosityMapper(TEXT("WARNING"),
                                                FColor::Yellow));
        SPimpl->VerbosityMap.Add(
                    TLogCore::EVerbosity::Display,
                    StaticImpl::VerbosityMapper(TEXT("DISPLAY"),
                                                FColor::Green));
        SPimpl->VerbosityMap.Add(
                    TLogCore::EVerbosity::Log,
                    StaticImpl::VerbosityMapper(TEXT("LOG"), FColor::Silver));
        SPimpl->VerbosityMap.Add(
                    TLogCore::EVerbosity::Verbose,
                    StaticImpl::VerbosityMapper(TEXT("VERBOSE"),
                                                FColor::Purple));
        SPimpl->VerbosityMap.Add(
                    TLogCore::EVerbosity::VeryVerbose,
                    StaticImpl::VerbosityMapper(TEXT("VERY_VERBOSE"),
                                                FColor::Magenta));

        SPimpl->bInitialized = true;
    }

    Pimpl->Verbosity = Verbosity;
    Pimpl->Category = Category;
    Pimpl->Key = Key;
    Pimpl->File = FString(StringCast<WIDECHAR>(File.c_str()).Get());
    Pimpl->Function = FString(StringCast<WIDECHAR>(Function.c_str()).Get());
    Pimpl->Line = FString::FromInt(Line);
#endif  /* defined ( HIDEANDSEEKWITHAI_LOGGING ) */
}

TLogCore::~TLogCore()
{
#if defined ( HIDEANDSEEKWITHAI_LOGGING )
    const EVerbosity& Verbosity = Pimpl->Verbosity;
    const ECategory& Category = Pimpl->Category;
    const StaticImpl::VerbosityMapper& VerbosityMapper =
            SPimpl->VerbosityMap[Verbosity];
    const FString &Tag = VerbosityMapper.Tag;
    const FColor &Color = VerbosityMapper.Color;

    const FString Message(FString::Printf(
                              TEXT("[%s %s %s %s] %s"),
                              Tag.GetCharArray().GetData(),
                              Pimpl->File.GetCharArray().GetData(),
                              Pimpl->Function.GetCharArray().GetData(),
                              Pimpl->Line.GetCharArray().GetData(),
                              Buffer.GetCharArray().GetData()));

    /// Generic
    if (Category == ECategory::Generic)
    {
        switch(Verbosity)
        {

        case EVerbosity::Display:
        {
            UE_LOG(Log_Generic, Display, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Error:
        {
            UE_LOG(Log_Generic, Error, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Fatal:
        {
            UE_LOG(Log_Generic, Fatal, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Log:
        {
            UE_LOG(Log_Generic, Log, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Verbose:
        {
            UE_LOG(Log_Generic, Verbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::VeryVerbose:
        {
            UE_LOG(Log_Generic, VeryVerbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Warning:
        {
            UE_LOG(Log_Generic, Warning, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        }
    }

    /// AI
    if (Category == ECategory::AI)
    {
        switch(Verbosity)
        {
        case EVerbosity::Display:
        {
            UE_LOG(Log_AI, Display, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Error:
        {
            UE_LOG(Log_AI, Error, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Fatal:
        {
            UE_LOG(Log_AI, Fatal, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Log:
        {
            UE_LOG(Log_AI, Log, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Verbose:
        {
            UE_LOG(Log_AI, Verbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::VeryVerbose:
        {
            UE_LOG(Log_AI, VeryVerbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Warning:
        {
            UE_LOG(Log_AI, Warning, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        }
    }

    /// Input
    if (Category == ECategory::Input)
    {
        switch(Verbosity)
        {

        case EVerbosity::Display:
        {
            UE_LOG(Log_Input, Display, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Error:
        {
            UE_LOG(Log_Input, Error, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Fatal:
        {
            UE_LOG(Log_Input, Fatal, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Log:
        {
            UE_LOG(Log_Input, Log, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Verbose:
        {
            UE_LOG(Log_Input, Verbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::VeryVerbose:
        {
            UE_LOG(Log_Input, VeryVerbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Warning:
        {
            UE_LOG(Log_Input, Warning, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        }
    }

    /// Player
    if (Category == ECategory::Player)
    {
        switch(Verbosity)
        {
        case EVerbosity::Display:
        {
            UE_LOG(Log_Player, Display, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Error:
        {
            UE_LOG(Log_Player, Error, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Fatal:
        {
            UE_LOG(Log_Player, Fatal, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Log:
        {
            UE_LOG(Log_Player, Log, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Verbose:
        {
            UE_LOG(Log_Player, Verbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::VeryVerbose:
        {
            UE_LOG(Log_Player, VeryVerbose, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        case EVerbosity::Warning:
        {
            UE_LOG(Log_Player, Warning, TEXT("%s"),
                   Message.GetCharArray().GetData());
        }
            break;

        }
    }

    if (GEngine)
    {
        const FString OnScreenMessage(
                    FString::Printf(TEXT("[%s %s %s] %s"),
                                    Tag.GetCharArray().GetData(),
                                    Pimpl->Function.GetCharArray().GetData(),
                                    Pimpl->Line.GetCharArray().GetData(),
                                    Buffer.GetCharArray().GetData()));

        GEngine->AddOnScreenDebugMessage(Pimpl->Key, ON_SCREEN_LOG_DURATION,
                                         Color, OnScreenMessage);
    }

#endif  /* defined ( HIDEANDSEEKWITHAI_LOGGING ) */
}

TLogCore::StaticImpl::StaticImpl()
    : bInitialized(false)
{

}

TLogCore::StaticImpl::VerbosityMapper::VerbosityMapper(
        const FString& tag,
        const FColor& color)
    : Tag(tag),
      Color(color)
{

}

void TLogCore::StaticImplDeleter::operator()(TLogCore::StaticImpl* Pointer)
{
    delete Pointer;
}

void TLogCore::ImplDeleter::operator()(TLogCore::Impl* Pointer)
{
    delete Pointer;
}
