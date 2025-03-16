/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        launcher/Main.cpp
 *  PURPOSE:     Launcher entry
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

/*
    IMPORTANT (Windows only)

    If this project changes, a new release build should be copied into
    the Shared\data\launchers directory.

    The .exe in Shared\data\launchers will be used by the installer and updater.

    (set flag.new_server_exe on the build server to generate new exe)
*/

#include "CDynamicLibrary.h"
#include <iostream>
#include "MTAPlatform.h"
#include "SharedUtil.h"

#ifdef WIN32
// Linux gcc 4.4.5 memory corruption on destruction of g_StatEvents (Reason unknown)
    #include "SharedUtil.hpp"
#else
    #include <unistd.h>

FILE* SharedUtil::File::Fopen(const char* szFilename, const char* szMode)
{
    return fopen(szFilename, szMode);
}
#endif

#ifdef WIN32
    #define LIB_CORE SERVER_BIN_PATH "core" MTA_LIB_SUFFIX MTA_LIB_EXTENSION
#else
    #define LIB_CORE "./" SERVER_BIN_PATH "core" MTA_LIB_SUFFIX MTA_LIB_EXTENSION
#endif

int main(int argc, char* argv[])
{
    // Work out the launched directory and filename
    int   iLength = strlen(argv[0]);
    char* szLaunchDirectory = static_cast<char*>(alloca(iLength + 1));

    strncpy(szLaunchDirectory, argv[0], iLength + 1);

    for (int i = 0; i < iLength; i++)
        if (szLaunchDirectory[i] == '\\')
            szLaunchDirectory[i] = '/';

    SString strLaunchFile, strLaunchDirectory;
    if (char* cpPos = strrchr(szLaunchDirectory, '/'))
    {
        *cpPos = 0;
        strLaunchFile = cpPos + 1;
        strLaunchDirectory = szLaunchDirectory;
    }
    else
    {
        strLaunchFile = szLaunchDirectory;
        strLaunchDirectory = "";
    }

    if (argc > 1)
    {
        if (strcmp(argv[1], "/?") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
            printf("Usage: %s [OPTION]\n\n", *strLaunchFile);
            printf("  -v                   Shows the program version\n");
            printf("  -s                   Run server in silent mode\n");
#ifndef WIN32
            printf("  -d                   Run server daemonized\n");
#endif
            printf("  -t                   Run server with a simple console\n");
            printf("  -f                   Run server with a standard console (Default)\n");
            printf("  -n                   Disable the usage of ncurses (For screenlog)\n");
            printf("  -u                   Disable output buffering and flush instantly (useful for screenlog)\n");
#ifndef WIN32
            printf("  -x                   Disable simplified crash reports (To allow core dumps)\n");
            printf("  --child-process      Run server without output buffering and with a readyness event\n");
#endif
            printf("  -D [PATH]            Use as base directory\n");
            printf("  --config [FILE]      Alternate mtaserver.conf file\n");
            printf("  --ip [ADDR]          Set IP address\n");
            printf("  --port [PORT]        Set port\n");
            printf("  --httpport [PORT]    Set http port\n");
            printf("  --maxplayers [max]   Set maxplayers\n");
            printf("  --novoice            Disable voice communication\n");
            return 1;
        }
    }

#ifdef WIN32
    if (!IsWindowsXPSP3OrGreater())
    {
        printf("This version of MTA requires Windows XP SP3 or later\n");

        // Wait for a key then exit
        printf("Press enter to continue...\n");
        std::cin.get();
        return 1;
    }
#endif

    // If we are unable to access the core module, try changing to the directory of the launched file
    FILE* fh = File::Fopen(LIB_CORE, "r");
    if (!fh)
    {
        #ifdef WIN32
        wchar_t szBuffer[64000];
        GetModuleFileNameW(NULL, szBuffer, 64000);
        PathRemoveFileSpecW(szBuffer);
        SetCurrentDirectoryW(szBuffer);
        #else
        chdir(strLaunchDirectory);
        #endif
    }
    else
        fclose(fh);

    // Load the core library
    CDynamicLibrary Core;
    if (Core.Load(LIB_CORE))
    {
        // Grab the entrypoint
        typedef int(Main_t)(int, char*[]);
        Main_t* pfnEntryPoint = reinterpret_cast<Main_t*>((long long)(Core.GetProcedureAddress("Run")));
        if (pfnEntryPoint)
        {
            // Call it and return what it returns
            return pfnEntryPoint(argc, argv);
        }
        else
        {
            printf("ERROR: Bad file: %s\n", LIB_CORE);
        }
    }
    else
    {
        printf("ERROR: Could not load %s\n", LIB_CORE);
        printf("* Check installed data files.\n");
#ifdef WIN32
        printf("* Check installed Microsoft Visual C++ Redistributable Package (x86).\n");
#endif
    }

    // Wait for a key then exit
    printf("Press enter to continue...\n");
    std::cin.get();
    return 1;
}
