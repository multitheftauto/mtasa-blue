/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CExePatchedStatus.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

//////////////////////////////////////////////////////////
//
// GetExePatchedStatus
//
// Check which patches are currently applied
//
//////////////////////////////////////////////////////////
SExePatchedStatus GetExePatchedStatus(bool bUseExeCopy)
{
    SString strGTAEXEPath = GetExePathFilename(bUseExeCopy);

    SExePatchedStatus status;
    status.bTimestamp = UpdatePatchStatusTimestamp(strGTAEXEPath, PATCH_CHECK) == PATCH_CHECK_RESULT_ON;
    status.bLargeMem = UpdatePatchStatusLargeMem(strGTAEXEPath, PATCH_CHECK) == PATCH_CHECK_RESULT_ON;
    status.bDep = UpdatePatchStatusDep(strGTAEXEPath, PATCH_CHECK) == PATCH_CHECK_RESULT_ON;
    status.bNvightmare = UpdatePatchStatusNvightmare(strGTAEXEPath, PATCH_CHECK) == PATCH_CHECK_RESULT_ON;
    status.bAltModules = UpdatePatchStatusAltModules(strGTAEXEPath, PATCH_CHECK) == PATCH_CHECK_RESULT_ON;
    status.bEntryPoint = UpdatePatchStatusEntryPoint(strGTAEXEPath, PATCH_CHECK) == PATCH_CHECK_RESULT_ON;

    return status;
}

//////////////////////////////////////////////////////////
//
// GetExePatchRequirements
//
// Check which patches should be applied
//
//////////////////////////////////////////////////////////
SExePatchedStatus GetExePatchRequirements()
{
    SExePatchedStatus status;
    status.bTimestamp = GetApplicationSettingInt("aero-enabled") ? true : false;
    status.bLargeMem = true;
    status.bDep = GetGtaFileVersion(GetExePathFilename(false)) != EGtaFileVersion::Encrypted;
    status.bNvightmare = GetApplicationSettingInt("nvhacks", "optimus-export-enablement") ? true : false;
    status.bAltModules = GetPatchRequirementAltModules();
    status.bEntryPoint = GetPatchRequirementEntryPoint();

    return status;
}

//////////////////////////////////////////////////////////
//
// SetExePatchedStatus
//
// Apply selected patches to exe
// Return false if needs admin access
//
//////////////////////////////////////////////////////////
bool SetExePatchedStatus(bool bUseExeCopy, const SExePatchedStatus& status)
{
    SString strGTAEXEPath = GetExePathFilename(bUseExeCopy);

    bool bReqAdmin = false;
    bReqAdmin |= UpdatePatchStatusTimestamp(strGTAEXEPath, status.bTimestamp ? PATCH_SET_ON : PATCH_SET_OFF) == PATCH_SET_RESULT_REQ_ADMIN;
    bReqAdmin |= UpdatePatchStatusLargeMem(strGTAEXEPath, status.bLargeMem ? PATCH_SET_ON : PATCH_SET_OFF) == PATCH_SET_RESULT_REQ_ADMIN;
    bReqAdmin |= UpdatePatchStatusDep(strGTAEXEPath, status.bDep ? PATCH_SET_ON : PATCH_SET_OFF) == PATCH_SET_RESULT_REQ_ADMIN;
    bReqAdmin |= UpdatePatchStatusNvightmare(strGTAEXEPath, status.bNvightmare ? PATCH_SET_ON : PATCH_SET_OFF) == PATCH_SET_RESULT_REQ_ADMIN;
    bReqAdmin |= UpdatePatchStatusAltModules(strGTAEXEPath, status.bAltModules ? PATCH_SET_ON : PATCH_SET_OFF) == PATCH_SET_RESULT_REQ_ADMIN;
    bReqAdmin |= UpdatePatchStatusEntryPoint(strGTAEXEPath, status.bEntryPoint ? PATCH_SET_ON : PATCH_SET_OFF) == PATCH_SET_RESULT_REQ_ADMIN;

    return !bReqAdmin;
}

//////////////////////////////////////////////////////////
//
// ShouldUseExeCopy
//
// Returns true if patches should be applied to exe copy
//
//////////////////////////////////////////////////////////
bool ShouldUseExeCopy()
{
    SString strUseCopyReason;
    if (GetApplicationSettingInt("nvhacks", "optimus"))
        strUseCopyReason = GetApplicationSettingInt("nvhacks", "optimus-rename-exe") == 0 ? "" : "optimus-rename-exe";
    else
        strUseCopyReason = GetApplicationSettingInt("driver-overrides-disabled") == 0 ? "" : "driver-overrides-disabled";

    if (GetPatchRequirementAltModules())
        strUseCopyReason += " AltModules";

    if (GetPatchRequirementEntryPoint())
        strUseCopyReason += " EntryPoint";

    if (RequiresAltTabFix())
        strUseCopyReason += " AltTabFix";

    // Log reason for using proxy_sa
    static SString strUseCopyReasonPrevious;
    if (strUseCopyReasonPrevious != strUseCopyReason)
    {
        WriteDebugEventAndReport(3500, SString("Using proxy_sa because: %s", *strUseCopyReason));
        strUseCopyReasonPrevious = strUseCopyReason;
    }
    return !strUseCopyReason.empty();
}

//////////////////////////////////////////////////////////
//
// RequiresAltTabFix
//
// Return true if there might be an alt-tab black screen problem when using gta_sa.exe
//
//////////////////////////////////////////////////////////
bool RequiresAltTabFix()
{
    // Exception for optimus because of better hi-perf detection when using gta_sa.exe
    if (GetApplicationSettingInt("nvhacks", "optimus"))
        return false;

    // Check for problem combo of: Windows 10 + NVidia card + full screen
    if (IsWindows10OrGreater() && GetApplicationSettingInt("nvhacks", "nvidia"))
    {
        // Slighty hacky way of checking in-game settings
        SString strCoreConfig;
        FileLoad(CalcMTASAPath(PathJoin("mta", "config", "coreconfig.xml")), strCoreConfig);
        int iWindowed = atoi(strCoreConfig.SplitRight("<display_windowed>"));
        int iFullscreenStyle = atoi(strCoreConfig.SplitRight("<display_fullscreen_style>"));
        if (iWindowed == 0 && iFullscreenStyle == 0)            // 0=FULLSCREEN_STANDARD
            return true;
    }
    return false;
}

//////////////////////////////////////////////////////////
//
// GetPatchExeAdminReason
//
// Get reason for user message
//
//////////////////////////////////////////////////////////
SString GetPatchExeAdminReason(bool bUseExeCopy, const SExePatchedStatus& reqStatus)
{
    // Get current status
    SExePatchedStatus status = GetExePatchedStatus(bUseExeCopy);

    // See what needs doing
    if (status.bTimestamp != reqStatus.bTimestamp)
        return _("Update Aero setting");
    if (status.bLargeMem != reqStatus.bLargeMem)
        return _("Update Large Memory setting");
    if (status.bDep != reqStatus.bDep)
        return _("Update DEP setting");
    if (status.bNvightmare != reqStatus.bNvightmare)
        return _("Update graphics driver compliance");
    if (status.bAltModules != reqStatus.bAltModules)
        return _("Fix file issues");
    if (status.bEntryPoint != reqStatus.bEntryPoint)
        return _("Fix file issues");
    return _("Copy main executable to avoid graphic driver issues");
}

//////////////////////////////////////////////////////////
//
// GetExeFileSize
//
//
//
//////////////////////////////////////////////////////////
uint64 GetExeFileSize(bool bUseExeCopy)
{
    SString strGTAEXEPath = GetExePathFilename(bUseExeCopy);
    return FileSize(strGTAEXEPath);
}

//////////////////////////////////////////////////////////
//
// CopyExe
//
// Return false if needs admin access
//
//////////////////////////////////////////////////////////
bool CopyExe()
{
    SString strGTAEXEPathFrom = GetExePathFilename(false);
    SString strGTAEXEPathTo = GetExePathFilename(true);
    if (strGTAEXEPathFrom != strGTAEXEPathTo)
        if (!FileCopy(strGTAEXEPathFrom, strGTAEXEPathTo))
            return false;
    return true;
}

//////////////////////////////////////////////////////////
//
// GetExePathFilename
//
// Return full path and filename of main or copy exe
//
//////////////////////////////////////////////////////////
SString GetExePathFilename(bool bUseExeCopy)
{
    SString strGTAPath = GetGTAPath();
    if (!strGTAPath.empty())
    {
        const char* szExeName = bUseExeCopy ? MTA_HTAEXE_NAME : MTA_GTAEXE_NAME;
        SString     strGTAEXEPath = PathJoin(strGTAPath, szExeName);
        return strGTAEXEPath;
    }
    return "unknown";
}

//////////////////////////////////////////////////////////
//
// GetUsingExePathFilename
//
// Return full path and filename of exe we will probably be using
//
//////////////////////////////////////////////////////////
SString GetUsingExePathFilename()
{
    return GetExePathFilename(ShouldUseExeCopy());
}

//////////////////////////////////////////////////////////
//
// UpdatePatchStatusTimestamp
//
// Change the link timestamp in gta_sa.exe to trick windows 7 into using aero
//
//////////////////////////////////////////////////////////
EPatchResult UpdatePatchStatusTimestamp(const SString& strGTAEXEPath, EPatchMode mode)
{
    SPEFileOffsets fileOffsets;
    GetPEFileOffsets(fileOffsets, strGTAEXEPath);

    // Get the top byte of the file link timestamp
    uchar ucTimeStamp = 0;
    ReadFileValue(strGTAEXEPath, ucTimeStamp, fileOffsets.TimeDateStamp + 3);

    const uchar AERO_DISABLED = 0x42;
    const uchar AERO_ENABLED = 0x43;

    // Return status if just checking
    if (mode == PATCH_CHECK)
    {
        if (ucTimeStamp == AERO_ENABLED)
            return PATCH_CHECK_RESULT_ON;
        return PATCH_CHECK_RESULT_OFF;
    }

    // Check it's a value we're expecting
    bool bCanChangeAeroSetting = (ucTimeStamp == AERO_DISABLED || ucTimeStamp == AERO_ENABLED);
    SetApplicationSettingInt("aero-changeable", bCanChangeAeroSetting);

    if (bCanChangeAeroSetting)
    {
        // Get option to set
        bool  bAeroEnabled = (mode == PATCH_SET_ON);
        uchar ucTimeStampRequired = bAeroEnabled ? AERO_ENABLED : AERO_DISABLED;
        if (ucTimeStamp != ucTimeStampRequired)
        {
            // Change needed!
            if (!WriteFileValue(strGTAEXEPath, ucTimeStampRequired, fileOffsets.TimeDateStamp + 3))
            {
                return PATCH_SET_RESULT_REQ_ADMIN;
            }
        }
    }
    return PATCH_SET_RESULT_OK;
}

//////////////////////////////////////////////////////////
//
// UpdatePatchStatusLargeMem
//
// Change the PE header to give GTA access to more memory
//
//////////////////////////////////////////////////////////
EPatchResult UpdatePatchStatusLargeMem(const SString& strGTAEXEPath, EPatchMode mode)
{
    SPEFileOffsets fileOffsets;
    GetPEFileOffsets(fileOffsets, strGTAEXEPath);
    ushort usCharacteristics = 0;
    ReadFileValue(strGTAEXEPath, usCharacteristics, fileOffsets.Characteristics);

    const ushort LARGEMEM_DISABLED = 0x10f;
    const ushort LARGEMEM_ENABLED = 0x12f;

    // Return status if just checking
    if (mode == PATCH_CHECK)
    {
        if (usCharacteristics == LARGEMEM_ENABLED)
            return PATCH_CHECK_RESULT_ON;
        return PATCH_CHECK_RESULT_OFF;
    }

    // Check it's a value we're expecting
    bool bCanChangeLargeMemSetting = (usCharacteristics == LARGEMEM_DISABLED || usCharacteristics == LARGEMEM_ENABLED);

    if (bCanChangeLargeMemSetting)
    {
        ushort usCharacteristicsRequired = (mode == PATCH_SET_ON) ? LARGEMEM_ENABLED : LARGEMEM_DISABLED;
        dassert(usCharacteristicsRequired == LARGEMEM_ENABLED);
        if (usCharacteristics != usCharacteristicsRequired)
        {
            // Change needed!
            if (!WriteFileValue(strGTAEXEPath, usCharacteristicsRequired, fileOffsets.Characteristics))
            {
                return PATCH_SET_RESULT_REQ_ADMIN;
            }
        }
    }
    else
    {
        WriteDebugEventAndReport(
            9805, SString("Unable to set LARGE_ADDRESS_AWARE [FilePosition=0x%x Characteristics=0x%x]", fileOffsets.Characteristics, usCharacteristics));
    }
    return PATCH_SET_RESULT_OK;
}

//////////////////////////////////////////////////////////
//
// UpdatePatchStatusDep
//
// Change the PE header to enable DEP
//
//////////////////////////////////////////////////////////
EPatchResult UpdatePatchStatusDep(const SString& strGTAEXEPath, EPatchMode mode)
{
    SPEFileOffsets fileOffsets;
    GetPEFileOffsets(fileOffsets, strGTAEXEPath);

    // Get the value from the header
    ushort usDllCharacteristics = 0;
    ReadFileValue(strGTAEXEPath, usDllCharacteristics, fileOffsets.DllCharacteristics);

    const ushort DEP_DISABLED = 0x0000;
    const ushort DEP_ENABLED = 0x0100;

    // Return status if just checking
    if (mode == PATCH_CHECK)
    {
        if (usDllCharacteristics == DEP_ENABLED)
            return PATCH_CHECK_RESULT_ON;
        return PATCH_CHECK_RESULT_OFF;
    }

    // Check it's a value we're expecting
    bool bCanChangeDepSetting = (usDllCharacteristics == DEP_DISABLED || usDllCharacteristics == DEP_ENABLED);

    if (bCanChangeDepSetting)
    {
        ushort usDllCharacteristicsRequired = (mode == PATCH_SET_ON) ? DEP_ENABLED : DEP_DISABLED;
        if (usDllCharacteristics != usDllCharacteristicsRequired)
        {
            // Change needed!
            if (!WriteFileValue(strGTAEXEPath, usDllCharacteristicsRequired, fileOffsets.DllCharacteristics))
            {
                return PATCH_SET_RESULT_REQ_ADMIN;
            }
        }
    }
    return PATCH_SET_RESULT_OK;
}

//////////////////////////////////////////////////////////
//
// UpdatePatchStatusNvightmare
//
// Change the PE header to export 'NvOptimusEnablement'
//
//////////////////////////////////////////////////////////
namespace
{
    #define EU_VERSION_BYTE 0x004A1AA0     // Zero if US version

    uint oldExportDir[] = {0, 0};
    uint newExportDir[] = {0x004a32d0, 0x00000060};
    uint oldExportTable[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint newExportTable[] = {0x00000000, 0x51a9df70, 0x00000000, 0x004a3302,             //  ....pߩQ.....3J.
                             0x00000001, 0x00000001, 0x00000001, 0x004a32f8,             //  ............<F8>2J.
                             0x004a32fc, 0x004a3300, 0x004c6988, 0x004a3317,             //  <FC>/<A3>..3J.<88>iL..3J.
                             0x74670000, 0x61735f61, 0x6578652e, 0x00000000,             //  ..gta_sa.exe....
                             0x00000000, 0x4e000000, 0x74704f76, 0x73756d69,             //  .......NvOptimus
                             0x62616e45, 0x656d656c, 0x0000746e, 0x00000000};            //  Enablement......

    static_assert(sizeof(oldExportDir) == sizeof(newExportDir), "Invalid export dir size");
    static_assert(sizeof(oldExportTable) == sizeof(newExportTable), "Invalid export table size");

    struct SDataumRow
    {
        uint  uiFileOffsetUS;
        uint  uiFileOffsetEU;
        void* pOldData;
        void* pNewData;
        uint  uiDataSize;
    };

    // List of patchlets
    SDataumRow datumList[] = {
        {
            0x004A1AD0,
            0x004A1ED0,
            oldExportTable,
            newExportTable,
            sizeof(newExportTable),
        },
        {
            0x000000F8,
            0x000000F8,
            oldExportDir,
            newExportDir,
            sizeof(newExportDir),
        },
    };

    SDataumRow valueItem = {0x004C4588, 0x004C4988, NULL, NULL, 0};
}            // namespace

EPatchResult UpdatePatchStatusNvightmare(const SString& strGTAEXEPath, EPatchMode mode)
{
    char  bIsEUVersion = false;
    bool  bHasExportTable = true;
    uint  uiExportValue = 0;
    bool  bUnknownBytes = false;
    FILE* fh = File::Fopen(strGTAEXEPath, "rb");
    bool  bFileError = (fh == NULL);
    if (!bFileError)
    {
        // Determine version
        bFileError |= (fseek(fh, EU_VERSION_BYTE, SEEK_SET) != 0);
        bFileError |= (fread(&bIsEUVersion, 1, 1, fh) != 1);

        // Determine patched status
        for (uint i = 0; i < NUMELMS(datumList); i++)
        {
            const SDataumRow& row = datumList[i];
            uint              uiFileOffset = bIsEUVersion ? row.uiFileOffsetEU : row.uiFileOffsetUS;

            bFileError |= (fseek(fh, uiFileOffset, SEEK_SET) != 0);

            std::vector<char> buffer(row.uiDataSize);
            bFileError |= (fread(&buffer[0], row.uiDataSize, 1, fh) != 1);

            if (memcmp(&buffer[0], row.pOldData, row.uiDataSize) == 0)
                bHasExportTable = false;
            else if (memcmp(&buffer[0], row.pNewData, row.uiDataSize) != 0)
                bUnknownBytes = true;
        }
        // Determine export value
        {
            uint uiFileOffset = bIsEUVersion ? valueItem.uiFileOffsetEU : valueItem.uiFileOffsetUS;
            bFileError |= (fseek(fh, uiFileOffset, SEEK_SET) != 0);
            bFileError |= (fread(&uiExportValue, sizeof(uiExportValue), 1, fh) != 1);
        }
        fclose(fh);
    }

    // Return status if just checking
    if (mode == PATCH_CHECK)
    {
        if (bHasExportTable && uiExportValue == 1)
            return PATCH_CHECK_RESULT_ON;
        return PATCH_CHECK_RESULT_OFF;
    }

    if (bFileError)
        WriteDebugEventAndReport(9801, "Nvightmare patch: Can not apply due to unknown file error");
    else if (bUnknownBytes)
        WriteDebugEventAndReport(9802, "Nvightmare patch: Can not apply due to unknown file bytes");
    else
    {
        // Determine if change required
        bool bReqExportTable = (mode == PATCH_SET_ON);
        uint uiReqExportValue = 1;
        if (bReqExportTable == bHasExportTable && uiReqExportValue == uiExportValue)
        {
            if (bReqExportTable)
                WriteDebugEvent(SString("Nvightmare patch:  Already applied ExportValue of %d", uiReqExportValue));
        }
        else
        {
            // Change needed!
            SetFileAttributes(strGTAEXEPath, FILE_ATTRIBUTE_NORMAL);
            FILE* fh = File::Fopen(strGTAEXEPath, "r+b");
            if (!fh)
            {
                return PATCH_SET_RESULT_REQ_ADMIN;
            }

            bool bFileError = false;
            // Write patches
            if (bReqExportTable != bHasExportTable)
            {
                WriteDebugEvent(SString("Nvightmare patch: Changing HasExportTable to %d", bReqExportTable));
                for (uint i = 0; i < NUMELMS(datumList); i++)
                {
                    const SDataumRow& row = datumList[i];
                    uint              uiFileOffset = bIsEUVersion ? row.uiFileOffsetEU : row.uiFileOffsetUS;

                    bFileError |= (fseek(fh, uiFileOffset, SEEK_SET) != 0);
                    if (bReqExportTable)
                        bFileError |= (fwrite(row.pNewData, row.uiDataSize, 1, fh) != 1);
                    else
                        bFileError |= (fwrite(row.pOldData, row.uiDataSize, 1, fh) != 1);
                }
            }
            // Write value
            if (uiReqExportValue != uiExportValue)
            {
                WriteDebugEvent(SString("Nvightmare patch: Changing ExportValue to %d", uiReqExportValue));
                uint uiFileOffset = bIsEUVersion ? valueItem.uiFileOffsetEU : valueItem.uiFileOffsetUS;
                bFileError |= (fseek(fh, uiFileOffset, SEEK_SET) != 0);
                bFileError |= (fwrite(&uiReqExportValue, sizeof(uiReqExportValue), 1, fh) != 1);
            }

            fclose(fh);
            if (bFileError)
                WriteDebugEventAndReport(9803, "Nvightmare patch: File update completed with file errors");
            else
                WriteDebugEvent("Nvightmare patch: File update completed");
        }
    }
    return PATCH_SET_RESULT_OK;
}

//////////////////////////////////////////////////////////
//
// GetPatchRequirementAltModules
//
// Return true if checksum for some dlls will cause problems
//
//////////////////////////////////////////////////////////
bool GetPatchRequirementAltModules()
{
    // Only do file check once per launch
    static bool bDone = false;
    static bool bMismatch = false;

    if (!bDone)
    {
        SString strGTAPath = GetGTAPath();
        if (!strGTAPath.empty())
        {
            struct
            {
                const char* szMd5;
                const char* szFilename;
            } fileList[] = {{"309D860FC8137E5FE9E7056C33B4B8BE", "eax.dll"},
                            {"0602F672BA595716E64EC4040E6DE376", "ogg.dll"},
                            {"2840F08DD9753A5B13C60D6D1C165C9A", "vorbis.dll"},
                            {"2B7B803311D2B228F065C45D13E1AEB2", "vorbisfile.dll"}};

            for (uint i = 0; i < NUMELMS(fileList); i++)
            {
                SString strPathFilename = PathJoin(strGTAPath, fileList[i].szFilename);
                SString strMd5 = CMD5Hasher::CalculateHexString(strPathFilename);
                if (strMd5.CompareI(fileList[i].szMd5) == false)
                    bMismatch = true;
            }
        }
        bDone = true;

        if (bMismatch)
            WriteDebugEvent("PatchRequirementAltModules: Need to use alt modules");
    }
    return bMismatch;
}

//////////////////////////////////////////////////////////
//
// UpdatePatchStatusAltModules
//
// Change dll list to use different files
//
//////////////////////////////////////////////////////////
struct SSearchInfo
{
    uint        uiOffsetStart;
    uint        uiSearchSize;
    const char* szOldName;
    const char* szNewName;
    uint        uiFileOffset;
};

bool SearchPredicate(char a, char b)
{
    return (tolower(a) == tolower(b));
}

EPatchResult UpdatePatchStatusAltModules(const SString& strGTAEXEPath, EPatchMode mode)
{
    // List of names to check/change
    SSearchInfo searchList[] = {
        {
            0x4a0000,
            0x3000,
            "vorbisfile.dll",
            "vvof.dll",
            0,
        },
        {
            0x4a0000,
            0x3000,
            "eax.dll",
            "vea.dll",
            0,
        },
        {
            0xD96000,
            0x7000,
            "vorbisfile.dll",
            "vvof.dll",
            0,
        },
        {
            0xD96000,
            0x7000,
            "eax.dll",
            "vea.dll",
            0,
        },
    };

    uint uiNumOldNames = 0;
    uint uiNumNewNames = 0;

    FILE* fh = File::Fopen(strGTAEXEPath, "rb");
    if (fh)
    {
        for (uint i = 0; i < NUMELMS(searchList); i++)
        {
            SSearchInfo& item = searchList[i];
            if (!fseek(fh, item.uiOffsetStart, SEEK_SET))
            {
                std::vector<char> buffer;
                buffer.resize(item.uiSearchSize);
                if (fread(&buffer[0], item.uiSearchSize, 1, fh) == 1)
                {
                    std::vector<char>::iterator it;
                    it = std::search(buffer.begin(), buffer.end(), item.szOldName, item.szOldName + strlen(item.szOldName), SearchPredicate);
                    if (it != buffer.end())
                    {
                        uiNumOldNames++;
                    }
                    else
                    {
                        it = std::search(buffer.begin(), buffer.end(), item.szNewName, item.szNewName + strlen(item.szNewName), SearchPredicate);
                        if (it != buffer.end())
                        {
                            uiNumNewNames++;
                        }
                    }

                    if (it != buffer.end())
                    {
                        char* p0 = &buffer[0];
                        char* p1 = &(*it);
                        item.uiFileOffset = item.uiOffsetStart + (uint)p1 - (uint)p0;
                    }
                }
            }
        }
        fclose(fh);
    }

    // Return status if just checking
    if (mode == PATCH_CHECK)
    {
        if (uiNumNewNames > 0)
            return PATCH_CHECK_RESULT_ON;
        return PATCH_CHECK_RESULT_OFF;
    }

    if (uiNumOldNames + uiNumNewNames != 4)
    {
        WriteDebugEventAndReport(9804, SString("UpdatePatchStatusAltModules: Can't find module names (%d,%d)", uiNumOldNames, uiNumNewNames));
    }

    if ((mode == PATCH_SET_ON && uiNumOldNames > 0) || (mode == PATCH_SET_OFF && uiNumNewNames > 0))
    {
        // Change needed!

        // Can't do this to gta_sa.exe 4 sure
        assert(!strGTAEXEPath.EndsWithI("gta_sa.exe"));

        SetFileAttributes(strGTAEXEPath, FILE_ATTRIBUTE_NORMAL);
        FILE* fh = File::Fopen(strGTAEXEPath, "r+b");
        if (!fh)
        {
            return PATCH_SET_RESULT_REQ_ADMIN;
        }

        for (uint i = 0; i < NUMELMS(searchList); i++)
        {
            SSearchInfo& item = searchList[i];
            const char*  szName = (mode == PATCH_SET_ON) ? item.szNewName : item.szOldName;

            if (item.uiFileOffset)
            {
                if (!fseek(fh, item.uiFileOffset, SEEK_SET))
                {
                    fwrite(szName, strlen(szName) + 1, 1, fh);
                }
            }
        }
        fclose(fh);

        if (mode == PATCH_SET_ON)
            WriteDebugEvent(SString("UpdatePatchStatusAltModules: Now using alt modules (%d,%d)", uiNumOldNames, uiNumNewNames));
    }

    return PATCH_SET_RESULT_OK;
}

//////////////////////////////////////////////////////////
//
// UpdatePatchStatusEntryPoint
//
// Fix AddressOfEntryPoint incase of corruption
//
//////////////////////////////////////////////////////////
EPatchResult UpdatePatchStatusEntryPoint(const SString& strGTAEXEPath, EPatchMode mode)
{
    SPEFileOffsets fileOffsets;
    GetPEFileOffsets(fileOffsets, strGTAEXEPath);

    // Get address from the file
    uint uiCurrentEntryPoint = 0;
    ReadFileValue(strGTAEXEPath, uiCurrentEntryPoint, fileOffsets.AddressOfEntryPoint);

    // Calc required address
    std::map<EGtaFileVersion, uint> versionEntryPoints = {
        {EGtaFileVersion::US, 0x0424570}, {EGtaFileVersion::EU, 0x04245B0}, {EGtaFileVersion::Encrypted, 0x0EFFE40}};
    uint uiRequiredEntryPoint = MapFindRef(versionEntryPoints, GetGtaFileVersion(strGTAEXEPath));

    if (mode == PATCH_CHECK)
    {
        if (uiCurrentEntryPoint == uiRequiredEntryPoint)
            return PATCH_CHECK_RESULT_ON;
        return PATCH_CHECK_RESULT_OFF;
    }

    // Check it's a value that should be fixed
    bool bCanChangeEntryPoint = (uiCurrentEntryPoint > 0x1170000) || (uiCurrentEntryPoint == uiRequiredEntryPoint);
    if (bCanChangeEntryPoint)
    {
        if (mode == PATCH_SET_ON && uiCurrentEntryPoint != uiRequiredEntryPoint)
        {
            // Change needed! Can't do this to gta_sa.exe
            assert(!strGTAEXEPath.EndsWithI("gta_sa.exe"));
            if (!WriteFileValue(strGTAEXEPath, uiRequiredEntryPoint, fileOffsets.AddressOfEntryPoint))
            {
                return PATCH_SET_RESULT_REQ_ADMIN;
            }
        }
    }
    else
    {
        WriteDebugEventAndReport(
            9806, SString("Unable to set AddressOfEntryPoint [FilePosition=0x%x EntryPoint=0x%x]", fileOffsets.AddressOfEntryPoint, uiCurrentEntryPoint));
    }
    return PATCH_SET_RESULT_OK;
}

//////////////////////////////////////////////////////////
//
// GetPatchRequirementEntryPoint
//
// Return true if AddressOfEntryPoint in gta_sa.exe should be fixed (in proxy_sa.exe)
//
//////////////////////////////////////////////////////////
bool GetPatchRequirementEntryPoint()
{
    // Only do file check once per launch
    static bool bDone = false;
    static bool bMismatch = false;

    if (!bDone)
    {
        SString strGTAExePath = GetExePathFilename(false);
        if (!strGTAExePath.empty())
        {
            bMismatch = (UpdatePatchStatusEntryPoint(strGTAExePath, PATCH_CHECK) == PATCH_CHECK_RESULT_OFF);
        }
        bDone = true;

        if (bMismatch)
            WriteDebugEvent("PatchRequirementEntryPoint: Need to change entry point");
    }
    return bMismatch;
}
