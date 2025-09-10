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
}            // namespace

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
    WriteDebugEventAndReport(ReportLogID::NVIDIA_OPTIMUS_DETECTED, SString("NvAPI - InterfaceVersion:'%s' DriverVersion:%d.%d Branch:'%s'", szDesc, uiDriverVersion / 100,
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
        NV_SYSTEM_TYPE    SystemType = (NV_SYSTEM_TYPE)-1;            // 1-Laptop 2-Desktop
        NV_GPU_TYPE       GpuType = (NV_GPU_TYPE)-1;                  // 1-Integrated 2-Discrete
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
        WriteDebugEventAndReport(ReportLogID::NVIDIA_OPTIMUS_GPU_RESULT, strStatus);
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
