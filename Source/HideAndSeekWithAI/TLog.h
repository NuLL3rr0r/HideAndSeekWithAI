#pragma once

#include <memory>
#include <string>

#include <cstddef>
#include <cstdio>

#include <Containers/StringConv.h>
#include <Containers/UnrealString.h>
#include <CoreTypes.h>
#include <GameFramework/Actor.h>
#include <Logging/LogVerbosity.h>
#include <Math/Rotator.h>
#include <Math/Vector.h>

DECLARE_LOG_CATEGORY_EXTERN ( Log_AI, All, All );
DECLARE_LOG_CATEGORY_EXTERN ( Log_Generic, All, All );
DECLARE_LOG_CATEGORY_EXTERN ( Log_Input, All, All );
DECLARE_LOG_CATEGORY_EXTERN ( Log_Player, All, All );

static constexpr uint64 TLOG_KEY_INFINITE = static_cast<uint64>(-1);
static constexpr uint64 TLOG_KEY_AI = static_cast<uint64>(400);
static constexpr uint64 TLOG_KEY_GENERIC = static_cast<uint64>(100);
static constexpr uint64 TLOG_KEY_INPUT = static_cast<uint64>(200);
static constexpr uint64 TLOG_KEY_PLAYER = static_cast<uint64>(300);

/** This template gives template specialization errors for types that have not
 *  been specialized, yet. */
template<typename TYPE>
struct TLogString
{
    TYPE T1;
    FString T2;
    static_assert(std::is_same<decltype(T1), decltype(T2)>::value,
                  "Error: you are missing a template specialization!");
};

/** This template gives template specialization errors for types that have not
 *  been specialized, yet. */
template<typename TYPE, size_t LENGTH>
struct TLogString<TYPE[LENGTH]>
{
    TYPE T1;
    FString T2;
    static_assert(std::is_same<decltype(T1), decltype(T2)>::value,
                  "Error: you are missing a template specialization!");
};

/** Template specialization for const AACtor*. */
template <>
struct TLogString<const AActor*>
{
    static void Format(const AActor* Actor, FString& Out_String)
    {
        // Guard against NULL values!
        checkf(Actor, TEXT("FATAL: cannot log NULL actor object!"));

        Out_String = FString::Printf(TEXT("%s"), Actor->GetName().GetCharArray().GetData());
    }
};

/** Template specialization for AACtor*. */
template <>
struct TLogString<AActor*>
{
    static void Format(const AActor* Actor, FString& Out_String)
    {
        // Guard against NULL values!
        checkf(Actor, TEXT("FATAL: cannot log NULL actor object!"));

        Out_String = FString::Printf(TEXT("%s"), Actor->GetName().GetCharArray().GetData());
    }
};

/** Template specialization for const bool. */
template <>
struct TLogString<const bool>
{
    static void Format(const bool Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), (Value ? TEXT("True") : TEXT("False")));
    }
};

/** Template specialization for bool. */
template <>
struct TLogString<bool>
{
    static void Format(const bool Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), (Value ? TEXT("True") : TEXT("False")));
    }
};

/** Template specialization for const char[]. */
template< std::size_t LENGTH>
struct TLogString<const char[LENGTH]>
{
    static void Format(const char* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

/** Template specialization for char[]. */
template< std::size_t LENGTH>
struct TLogString<char[LENGTH]>
{
    static void Format(const char* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

/** Template specialization for const char*. */
template <>
struct TLogString<const char*>
{
    static void Format(const char* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

/** Template specialization for char*. */
template <>
struct TLogString<char*>
{
    static void Format(const char* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

#if defined ( __linux__ )

/** Template specialization for const char16_t[]. */
template< std::size_t LENGTH>
struct TLogString<const char16_t[LENGTH]>
{
    static void Format(const char16_t* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

/** Template specialization for char16_t[]. */
template< std::size_t LENGTH>
struct TLogString<char16_t[LENGTH]>
{
    static void Format(const char16_t* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

/** Template specialization for const char16_t*. */
template <>
struct TLogString<const char16_t*>
{
    static void Format(const char16_t* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

/** Template specialization for char16_t*. */
template <>
struct TLogString<char16_t*>
{
    static void Format(const char16_t* Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value).Get());
    }
};

#endif  /* defined ( __linux__ ) */

/** Template specialization for const double. */
template <>
struct TLogString<const double>
{
    static void Format(const double Value, FString& Out_String)
    {
        char Buffer[100];
        std::snprintf(Buffer, sizeof(Buffer), "%f", Value);

        std::string String(Buffer);

        Out_String = FString(StringCast<WIDECHAR>(String.c_str()).Get());
    }
};

/** Template specialization for double. */
template <>
struct TLogString<double>
{
    static void Format(const double Value, FString& Out_String)
    {
        char Buffer[100];
        std::snprintf(Buffer, sizeof(Buffer), "%f", Value);

        std::string String(Buffer);

        Out_String = FString(StringCast<WIDECHAR>(String.c_str()).Get());
    }
};

/** Template specialization for const float. */
template <>
struct TLogString<const float>
{
    static void Format(const float Value, FString& Out_String)
    {
        Out_String = FString::SanitizeFloat(static_cast<double>(Value), 1);
    }
};

/** Template specialization for float. */
template <>
struct TLogString<float> {
    static void Format(const float Value, FString& Out_String)
    {
        Out_String = FString::SanitizeFloat(static_cast<double>(Value), 1);
    }
};

/** Template specialization for Const FName. */
template <>
struct TLogString<const FName>
{
    static void Format(const FName& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.ToString().GetCharArray().GetData());
    }
};

/** Template specialization for FName. */
template <>
struct TLogString<FName>
{
    static void Format(const FName& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.ToString().GetCharArray().GetData());
    }
};

/** Template specialization for Const FRotator. */
template <>
struct TLogString<const FRotator>
{
    static void Format(const FRotator& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.ToString().GetCharArray().GetData());
    }
};

/** Template specialization for FRotator. */
template <>
struct TLogString<FRotator>
{
    static void Format(const FRotator& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.ToString().GetCharArray().GetData());
    }
};

/** Template specialization for const FString. */
template <>
struct TLogString<const FString>
{
    static void Format(const FString& Value, FString& Out_String)
    {
        Out_String = Value;
    }
};

/** Template specialization for FString. */
template <>
struct TLogString<FString>
{
    static void Format(const FString& Value, FString& Out_String)
    {
        Out_String = Value;
    }
};

/** Template specialization for const FVector. */
template <>
struct TLogString<const FVector>
{
    static void Format(const FVector& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.ToString().GetCharArray().GetData());
    }
};

/** Template specialization for FVector. */
template <>
struct TLogString<FVector>
{
    static void Format(const FVector& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.ToString().GetCharArray().GetData());
    }
};

/** Template specialization for const int8. */
template <>
struct TLogString<const int8>
{
    static void Format(const int8 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for int8. */
template <>
struct TLogString<int8>
{
    static void Format(const int8 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for const int16. */
template <>
struct TLogString<const int16>
{
    static void Format(const int16 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for int16. */
template <>
struct TLogString<int16>
{
    static void Format(const int16 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for const int32. */
template <>
struct TLogString<const int32>
{
    static void Format(const int32 Value, FString& Out_String)
    {
        Out_String = FString::FromInt(Value);
    }
};

/** Template specialization for int32. */
template <>
struct TLogString<int32>
{
    static void Format(const int32 Value, FString& Out_String)
    {
        Out_String = FString::FromInt(Value);
    }
};

/** Template specialization for const int64. */
template <>
struct TLogString<const int64>
{
    static void Format(const int64 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for int64. */
template <>
struct TLogString<int64>
{
    static void Format(const int64 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for const long double. */
template <>
struct TLogString<const long double>
{
    static void Format(const long double Value, FString& Out_String)
    {
        char Buffer[100];
        std::snprintf(Buffer, sizeof(Buffer), "%Lf", Value);

        std::string String(Buffer);

        Out_String = FString(StringCast<WIDECHAR>(String.c_str()).Get());
    }
};

/** Template specialization for long double. */
template <>
struct TLogString<long double>
{
    static void Format(const long double Value, FString& Out_String)
    {
        char Buffer[100];
        std::snprintf(Buffer, sizeof(Buffer), "%Lf", Value);

        std::string String(Buffer);

        Out_String = FString(StringCast<WIDECHAR>(String.c_str()).Get());
    }
};

#if defined ( __linux__ )

/** Template specialization for const std::size_t. */
template <>
struct TLogString<const std::size_t>
{
    static void Format(const std::size_t Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

#endif  /* defined ( __linux__ ) */

#if defined ( __linux__ )

/** Template specialization for std::size_t. */
template <>
struct TLogString<std::size_t>
{
    static void Format(const std::size_t Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

#endif  /* defined ( __linux__ ) */

/** Template specialization for const uint8. */
template <>
struct TLogString<const uint8>
{
    static void Format(const int8 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for uint8. */
template <>
struct TLogString<uint8>
{
    static void Format(const int8 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for const uint16. */
template <>
struct TLogString<const uint16>
{
    static void Format(const int16 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for uint16. */
template <>
struct TLogString<uint16>
{
    static void Format(const int16 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for const uint32. */
template <>
struct TLogString<const uint32>
{
    static void Format(const int32 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for uint32. */
template <>
struct TLogString<uint32>
{
    static void Format(const int32 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for const uint64. */
template <>
struct TLogString<const uint64>
{
    static void Format(const int64 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for uint64. */
template <>
struct TLogString<uint64>
{
    static void Format(const int64 Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%d"), Value);
    }
};

/** Template specialization for const std::string. */
template <>
struct TLogString<const std::string>
{
    static void Format(const std::string& Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value.c_str()).Get());
    }
};

/** Template specialization for std::string. */
template <>
struct TLogString<std::string>
{
    static void Format(const std::string& Value, FString& Out_String)
    {
        Out_String = FString(StringCast<WIDECHAR>(Value.c_str()).Get());
    }
};

/** Template specialization for const wchar_t[]. */
template<std::size_t LENGTH>
struct TLogString<const wchar_t[LENGTH]>
{
    static void Format(const wchar_t* Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value);
    }
};

/** Template specialization for wchar_t[]. */
template<std::size_t LENGTH>
struct TLogString<wchar_t[LENGTH]>
{
    static void Format(const wchar_t* Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value);
    }
};

/** Template specialization for const wchar_t*. */
template <>
struct TLogString<const wchar_t*>
{
    static void Format(const wchar_t* Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value);
    }
};

/** Template specialization for wchar_t*. */
template <>
struct TLogString<wchar_t*>
{
    static void Format(const wchar_t* Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value);
    }
};

/** Template specialization for const std::wstring. */
template <>
struct TLogString<const std::wstring>
{
    static void Format(const std::wstring& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.c_str());
    }
};

/** Template specialization for std::wstring. */
template <>
struct TLogString<std::wstring>
{
    static void Format(const std::wstring& Value, FString& Out_String)
    {
        Out_String = FString::Printf(TEXT("%s"), Value.c_str());
    }
};

/** A generic LOG class which prints logs on both screen and log file for
 *  debug or development builds and eliminates the logs automatically from
 *  release builds. */
class HIDEANDSEEKWITHAI_API TLogCore
{
public:
    /** The log verbosity. */
    enum class EVerbosity : uint8
    {
        /** Always prints s fatal error to console (and log file) and crashes
         *  (even if logging is disabled). */
        Fatal,

        /** Prints an error to console (and log file).
         *  Commandlets and the editor collect and report errors. Error messages
         *  result in commandlet failure. */
        Error,

        /** Prints a warning to console (and log file).
         *  Commandlets and the editor collect and report warnings. Warnings can
         *  be treated as an error. */
        Warning,

        /** Prints a message to console (and log file). */
        Display,

        /** Prints a message to a log file (does not print to console). */
        Log,

        /** Prints a verbose message to a log file (if Verbose logging is
         *  enabled for the given category, usually used for detailed
         *  logging). */
        Verbose,

        /** Prints a verbose message to a log file (if VeryVerbose logging is
         *  enabled, usually used for detailed logging that would otherwise spam
         *  output). */
        VeryVerbose,
    };

    /** The log categories. */
    enum class ECategory
    {
        AI,
        Generic,
        Input,
        Player
    };

private:
    /** Opaque pointer for private static stuff. */
    struct StaticImpl;
    struct StaticImplDeleter
    {
        void operator()(StaticImpl* Pointer);
    };
    static std::unique_ptr<StaticImpl, StaticImplDeleter> SPimpl;
    static StaticImplDeleter SPimplDeleter;

    /** Opaque pointer for static stuff. */
    struct Impl;
    struct ImplDeleter
    {
        void operator()(Impl* Pointer);
    };
    std::unique_ptr<Impl, ImplDeleter> Pimpl;
    ImplDeleter PimplDeleter;

private:
    FString Buffer;
    bool bAnyEntries;

public:
    TLogCore(const EVerbosity& Verbosity,
         const ECategory& Category,
         const uint64 Key,
         const std::string& File,
         const std::string& Function,
         const int32 Line);
    virtual ~TLogCore();

public:
    template <typename TYPE>
    TLogCore& operator,(const TYPE& Argument)
    {
        if (bAnyEntries)
        {
            Buffer += FString(TEXT(" • "));
        }

        FString FormattedArgument;
        TLogString<TYPE>::Format(Argument, FormattedArgument);
        Buffer += FormattedArgument;
        bAnyEntries = true;

        return *this;
    }
};

/* Generic logging macros */

#define TLOG_FATAL( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Fatal, TLogCore::ECategory::Generic, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_ERROR( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Error, TLogCore::ECategory::Generic, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_WARNING( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Warning, TLogCore::ECategory::Generic, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_DISPLAY( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Display, TLogCore::ECategory::Generic, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_LOG( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::Generic, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Verbose, TLogCore::ECategory::Generic, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_VERY_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::VeryVerbose, TLogCore::ECategory::Generic, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG( ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::Generic, TLOG_KEY_INFINITE, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

/* AI logging macros */

#define TLOG_AI_FATAL( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Fatal, TLogCore::ECategory::AI, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_AI_ERROR( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Error, TLogCore::ECategory::AI, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_AI_WARNING( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Warning, TLogCore::ECategory::AI, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_AI_DISPLAY( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Display, TLogCore::ECategory::AI, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_AI_LOG( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::AI, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_AI_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Verbose, TLogCore::ECategory::AI, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_AI_VERY_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::VeryVerbose, TLogCore::ECategory::AI, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_AI( ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::AI, TLOG_KEY_INFINITE, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

/* Input logging macros */

#define TLOG_INPUT_FATAL( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Fatal, TLogCore::ECategory::Input, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_INPUT_ERROR( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Error, TLogCore::ECategory::Input, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_INPUT_WARNING( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Warning, TLogCore::ECategory::Input, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_INPUT_DISPLAY( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Display, TLogCore::ECategory::Input, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_INPUT_LOG( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::Input, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_INPUT_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Verbose, TLogCore::ECategory::Input, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_INPUT_VERY_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::VeryVerbose, TLogCore::ECategory::Input, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_INPUT( ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::Input, TLOG_KEY_INFINITE, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

/* Player logging macros */

#define TLOG_PLAYER_FATAL( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Fatal, TLogCore::ECategory::Player, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_PLAYER_ERROR( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Error, TLogCore::ECategory::Player, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_PLAYER_WARNING( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Warning, TLogCore::ECategory::Player, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_PLAYER_DISPLAY( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Display, TLogCore::ECategory::Player, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_PLAYER_LOG( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::Player, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_PLAYER_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::Verbose, TLogCore::ECategory::Player, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_PLAYER_VERY_VERBOSE( Key, ... )  \
    (TLogCore(TLogCore::EVerbosity::VeryVerbose, TLogCore::ECategory::Player, Key, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;

#define TLOG_PLAYER( ... )  \
    (TLogCore(TLogCore::EVerbosity::Log, TLogCore::ECategory::Player, TLOG_KEY_INFINITE, __FILE__, __FUNCTION__, __LINE__)), __VA_ARGS__;
