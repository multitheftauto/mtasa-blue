/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Misc.hpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "SharedUtil.Logging.h"
#include <set>
#include "SharedUtil.Misc.h"
#include "SharedUtil.File.h"
#include "SharedUtil.Time.h"

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef MTA_DEBUG

namespace SharedUtil
{
    //
    // Add tags here to hide permanently
    //
    void InitDebugTags()
    {
        SetDebugTagHidden("Updater");
        SetDebugTagHidden("Shader");
        SetDebugTagHidden("Browser");
        SetDebugTagHidden("RPC");
        SetDebugTagHidden("Bass");
        SetDebugTagHidden("Database");
        SetDebugTagHidden("AsyncLoading");
        SetDebugTagHidden("ClientSound");
        SetDebugTagHidden("Lua");
        SetDebugTagHidden("Sync");
        SetDebugTagHidden("DEBUG EVENT");
        SetDebugTagHidden("ReportLog");
        SetDebugTagHidden("InstanceCount");
        SetDebugTagHidden("ModelLoading");
        SetDebugTagHidden("Models");
        SetDebugTagHidden("CEGUI");
        SetDebugTagHidden("LogEvent");
        SetDebugTagHidden("Mem");

        #ifdef Has_InitDebugTagsLocal
        InitDebugTagsLocal();
        #endif
    }

    static std::set<SString> ms_debugTagInvisibleMap;
};            // namespace SharedUtil

//
// Set hiddeness of a debug tag
//
void SharedUtil::SetDebugTagHidden(const SString& strTag, bool bHidden)
{
    if (bHidden)
        MapInsert(ms_debugTagInvisibleMap, strTag.ToLower());
    else
        MapRemove(ms_debugTagInvisibleMap, strTag.ToLower());
}

//
// Check is a debug tag is hidden
//
bool SharedUtil::IsDebugTagHidden(const SString& strTag)
{
    static bool bDoneInitTags = false;
    if (!bDoneInitTags)
    {
        bDoneInitTags = true;
        InitDebugTags();
    }
    return MapContains(ms_debugTagInvisibleMap, strTag.ToLower());
}

//
// Output timestamped line into the debugger
//
void SharedUtil::OutputDebugLine(const char* szMessage)
{
    if (szMessage[0] == '[')
    {
        // Filter test
        const char* szEnd = strchr(szMessage, ']');
        if (szEnd && IsDebugTagHidden(std::string(szMessage + 1, szEnd - szMessage - 1)))
            return;
    }

    SString strMessage = GetLocalTimeString(false, true) + " - " + szMessage;
    if (strMessage.length() > 0 && strMessage[strMessage.length() - 1] != '\n')
        strMessage += "\n";
#ifdef _WIN32
    OutputDebugString(strMessage);
#else
        // Other platforms here
#endif
}

namespace SharedUtil
{
    struct SDebugCountInfo
    {
        SDebugCountInfo() : iCount(0), iHigh(0) {}
        int iCount;
        int iHigh;
    };
    static std::map<SString, SDebugCountInfo> ms_DebugCountMap;

    void DebugCreateCount(const SString& strName)
    {
        SDebugCountInfo& info = MapGet(ms_DebugCountMap, strName);
        info.iCount++;
        if (info.iCount > info.iHigh)
        {
            info.iHigh = info.iCount;
            OutputDebugLine(SString("[InstanceCount] New high of %d for %s", info.iCount, *strName));
        }
    }

    void DebugDestroyCount(const SString& strName)
    {
        SDebugCountInfo& info = MapGet(ms_DebugCountMap, strName);
        info.iCount--;
        if (info.iCount < 0)
            OutputDebugLine(SString("[InstanceCount] Count is negative (%d) for %s", info.iCount, *strName));
    }
}            // namespace SharedUtil

#endif  // MTA_DEBUG

//
// Output timestamped line into the debugger
//
void SharedUtil::OutputReleaseLine(const char* szMessage)
{
    SString strMessage = GetLocalTimeString(false, true) + " - " + szMessage;
    if (strMessage.length() > 0 && strMessage[strMessage.length() - 1] != '\n')
        strMessage += "\n";
#ifdef _WIN32
    OutputDebugString(strMessage);
#else
        // Other platforms here
#endif
}

//
// Cycle (log) files when they reach a certain size.
//
// uiCycleThreshKB - 0 = never cycle   1 = always cycle   >1 cycle when main file reaches this size
//
void SharedUtil::CycleFile(const SString& strPathFilename, uint uiCycleThreshKB, uint uiNumBackups)
{
    if (uiCycleThreshKB == 0)
        return;

    if (uiCycleThreshKB == 1 || FileSize(strPathFilename) / 1024 > uiCycleThreshKB)
    {
        for (uint i = 0; i < uiNumBackups; i++)
        {
            // Rename older files .1 .2 etc
            uint    uiNew = uiNumBackups - 1 - i;
            uint    uiOld = uiNumBackups - i;
            SString strFilenameNewer = strPathFilename + (uiNew ? SString(".%d", uiNew) : "");
            SString strFilenameOlder = strPathFilename + (uiOld ? SString(".%d", uiOld) : "");

            FileDelete(strFilenameOlder);
            FileRename(strFilenameNewer, strFilenameOlder);
            FileDelete(strFilenameNewer);
        }

        FileDelete(strPathFilename);
    }
}
