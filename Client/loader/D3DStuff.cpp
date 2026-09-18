/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/D3DStuff.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "D3DStuff.h"
#include "Utils.h"
#include "Dialogs.h"
#include "../sdk/core/CrashHandlerExports.h"

#include <atomic>
#include <d3d9.h>
#include <nvapi/nvapi.h>

namespace
{
    SString GUIDToString(const GUID& g)
    {
        return SString("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3],
                       g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
    }

    SString ToString(const D3DADAPTER_IDENTIFIER9& a)
    {
        return SString(
            "    Driver:%s\n"
            "    Description:%s\n"
            "    DeviceName:%s\n"
            "    DriverVersion:0x%08x 0x%08x\n"
            "    VendorId:0x%08x  DeviceId:0x%08x  SubSysId:0x%08x  Revision:0x%08x  WHQLLevel:0x%08x\n"
            "    DeviceIdentifier:%s",
            a.Driver, a.Description, a.DeviceName, a.DriverVersion.HighPart, a.DriverVersion.LowPart, a.VendorId, a.DeviceId, a.SubSysId, a.Revision,
            a.WHQLLevel, *GUIDToString(a.DeviceIdentifier));
    }

    SString ToString(const D3DDISPLAYMODE& a)
    {
        return SString(
            " Width:%d"
            " Height:%d"
            " RefreshRate:%d"
            " Format:%d",
            a.Width, a.Height, a.RefreshRate, a.Format);
    }

    SString ToString(const D3DCAPS9& a)
    {
        return SString(
            " VertexShaderVersion:0x%08x"
            " PixelShaderVersion:0x%08x"
            " DeclTypes:0x%03x",
            a.VertexShaderVersion, a.PixelShaderVersion, a.DeclTypes);
    }

    IDirect3D9*                  pD3D9 = NULL;
    IDirect3DDevice9*            pD3DDevice9 = NULL;
    IDirect3DVertexDeclaration9* pD3DVertexDeclarations[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    using EnableHandlersFn = BOOL(WINAPI*)(void);
    std::atomic<EnableHandlersFn> g_enableHandlersImpl{nullptr};
    std::atomic<bool>             g_enableHandlersPending{false};
}  // namespace

// Loader is built without core crash handler code; defer to the implementation once core.dll is loaded.
extern "C" BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableAllHandlersAfterInitialization(void) noexcept
{
    if (EnableHandlersFn impl = g_enableHandlersImpl.load(std::memory_order_acquire))
    {
        return impl();
    }

    g_enableHandlersPending.store(true, std::memory_order_release);
    return TRUE;
}

void LoaderResolveCrashHandlerExports(HMODULE coreModule) noexcept
{
    if (!coreModule)
        return;

    auto resolved = reinterpret_cast<EnableHandlersFn>(static_cast<void*>(GetProcAddress(coreModule, "EnableAllHandlersAfterInitialization")));

    if (!resolved)
    {
        resolved = reinterpret_cast<EnableHandlersFn>(static_cast<void*>(GetProcAddress(coreModule, "_EnableAllHandlersAfterInitialization@0")));
    }

    g_enableHandlersImpl.store(resolved, std::memory_order_release);

    if (!resolved)
    {
        WriteDebugEvent("Loader - Failed to resolve EnableAllHandlersAfterInitialization export");
    }

    if (resolved && g_enableHandlersPending.exchange(false, std::memory_order_acq_rel))
    {
        resolved();
    }
}

//////////////////////////////////////////////////////////
//
// NvOptimusDetect
//
// Try detecting optimus via NvAPI
//
//////////////////////////////////////////////////////////
bool NvOptimusDetect()
{
    if (NvAPI_Initialize() != NVAPI_OK)
    {
        return false;
    }

    // Get and log driver info
    NvAPI_ShortString szDesc = "-";
    NvU32             uiDriverVersion = -1;
    NvAPI_ShortString szBuildBranchString = "-";

    NvAPI_GetInterfaceVersionString(szDesc);
    NvAPI_SYS_GetDriverAndBranchVersion(&uiDriverVersion, szBuildBranchString);
    WriteDebugEventAndReport(7460, SString("NvAPI - InterfaceVersion:'%s' DriverVersion:%d.%d Branch:'%s'", szDesc, uiDriverVersion / 100,
                                           uiDriverVersion % 100, szBuildBranchString));

    // Get all the Physical GPU Handles
    NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS] = {0};
    NvU32               uiGpuCount = 0;
    if (NvAPI_EnumPhysicalGPUs(nvGPUHandle, &uiGpuCount) != NVAPI_OK)
    {
        return false;
    }

    bool bFoundOptimus = false;
    for (NvU32 i = 0; i < uiGpuCount; i++)
    {
        NV_SYSTEM_TYPE    SystemType = (NV_SYSTEM_TYPE)-1;  // 1-Laptop 2-Desktop
        NV_GPU_TYPE       GpuType = (NV_GPU_TYPE)-1;        // 1-Integrated 2-Discrete
        NvAPI_ShortString szName = "-";

        NvAPI_GPU_GetSystemType(nvGPUHandle[i], &SystemType);
        NvAPI_GPU_GetGPUType(nvGPUHandle[i], &GpuType);
        NvAPI_GPU_GetFullName(nvGPUHandle[i], szName);
        SString strStatus("NvAPI - GPU %d/%d - SystemType:%d GpuType:%d (%s)", i, uiGpuCount, SystemType, GpuType, szName);

        if (SystemType == NV_SYSTEM_TYPE_LAPTOP && GpuType == NV_SYSTEM_TYPE_DGPU)
        {
            bFoundOptimus = true;
            strStatus += " FoundOptimus";
        }
        WriteDebugEventAndReport(7461, strStatus);
    }

    return bFoundOptimus;
}

//////////////////////////////////////////////////////////
//
// BeginD3DStuff
//
// Look all busy and important in case any graphic drivers are looking
//
//////////////////////////////////////////////////////////
void BeginD3DStuff()
{
    pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

    if (!pD3D9)
    {
        WriteDebugEvent("D3DStuff - Direct3DCreate9 failed");
        return;
    }

    WriteDebugEvent("D3DStuff -------------------------");
    WriteDebugEvent(SString("D3DStuff - Direct3DCreate9: 0x%08x", pD3D9));

    bool bDetectedOptimus = false;
    bool bDetectedNVidia = false;
    // Get info about each connected adapter
    uint uiNumAdapters = pD3D9->GetAdapterCount();
    WriteDebugEvent(SString("D3DStuff - %d Adapters", uiNumAdapters));

    for (uint i = 0; i < uiNumAdapters; i++)
    {
        D3DADAPTER_IDENTIFIER9 Identifier;
        D3DDISPLAYMODE         DisplayMode;
        D3DCAPS9               Caps9;

        HRESULT  hr1 = pD3D9->GetAdapterIdentifier(i, 0, &Identifier);
        HRESULT  hr2 = pD3D9->GetAdapterDisplayMode(i, &DisplayMode);
        HRESULT  hr3 = pD3D9->GetDeviceCaps(i, D3DDEVTYPE_HAL, &Caps9);
        UINT     ModeCount = pD3D9->GetAdapterModeCount(i, D3DFMT_X8R8G8B8);
        HMONITOR hMonitor = pD3D9->GetAdapterMonitor(i);

        if (FAILED(hr1) || FAILED(hr2) || FAILED(hr3))
        {
            WriteDebugEvent(SString("D3DStuff %d Failed GetAdapterIdentifier(%x) GetAdapterDisplayMode(%x) GetDeviceCaps(%x) ", i, hr1, hr2, hr3));
            continue;
        }

        // Detect Optimus combo
        if (SStringX(Identifier.Driver).BeginsWithI("nv") && SStringX(Identifier.Description).BeginsWithI("Intel"))
        {
            bDetectedOptimus = true;
            WriteDebugEvent(SString("D3DStuff %d - Detected Optimus Combo", i));
        }
        if (GetModuleHandle("nvd3d9wrap.dll") != NULL)
        {
            bDetectedOptimus = true;
            WriteDebugEvent(SString("D3DStuff %d - Detected nvd3d9wrap", i));
        }
        if (SStringX(Identifier.Driver).BeginsWithI("nv"))
        {
            bDetectedNVidia = true;
        }

        WriteDebugEvent(SString("D3DStuff %d Identifier - %s", i, *ToString(Identifier)));
        WriteDebugEvent(SString("D3DStuff %d DisplayMode - %s", i, *ToString(DisplayMode)));
        WriteDebugEvent(SString("D3DStuff %d  hMonitor:0x%08x  ModeCount:%d", i, hMonitor, ModeCount));
        WriteDebugEvent(SString("D3DStuff %d Caps9 - %s ", i, *ToString(Caps9)));
    }

    if (GetApplicationSettingInt("nvhacks", "optimus-force-detection"))
        bDetectedOptimus = true;

    if (NvOptimusDetect())
        bDetectedOptimus = true;

    SetApplicationSettingInt("nvhacks", "optimus", bDetectedOptimus);
    SetApplicationSettingInt("nvhacks", "nvidia", bDetectedNVidia);

    if (int revision = GetApplicationSettingInt("nvhacks", "reset-revision"); revision < 21590)
    {
        SetApplicationSettingInt("nvhacks", "reset-revision", 21590);
        SetApplicationSettingInt("nvhacks", "optimus-startup-option", 0);
        SetApplicationSettingInt("nvhacks", "optimus-alt-startup", 0);
        SetApplicationSettingInt("nvhacks", "optimus-export-enablement", 0);
        SetApplicationSettingInt("nvhacks", "optimus-force-windowed", 0);
        SetApplicationSettingInt("nvhacks", "optimus-remember-option", 0);
    }

    if (bDetectedOptimus)
    {
        ShowOptimusDialog(g_hInstance);
        HideOptimusDialog();
    }
    else
    {
        SetApplicationSettingInt("nvhacks", "optimus-alt-startup", 0);
        SetApplicationSettingInt("nvhacks", "optimus-export-enablement", 0);
        SetApplicationSettingInt("nvhacks", "optimus-force-windowed", 0);
    }

    // Crash handler enablement now occurs inside the game process once the
    // Direct3D device is fully created (see CGraphics::OnDeviceCreate).
}

//////////////////////////////////////////////////////////
//
// EndD3DStuff
//
// Clean up
//
//////////////////////////////////////////////////////////
void EndD3DStuff()
{
    for (uint i = 0; i < NUMELMS(pD3DVertexDeclarations); i++)
        SAFE_RELEASE(pD3DVertexDeclarations[i]);
    SAFE_RELEASE(pD3DDevice9);
    SAFE_RELEASE(pD3D9);
}

//////////////////////////////////////////////////////////
//
// DXVK (D3D9 -> Vulkan) support
//
//////////////////////////////////////////////////////////

static const char* const DXVK_LIBRARY_NAMES[] = {"d3d9.dll", "dxgi.dll"};

static SString GetDXVKStageDir()
{
    return PathJoin(GetMTASAPath(), "mta", "dxvk");
}

static SString GetDXVKStageFile(const char* szFileName)
{
    return PathJoin(GetDXVKStageDir(), szFileName);
}

static SString GetDXVKGtaDir()
{
    return UTF8FilePath(GetGameLaunchDirectory());
}

bool IsDXVKBinaryFile(const SString& strFilePath)
{
    if (!FileExists(strFilePath))
        return false;

    const SString strFileMd5 = CMD5Hasher::CalculateHexString(strFilePath);
    for (const char* szName : DXVK_LIBRARY_NAMES)
    {
        const SString strStagePath = GetDXVKStageFile(szName);
        if (FileExists(strStagePath) && CMD5Hasher::CalculateHexString(strStagePath).CompareI(strFileMd5))
            return true;
    }
    return false;
}

static void RemovePlacedDXVKFiles()
{
    const SString strGtaLaunchDir = GetDXVKGtaDir();
    for (const char* szName : DXVK_LIBRARY_NAMES)
    {
        const SString strPath = PathJoin(strGtaLaunchDir, szName);
        if (FileExists(strPath))
        {
            FileDelete(strPath);
            WriteDebugEvent(SString("DXVK: removed %s from GTA launch directory", *strPath));
        }
    }
}

static void CopyDXVKFileIfNeeded(const SString& strSrc, const SString& strDst)
{
    if (!FileExists(strSrc))
        return;
    if (FileExists(strDst) && CMD5Hasher::CalculateHexString(strDst).CompareI(CMD5Hasher::CalculateHexString(strSrc)))
        return;
    CopyFileW(FromUTF8(strSrc).c_str(), FromUTF8(strDst).c_str(), FALSE);
}

static bool ProbeD3D9Create(HMODULE hD3d9)
{
    using FnDirect3DCreate9 = IDirect3D9*(__stdcall*)(UINT);
    FnDirect3DCreate9 pfnDirect3DCreate9 = nullptr;
    if (!SharedUtil::TryGetProcAddress(hD3d9, "Direct3DCreate9", pfnDirect3DCreate9) || !pfnDirect3DCreate9)
        return false;

    __try
    {
        if (IDirect3D9* pD3D9 = pfnDirect3DCreate9(D3D_SDK_VERSION))
        {
            pD3D9->Release();
            return true;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
    return false;
}

static constexpr int DXVK_NEGATIVE_PROBE_RECHECK_LAUNCHES = 3;

static SString GetDXVKStageFilesId()
{
    SString strId;
    for (const char* szName : DXVK_LIBRARY_NAMES)
    {
        const SString strPath = GetDXVKStageFile(szName);

        WIN32_FILE_ATTRIBUTE_DATA fileInfo = {};
        if (!GetFileAttributesExW(FromUTF8(strPath).c_str(), GetFileExInfoStandard, &fileInfo))
            return "";

        strId += SString("%u_%u_%u_%u;", fileInfo.nFileSizeLow, fileInfo.nFileSizeHigh, fileInfo.ftLastWriteTime.dwLowDateTime,
                         fileInfo.ftLastWriteTime.dwHighDateTime);
    }
    return strId;
}

static bool ProbeVulkanSupport(const SString& strStageDir)
{
    const SString strStageD3d9 = PathJoin(strStageDir, "d3d9.dll");
    if (!FileExists(strStageD3d9))
        return false;

    const SString strFilesId = GetDXVKStageFilesId();
    if (strFilesId.empty())
        return false;

    if (GetApplicationSetting("dxvk", "probe_files_id") == strFilesId)
    {
        if (GetApplicationSettingInt("dxvk", "vulkan_supported") == 1)
            return true;

        const int iLaunchesSinceProbe = GetApplicationSettingInt("dxvk", "probe_launches_since_unsupported");
        if (iLaunchesSinceProbe < DXVK_NEGATIVE_PROBE_RECHECK_LAUNCHES)
        {
            SetApplicationSettingInt("dxvk", "probe_launches_since_unsupported", iLaunchesSinceProbe + 1);
            return false;
        }
    }

    const SString strProbeD3d9 = PathJoin(strStageDir, SString("d3d9_probe_%d.dll", GetCurrentProcessId()));
    if (!CopyFileW(FromUTF8(strStageD3d9).c_str(), FromUTF8(strProbeD3d9).c_str(), FALSE))
        return false;

    bool bSupported = false;
    if (HMODULE hD3d9 = LoadLibraryExW(FromUTF8(strProbeD3d9).c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH))
    {
        bSupported = ProbeD3D9Create(hD3d9);
        FreeLibrary(hD3d9);
    }

    FileDelete(strProbeD3d9);

    SetApplicationSetting("dxvk", "probe_files_id", strFilesId);
    SetApplicationSettingInt("dxvk", "probe_launches_since_unsupported", bSupported ? 0 : 1);

    return bSupported;
}

static SString GetCoreConfigFilename()
{
    return CalcMTASAPath(PathJoin("mta", "config", "coreconfig.xml"));
}

bool GetDXVKEnabledSetting()
{
    SString strCoreConfig;
    FileLoad(GetCoreConfigFilename(), strCoreConfig);
    return strCoreConfig.SplitRight("<dxvk_enabled>").Left(1) == "1";
}

void SetDXVKEnabledSetting(bool bEnable)
{
    const char* szFrom = bEnable ? "<dxvk_enabled>0" : "<dxvk_enabled>1";
    const char* szTo = bEnable ? "<dxvk_enabled>1" : "<dxvk_enabled>0";

    SString strCoreConfig;
    if (!FileLoad(GetCoreConfigFilename(), strCoreConfig) || !strCoreConfig.Contains(szFrom))
        return;

    strCoreConfig = strCoreConfig.Replace(szFrom, szTo);
    FileSave(GetCoreConfigFilename(), strCoreConfig);
    WriteDebugEvent(SString("DXVK: set dxvk_enabled=%d in coreconfig.xml", bEnable ? 1 : 0));
}

static bool IsValidDXVKBinary(const SString& strPath)
{
    const uint64 uiFileSize = FileSize(strPath);
    if (uiFileSize < 1000000)
        return false;

    SString strDosHeader;
    if (!FileLoad(strPath, strDosHeader, 0x40) || strDosHeader.length() < 0x40)
        return false;

    const unsigned char* pDosHeader = reinterpret_cast<const unsigned char*>(strDosHeader.c_str());
    if (pDosHeader[0] != 'M' || pDosHeader[1] != 'Z')
        return false;

    const unsigned int uiPe = pDosHeader[0x3C] | (pDosHeader[0x3D] << 8) | (pDosHeader[0x3E] << 16) | (static_cast<unsigned int>(pDosHeader[0x3F]) << 24);
    if (uiPe == 0 || uiPe >= uiFileSize)
        return false;

    SString strPeHeader;
    if (!FileLoad(strPath, strPeHeader, 0x8, static_cast<int>(uiPe)) || strPeHeader.length() < 6)
        return false;

    const unsigned char* pPeHeader = reinterpret_cast<const unsigned char*>(strPeHeader.c_str());
    if (pPeHeader[0] != 'P' || pPeHeader[1] != 'E' || pPeHeader[2] != 0 || pPeHeader[3] != 0)
        return false;

    const unsigned short usMachine = static_cast<unsigned short>(pPeHeader[4] | (pPeHeader[5] << 8));
    if (usMachine != 0x014C)
        return false;

    return true;
}

static SString VerifyDXVKStageFiles()
{
    for (const char* szName : DXVK_LIBRARY_NAMES)
    {
        const SString strPath = GetDXVKStageFile(szName);
        if (!FileExists(strPath))
            return SString("%s is missing from mta\\dxvk", szName);
        if (!IsValidDXVKBinary(strPath))
            return SString("%s is missing, corrupted or not a valid 32-bit binary", szName);
    }
    return "";
}

bool ManageDXVK()
{
    const bool    bEnable = GetDXVKEnabledSetting();
    const SString strGtaLaunchDir = GetDXVKGtaDir();

    const SString strVerifyProblem = VerifyDXVKStageFiles();
    const bool    bFilesOk = strVerifyProblem.empty();

    const bool bVulkanSupported = bFilesOk && ProbeVulkanSupport(GetDXVKStageDir());
    SetApplicationSetting("dxvk", "vulkan_supported", bVulkanSupported ? "1" : "0");
    SetApplicationSetting("dxvk", "files_ok", bFilesOk ? "1" : "0");

    if (!bEnable)
    {
        RemovePlacedDXVKFiles();
        return true;
    }

    if (!bFilesOk)
    {
        RemovePlacedDXVKFiles();
        WriteDebugEvent(SString("DXVK: %s - Vulkan files not usable", *strVerifyProblem));
        AddReportLog(7210, SString("DXVK: %s - Vulkan files not usable", *strVerifyProblem));

        SetDXVKEnabledSetting(false);
        AddReportLog(7215, "DXVK: Vulkan auto-disabled (files missing or damaged)");
        return true;
    }

    if (!bVulkanSupported)
    {
        RemovePlacedDXVKFiles();
        WriteDebugEvent("DXVK: Vulkan not supported on this system");
        AddReportLog(7212, "DXVK: Vulkan not supported");

        SetDXVKEnabledSetting(false);
        AddReportLog(7213, "DXVK: Vulkan auto-disabled (not supported)");
        return true;
    }

    for (const char* szName : DXVK_LIBRARY_NAMES)
    {
        const SString strGtaPath = PathJoin(strGtaLaunchDir, szName);
        if (FileExists(strGtaPath) && !IsDXVKBinaryFile(strGtaPath))
        {
            FileDelete(strGtaPath);
            WriteDebugEvent(SString("DXVK: removed foreign %s from GTA launch directory", szName));
            AddReportLog(7211, SString("DXVK: removed foreign %s from GTA launch directory", szName));
        }
    }

    MakeSureDirExists(strGtaLaunchDir);
    CopyDXVKFileIfNeeded(GetDXVKStageFile("d3d9.dll"), PathJoin(strGtaLaunchDir, "d3d9.dll"));
    CopyDXVKFileIfNeeded(GetDXVKStageFile("dxgi.dll"), PathJoin(strGtaLaunchDir, "dxgi.dll"));

    for (const char* szName : DXVK_LIBRARY_NAMES)
    {
        const SString strGtaPath = PathJoin(strGtaLaunchDir, szName);
        if (FileExists(strGtaPath))
        {
            SetApplicationSetting("diagnostics", SString("%s-dll-last-hash", szName), CMD5Hasher::CalculateHexString(strGtaPath));
            SetApplicationSetting("diagnostics", SString("%s-dll-not-again", szName), "yes");
        }
    }

    WriteDebugEvent("DXVK: Vulkan supported - D3D9 will be translated to Vulkan");
    return true;
}
