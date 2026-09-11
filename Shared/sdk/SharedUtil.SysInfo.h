/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.SysInfo.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#ifdef WIN32
    #include <vector>
    #include "SString.h"
    #include <windows.h>
    #include "WinVer.h"

namespace SharedUtil
{
    struct SQueryWMIResult : public std::vector<std::vector<SString> >
    {
    };

    struct SLibVersionInfo : VS_FIXEDFILEINFO
    {
        int     GetFileVersionMajor() { return HIWORD(dwFileVersionMS); }
        int     GetFileVersionMinor() { return LOWORD(dwFileVersionMS); }
        int     GetFileVersionBuild() { return HIWORD(dwFileVersionLS); }
        int     GetFileVersionRelease() { return LOWORD(dwFileVersionLS); }
        SString GetFileVersionString()
        {
            return SString("%d.%d.%d.%d", GetFileVersionMajor(), GetFileVersionMinor(), GetFileVersionBuild(), GetFileVersionRelease());
        }
        SString strCompanyName;
        SString strProductName;
    };

    struct SCPUInfo
    {
        SString      strName;
        unsigned int uiMaxClockSpeedMHz = 0;
        unsigned int uiCores = 0;
        unsigned int uiThreads = 0;
        unsigned int uiL1CacheKB = 0;
        unsigned int uiL2CacheKB = 0;
        unsigned int uiL3CacheKB = 0;
    };

    bool         QueryWMI(SQueryWMIResult& outResult, const SString& strQuery, const SString& strKeys, const SString& strNamespace = "CIMV2");
    SString      GetWMIOSVersion();
    unsigned int GetWMIVideoAdapterMemorySize(const unsigned long ulVen, const unsigned long ulDev);
    long long    GetWMITotalPhysicalMemory();
    void         GetWMIAntiVirusStatus(std::vector<SString>& outEnabledList, std::vector<SString>& outDisabledList);
    void         GetInstalledHotFixList(std::vector<SString>& outInstalledList);
    bool         IsHotFixInstalled(const SString& strHotFixId);
    bool         GetLibVersionInfo(const SString& strLibName, SLibVersionInfo* pOutLibVersionInfo);
    bool         Is64BitOS();
    void         GetCPUInfo(SCPUInfo& outInfo);
}  // namespace SharedUtil

#endif
