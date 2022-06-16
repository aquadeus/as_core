////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GitRevision.h"
#include "CompilerDefs.h"
#include "revision_data.h"

char const* GitRevision::GetHash()
{
    return _HASH;
}

char const* GitRevision::GetDate()
{
    return _DATE;
}

char const* GitRevision::GetBranch()
{
    return _BRANCH;
}

char const* GitRevision::GetSourceDirectory()
{
    return _SOURCE_DIRECTORY;
}

char const* GitRevision::GetMySQLExecutable()
{
    return _MYSQL_EXECUTABLE;
}

char const* GitRevision::GetFullDatabase()
{
    return _FULL_DATABASE;
}

char const* GitRevision::GetHotfixesDatabase()
{
    return _HOTFIXES_DATABASE;
}

#define _PACKAGENAME "Firestorm"

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
#  ifdef _WIN64
#    define TRINITY_PLATFORM_STR "Win64"
#  else
#    define TRINITY_PLATFORM_STR "Win32"
#  endif
#elif TRINITY_PLATFORM == TRINITY_PLATFORM_APPLE
#  define TRINITY_PLATFORM_STR "MacOSX"
#elif TRINITY_PLATFORM == TRINITY_PLATFORM_INTEL
#  define TRINITY_PLATFORM_STR "Intel"
#else ///< TRINITY_PLATFORM_UNIX
#  define TRINITY_PLATFORM_STR "Unix"
#endif

char const* GitRevision::GetFullVersion()
{
    return "Firestorm rev. " VER_PRODUCTVERSION_STR
    " (" TRINITY_PLATFORM_STR ", " _BUILD_DIRECTIVE ")";
}

char const* GitRevision::GetCompanyNameStr()
{
    return VER_COMPANYNAME_STR;
}

char const* GitRevision::GetLegalCopyrightStr()
{
    return VER_LEGALCOPYRIGHT_STR;
}

char const* GitRevision::GetFileVersionStr()
{
    return VER_FILEVERSION_STR;
}

char const* GitRevision::GetProductVersionStr()
{
    return VER_PRODUCTVERSION_STR;
}
