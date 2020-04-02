/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/Core.cpp
 *  PURPOSE:     Core library entry point
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_Core
#include "profiler/SharedUtil.Profiler.h"
#include "SharedUtil.Win32Utf8FileHooks.hpp"

CCore*         g_pCore = NULL;
CGraphics*     g_pGraphics = NULL;
CLocalization* g_pLocalization = NULL;

HINSTANCE g_hModule = NULL;

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    CFilePathTranslator FileTranslator;
    std::string         WorkingDirectory;

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        WriteDebugEvent(SString("DLL_PROCESS_ATTACH %08x", pvNothing));
        if (IsGTAProcess())
        {
            WriteDebugEvent(SString("ModuleFileName: %s", *GetLaunchPathFilename()));

            AddUtf8FileHooks();

            // Set low frag heap for XP
            ULONG heapInfo = 2;
            HeapSetInformation(GetProcessHeap(), HeapCompatibilityInformation, &heapInfo, sizeof(heapInfo));

            FileTranslator.GetGTARootDirectory(WorkingDirectory);
            SetCurrentDirectory(WorkingDirectory.c_str());

            // For dll searches, this call replaces the current directory entry and turns off 'SafeDllSearchMode'
            // Meaning it will search the supplied path before the system and windows directory.
            // http://msdn.microsoft.com/en-us/library/ms682586%28VS.85%29.aspx
            SetDllDirectory(CalcMTASAPath("MTA"));

            g_hModule = hModule;
            g_pCore = new CCore;

            FileTranslator.GetGTARootDirectory(WorkingDirectory);
            SetCurrentDirectory(WorkingDirectory.c_str());
        }
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        WriteDebugEvent(SString("DLL_PROCESS_DETACH %08x", pvNothing));
        if (IsGTAProcess())
        {
            RemoveUtf8FileHooks();

            AddReportLog(7102, "Core - PROCESS_DETACH");
            // For now, TerminateProcess if any destruction is attempted (or we'll crash)
            TerminateProcess(GetCurrentProcess(), 0);

            if (g_pCore)
            {
                delete g_pCore;
                g_pCore = NULL;
            }

            g_hModule = NULL;
        }
    }

    return TRUE;
}
