/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CProxyDirect3D9.cpp
 *  PURPOSE:     Direct3D 9 function hooking proxy
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <dwmapi.h>
#include <resource.h>

extern HINSTANCE g_hModule;

namespace
{
template <typename T>
bool IsValidComInterfacePointer(T* pointer)
{
    if (!pointer)
        return true;

    if (!SharedUtil::IsReadablePointer(pointer, sizeof(void*)))
        return false;

    void* const* vtablePtr = reinterpret_cast<void* const*>(pointer);
    if (!vtablePtr)
        return false;

    void* const vtable = *vtablePtr;
    if (!vtable)
        return false;

    constexpr size_t requiredBytes = sizeof(void*) * 3;
    return SharedUtil::IsReadablePointer(vtable, requiredBytes);
}

template <typename T>
void ReleaseInterface(T*& pointer, const char* context = nullptr)
{
    if (!pointer)
        return;

    if (IsValidComInterfacePointer(pointer))
    {
        pointer->Release();
    }
    else
    {
    SString label;
    label = context ? context : "ReleaseInterface";
        SString message;
        message.Format("%s: skipping Release on invalid COM pointer %p", label.c_str(), pointer);
        AddReportLog(8752, message, 5);
    }

    pointer = nullptr;
}

template <typename T>
void ReplaceInterface(T*& destination, T* source, const char* context = nullptr)
{
    if (destination == source)
        return;

    if (source && !IsValidComInterfacePointer(source))
    {
    SString label;
    label = context ? context : "ReplaceInterface";
        SString message;
        message.Format("%s: rejected invalid COM pointer %p", label.c_str(), source);
        AddReportLog(8753, message, 5);
        return;
    }

    ReleaseInterface(destination, context);
    destination = source;
    if (destination)
        destination->AddRef();
}

IDirect3D9* GetFirstValidTrackedDirect3D(std::vector<IDirect3D9*>& trackedList)
{
    for (auto iter = trackedList.begin(); iter != trackedList.end();)
    {
        IDirect3D9* candidate = *iter;
        if (candidate && IsValidComInterfacePointer(candidate))
            return candidate;

        SString message;
        message.Format("CProxyDirect3D9: removing invalid tracked IDirect3D9 pointer %p", candidate);
        AddReportLog(8756, message, 5);
        iter = trackedList.erase(iter);
    }

    return nullptr;
}
}        // namespace

HRESULT HandleCreateDeviceResult(HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                 D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
std::vector<IDirect3D9*> ms_CreatedDirect3D9List;
bool CreateDeviceSecondCallCheck(HRESULT& hOutResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                 D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);

CProxyDirect3D9::CProxyDirect3D9(IDirect3D9* pInterface)
    : m_pDevice(nullptr)
    , m_lRefCount(1)
{
    WriteDebugEvent(SString("CProxyDirect3D9::CProxyDirect3D9 %08x", this));

    if (!IsValidComInterfacePointer(pInterface))
    {
        SString message;
        message.Format("CProxyDirect3D9 ctor: received invalid IDirect3D9 pointer %p", pInterface);
        AddReportLog(8753, message, 5);
        m_pDevice = pInterface;
    }
    else
    {
        ReplaceInterface(m_pDevice, pInterface, "CProxyDirect3D9 ctor");
        pInterface->Release();
    }

    if (m_pDevice)
    {
        if (IsValidComInterfacePointer(m_pDevice))
        {
            ms_CreatedDirect3D9List.push_back(m_pDevice);
        }
        else
        {
            SString message;
            message.Format("CProxyDirect3D9 ctor: not tracking invalid IDirect3D9 pointer %p", m_pDevice);
            AddReportLog(8756, message, 5);
        }
    }
}

CProxyDirect3D9::~CProxyDirect3D9()
{
    WriteDebugEvent(SString("CProxyDirect3D9::~CProxyDirect3D9 %08x", this));
    ListRemove(ms_CreatedDirect3D9List, m_pDevice);
    ReleaseInterface(m_pDevice, "CProxyDirect3D9 dtor");
}

/*** IUnknown methods ***/
HRESULT CProxyDirect3D9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (!m_pDevice || !IsValidComInterfacePointer(m_pDevice))
    {
    SString message;
    message.Format("CProxyDirect3D9::QueryInterface rejected invalid IDirect3D9 pointer %p", m_pDevice);
    AddReportLog(8752, message, 5);
        if (ppvObj)
            *ppvObj = nullptr;
        return E_POINTER;
    }

    return m_pDevice->QueryInterface(riid, ppvObj);
}

ULONG CProxyDirect3D9::AddRef()
{
    LONG lNewRefCount = InterlockedIncrement(&m_lRefCount);

    if (m_pDevice)
    {
        if (IsValidComInterfacePointer(m_pDevice))
        {
            m_pDevice->AddRef();
        }
        else
        {
            SString message;
            message.Format("CProxyDirect3D9::AddRef skipped underlying AddRef; invalid pointer %p", m_pDevice);
            AddReportLog(8752, message, 5);
        }
    }

    return static_cast<ULONG>(lNewRefCount);
}

ULONG CProxyDirect3D9::Release()
{
    LONG lNewRefCount = InterlockedDecrement(&m_lRefCount);

    if (lNewRefCount < 0)
    {
    SString message;
    message.Format("CProxyDirect3D9::Release detected reference count underflow for proxy %p", this);
    AddReportLog(8752, message, 5);
        lNewRefCount = 0;
    }

    if (m_pDevice && lNewRefCount > 0)
    {
        if (IsValidComInterfacePointer(m_pDevice))
        {
            m_pDevice->Release();
        }
        else
        {
            SString message;
            message.Format("CProxyDirect3D9::Release skipped underlying Release; invalid pointer %p", m_pDevice);
            AddReportLog(8752, message, 5);
        }
    }

    if (lNewRefCount == 0)
        delete this;

    return static_cast<ULONG>(lNewRefCount);
}

/*** IDirect3D9 methods ***/
HRESULT CProxyDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction)
{
    return m_pDevice->RegisterSoftwareDevice(pInitializeFunction);
}

UINT CProxyDirect3D9::GetAdapterCount()
{
    return m_pDevice->GetAdapterCount();
}

HRESULT CProxyDirect3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
    return m_pDevice->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT CProxyDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
    return m_pDevice->GetAdapterModeCount(Adapter, Format);
}

HRESULT CProxyDirect3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
    return m_pDevice->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT CProxyDirect3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
    return m_pDevice->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT CProxyDirect3D9::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
    return m_pDevice->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT CProxyDirect3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType,
                                           D3DFORMAT CheckFormat)
{
    return m_pDevice->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT CProxyDirect3D9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed,
                                                    D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
    return m_pDevice->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT CProxyDirect3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat,
                                                D3DFORMAT DepthStencilFormat)
{
    return m_pDevice->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT CProxyDirect3D9::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
    return m_pDevice->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT CProxyDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
    return m_pDevice->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR CProxyDirect3D9::GetAdapterMonitor(UINT Adapter)
{
    return m_pDevice->GetAdapterMonitor(Adapter);
}

HMONITOR CProxyDirect3D9::StaticGetAdapterMonitor(UINT Adapter)
{
    IDirect3D9* pDirect3D = GetFirstValidTrackedDirect3D(ms_CreatedDirect3D9List);
    if (!pDirect3D)
        return NULL;

    return pDirect3D->GetAdapterMonitor(Adapter);
}

IDirect3D9* CProxyDirect3D9::StaticGetDirect3D()
{
    return GetFirstValidTrackedDirect3D(ms_CreatedDirect3D9List);
}

HRESULT CProxyDirect3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                      D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    // Do not change the code at the start of this function.
    // Some graphic card drivers seem sensitive to the content of the CreateDevice function.
    HRESULT hResult;

    WriteDebugEvent("CProxyDirect3D9::CreateDevice");

    if (!ppReturnedDeviceInterface)
    {
        AddReportLog(8754, SStringX("CProxyDirect3D9::CreateDevice - missing ppReturnedDeviceInterface pointer"), 5);
        return D3DERR_INVALIDCALL;
    }

    if (!SharedUtil::IsReadablePointer(ppReturnedDeviceInterface, sizeof(*ppReturnedDeviceInterface)))
    {
        SString message;
        message.Format("CProxyDirect3D9::CreateDevice - invalid ppReturnedDeviceInterface pointer %p", ppReturnedDeviceInterface);
        AddReportLog(8754, message, 5);
        return D3DERR_INVALIDCALL;
    }

    *ppReturnedDeviceInterface = nullptr;

    WriteDebugEvent(SString("    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x", Adapter, DeviceType, BehaviorFlags));

    // Make sure DirectX Get calls will work
    BehaviorFlags &= ~D3DCREATE_PUREDEVICE;

    WriteDebugEvent(SString("    BackBufferWidth:%d  Height:%d  Format:%d  Count:%d", pPresentationParameters->BackBufferWidth,
                            pPresentationParameters->BackBufferHeight, pPresentationParameters->BackBufferFormat, pPresentationParameters->BackBufferCount));

    WriteDebugEvent(SString("    MultiSampleType:%d  Quality:%d", pPresentationParameters->MultiSampleType, pPresentationParameters->MultiSampleQuality));

    WriteDebugEvent(SString("    SwapEffect:%d  Windowed:%d  EnableAutoDepthStencil:%d  AutoDepthStencilFormat:%d  Flags:0x%x",
                            pPresentationParameters->SwapEffect, pPresentationParameters->Windowed, pPresentationParameters->EnableAutoDepthStencil,
                            pPresentationParameters->AutoDepthStencilFormat, pPresentationParameters->Flags));

    WriteDebugEvent(SString("    FullScreen_RefreshRateInHz:%d  PresentationInterval:0x%08x", pPresentationParameters->FullScreen_RefreshRateInHz,
                            pPresentationParameters->PresentationInterval));

    // Change the window title to MTA: San Andreas
    #ifdef MTA_DEBUG
    SetWindowTextW(hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas [DEBUG]").c_str());
    #else
    SetWindowTextW(hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas").c_str());
    #endif

    // Set dark titlebar if needed
    BOOL darkTitleBar = GetSystemRegistryValue((uint)HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", "AppsUseLightTheme") == "\x0";
    DwmSetWindowAttribute(hFocusWindow, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkTitleBar, sizeof(darkTitleBar));

    // Update icon
    if (HICON icon = LoadIcon(g_hModule, MAKEINTRESOURCE(IDI_ICON1)))
    {
        const auto paramIcon = reinterpret_cast<LPARAM>(icon);
        for (const WPARAM size : {ICON_SMALL, ICON_BIG})
        {
            SendMessage(hFocusWindow, WM_SETICON, size, paramIcon);
        }

        // Clean
        DestroyIcon(icon);
    }

    // Redraw, we avoid possible problems with the fact that it won't replace the icon somewhere
    InvalidateRect(hFocusWindow, nullptr, TRUE);
    UpdateWindow(hFocusWindow);

    // Detect if second call to CreateDevice
    if (CreateDeviceSecondCallCheck(hResult, m_pDevice, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface))
    {
        return hResult;
    }

    // Enable the auto depth stencil parameter
    pPresentationParameters->EnableAutoDepthStencil = true;

    GetVideoModeManager()->PreCreateDevice(pPresentationParameters);

    // Create our object.
    hResult = m_pDevice->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

    // Check if the result is correct (a custom d3d9.dll could return D3D_OK with a null pointer)
    if (hResult == D3D_OK)
    {
        IDirect3DDevice9* pCreatedDevice = *ppReturnedDeviceInterface;
        if (!pCreatedDevice)
        {
            AddReportLog(8755, SStringX("CProxyDirect3D9::CreateDevice - driver returned nullptr device"), 5);
            hResult = D3DERR_INVALIDDEVICE;
        }
        else if (!IsValidComInterfacePointer(pCreatedDevice))
        {
            SString message;
            message.Format("CProxyDirect3D9::CreateDevice - rejected invalid IDirect3DDevice9 pointer %p", pCreatedDevice);
            AddReportLog(8755, message, 5);
            ReleaseInterface(pCreatedDevice, "CProxyDirect3D9::CreateDevice invalid return");
            *ppReturnedDeviceInterface = nullptr;
            hResult = D3DERR_INVALIDDEVICE;
        }
    }

    // Store the rendering window in the direct 3d data
    CDirect3DData::GetSingleton().StoreDeviceWindow(pPresentationParameters->hDeviceWindow);

    // Make sure the object was created successfully.
    if (hResult == D3D_OK)
    {
        // Apply input hook
        CMessageLoopHook::GetSingleton().ApplyHook(hFocusWindow);

        GetVideoModeManager()->PostCreateDevice(*ppReturnedDeviceInterface, pPresentationParameters);

        // We must first store the presentation values.
        CDirect3DData::GetSingleton().StoreViewport(0, 0, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

        // Calc and store readable depth format for shader use
        ERenderFormat ReadableDepthFormat = CDirect3DEvents9::DiscoverReadableDepthFormat(*ppReturnedDeviceInterface, pPresentationParameters->MultiSampleType,
                                                                                          pPresentationParameters->Windowed != 0);
        CGraphics::GetSingleton().GetRenderItemManager()->SetDepthBufferFormat(ReadableDepthFormat);

        // Now create the proxy device.
        IDirect3DDevice9* pOriginalDevice = *ppReturnedDeviceInterface;
        *ppReturnedDeviceInterface = new CProxyDirect3DDevice9(pOriginalDevice);
        if (pOriginalDevice)
        {
            if (IsValidComInterfacePointer(pOriginalDevice))
            {
                pOriginalDevice->Release();
            }
            else
            {
                SString message;
                message.Format("CProxyDirect3D9::CreateDevice - skipping Release on invalid original device pointer %p", pOriginalDevice);
                AddReportLog(8755, message, 5);
            }
        }

        // Debug output
        D3DDEVICE_CREATION_PARAMETERS parameters;
        (*ppReturnedDeviceInterface)->GetCreationParameters(&parameters);

        WriteDebugEvent(SString("    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x  ReadableDepth:%s", parameters.AdapterOrdinal, parameters.DeviceType,
                                parameters.BehaviorFlags, ReadableDepthFormat ? std::string((char*)&ReadableDepthFormat, 4).c_str() : "None"));
    }

    hResult =
        HandleCreateDeviceResult(hResult, m_pDevice, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

    // After successful device creation
    if (SUCCEEDED(hResult) && *ppReturnedDeviceInterface)
    {
        // Check if we're in borderless mode
        bool bIsBorderlessMode = pPresentationParameters->Windowed == TRUE;

        if (bIsBorderlessMode)
        {
            // Enable sRGB correction for borderless mode to compensate for DWM composition
            CProxyDirect3DDevice9* pProxyDevice = static_cast<CProxyDirect3DDevice9*>(*ppReturnedDeviceInterface);

            // Set initial render states for proper color handling
            pProxyDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);

            // Configure samplers for sRGB texture reading
            for (DWORD i = 0; i < 8; i++)
            {
                pProxyDevice->SetSamplerState(i, D3DSAMP_SRGBTEXTURE, TRUE);
            }

            WriteDebugEvent("Applied sRGB color correction for borderless mode");
        }
    }

    return hResult;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::LockRect
//
// For stats
//
/////////////////////////////////////////////////////////////
SString GUIDToString(const GUID& g);

#define CREATE_DEVICE_FAIL          1
#define CREATE_DEVICE_RETRY_SUCCESS 2
#define CREATE_DEVICE_SUCCESS       3

namespace
{
    // Debugging helpers
    SString ToString(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, const D3DPRESENT_PARAMETERS& pp)
    {
        return SString(
            "    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x\n"
            "    BackBufferWidth:%d  Height:%d  Format:%d  Count:%d\n"
            "    MultiSampleType:%d  Quality:%d\n"
            "    SwapEffect:%d  Windowed:%d  EnableAutoDepthStencil:%d  AutoDepthStencilFormat:%d  Flags:0x%x\n"
            "    FullScreen_RefreshRateInHz:%d  PresentationInterval:0x%08x",
            Adapter, DeviceType, BehaviorFlags, pp.BackBufferWidth, pp.BackBufferHeight, pp.BackBufferFormat, pp.BackBufferCount, pp.MultiSampleType,
            pp.MultiSampleQuality, pp.SwapEffect, pp.Windowed, pp.EnableAutoDepthStencil, pp.AutoDepthStencilFormat, pp.Flags, pp.FullScreen_RefreshRateInHz,
            pp.PresentationInterval);
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

    SString ToString(const D3DCAPS9& a)
    {
        return SString(
            " VSVer:0x%08x"
            " PSVer:0x%08x"
            " DeclTypes:0x%03x"
            " VerProcCaps:0x%03x"
            " MaxLights:0x%01x"
            " MaxClips:0x%01x"
            " TexOpCaps:0x%08x"
            " S30MaxSlots:%d/%d"
            " TexMaxSize:%d/%d/%d",
            a.VertexShaderVersion, a.PixelShaderVersion, a.DeclTypes, a.VertexProcessingCaps, a.MaxActiveLights, a.MaxUserClipPlanes, a.TextureOpCaps,
            a.MaxVertexShader30InstructionSlots, a.MaxPixelShader30InstructionSlots, a.MaxTextureWidth, a.MaxTextureHeight, a.MaxVolumeExtent);
    }

    //
    // Hacky log interception
    //
    SString ms_strExtraLogBuffer;
    void    WriteDebugEventTest(const SString& strText)
    {
        ms_strExtraLogBuffer += strText.Replace("\n", " ") + "\n";
        WriteDebugEvent(strText);
    }
    #define WriteDebugEvent WriteDebugEventTest

    uint ms_uiCreationAttempts = 0;
}            // namespace

////////////////////////////////////////////////
//
// CreateDeviceInsist
//
// Keep trying create device for a little bit
//
////////////////////////////////////////////////
HRESULT CreateDeviceInsist(uint uiMinTries, uint uiTimeout, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                           D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    HRESULT      hResult;
    CElapsedTime retryTimer;
    uint         uiRetryCount = 0;
    do
    {
        ms_uiCreationAttempts++;
        hResult = pDirect3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

        // Check if the result is correct (a custom d3d9.dll could return D3D_OK with a null pointer)
        if (hResult == D3D_OK)
        {
            IDirect3DDevice9* pCreatedDevice = *ppReturnedDeviceInterface;
            if (!pCreatedDevice)
            {
                AddReportLog(8755, SStringX("CreateDeviceInsist: driver returned nullptr device"), 5);
                hResult = D3DERR_INVALIDDEVICE;
            }
            else if (!IsValidComInterfacePointer(pCreatedDevice))
            {
                SString message;
                message.Format("CreateDeviceInsist: rejected invalid IDirect3DDevice9 pointer %p", pCreatedDevice);
                AddReportLog(8755, message, 5);
                ReleaseInterface(pCreatedDevice, "CreateDeviceInsist invalid return");
                *ppReturnedDeviceInterface = nullptr;
                hResult = D3DERR_INVALIDDEVICE;
            }
        }

        if (hResult == D3D_OK)
        {
            WriteDebugEvent(SString("   -- CreateDeviceInsist succeeded on try #%d", uiRetryCount + 1));
            break;
        }
        Sleep(1);
    } while (++uiRetryCount < uiMinTries || retryTimer.Get() < uiTimeout);

    return hResult;
}

////////////////////////////////////////////////
//
// DoCreateDevice
//
// In various ways
//
////////////////////////////////////////////////
HRESULT DoCreateDevice(IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                       D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    // If initial attempt failed, enable diagnostic log and try again a few more times over the next second
    WriteDebugEvent("  Pass #2 as before");
    WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));
    HRESULT hResult =
        CreateDeviceInsist(2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
    if (hResult == D3D_OK)
        return hResult;
    WriteDebugEvent(SString("  CreateDevice failed #2: %08x", hResult));
    HRESULT hResultFail = hResult;

    // If create failed, try removing multisampling if enabled
    if (pPresentationParameters->MultiSampleType)
    {
        pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_NONE;
        WriteDebugEvent("    Pass #3 with D3DMULTISAMPLE_NONE:");
        WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));
        hResult = CreateDeviceInsist(2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
        if (hResult == D3D_OK)
            return hResult;
        WriteDebugEvent(SString("    CreateDevice failed #3: %08x", hResult));
    }

    // Run through different combinations
    uint          presentIntervalList[] = {D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_DEFAULT, D3DPRESENT_INTERVAL_ONE};
    D3DSWAPEFFECT swapEffectList[] = {D3DSWAPEFFECT_DISCARD, D3DSWAPEFFECT_FLIP, D3DSWAPEFFECT_COPY};
    D3DFORMAT     rtFormatList32[] = {D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8};
    D3DFORMAT     depthFormatList32[] = {D3DFMT_D24S8, D3DFMT_D24X8};
    D3DFORMAT     rtFormatList16[] = {D3DFMT_R5G6B5};
    D3DFORMAT     depthFormatList16[] = {D3DFMT_D16};
    struct SFormat
    {
        D3DFORMAT* rtFormatList;
        uint       rtFormatListSize;
        D3DFORMAT* depthFormatList;
        uint       depthFormatListSize;
    } formatList[] = {{
                          rtFormatList32,
                          NUMELMS(rtFormatList32),
                          depthFormatList32,
                          NUMELMS(depthFormatList32),
                      },
                      {
                          rtFormatList16,
                          NUMELMS(rtFormatList16),
                          depthFormatList16,
                          NUMELMS(depthFormatList16),
                      }};

    D3DPRESENT_PARAMETERS savedPresentationParameters = *pPresentationParameters;
    for (uint iRes = 0; iRes < 6; iRes++)
    {
        // iRes:
        //      0 - Full screen or windowed mode, as per options
        //      1 - Force windowed mode
        //      2 - Force full screen
        //      3 - Force 640x480 full screen or windowed mode, as per options (for test only)
        //      4 - Force 640x480 windowed mode (for test only)
        //      5 - Force 640x480 full screen (for test only)

        // Reset settings
        *pPresentationParameters = savedPresentationParameters;

        if (iRes == 1 || iRes == 4)
        {
            // Force windowed mode
            if (pPresentationParameters->Windowed)
                continue;
            pPresentationParameters->Windowed = true;
            pPresentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        }

        if (iRes == 2 || iRes == 5)
        {
            // Force full screen
            if (!pPresentationParameters->Windowed)
                continue;
            pPresentationParameters->Windowed = false;
            pPresentationParameters->FullScreen_RefreshRateInHz = 60;
        }

        if (iRes == 3 || iRes == 4 || iRes == 5)
        {
            // 640x480 test
            pPresentationParameters->BackBufferWidth = 640;
            pPresentationParameters->BackBufferHeight = 480;
        }

        for (uint iColor = 0; iColor < 2; iColor++)
        {
            for (uint iBehavior = 0; iBehavior < 2; iBehavior++)
            {
                if (iBehavior == 0)
                    BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
                else
                    BehaviorFlags |= D3DCREATE_PUREDEVICE;

                for (uint iRefresh = 0; iRefresh < 2; iRefresh++)
                {
                    if (iRefresh == 1)
                    {
                        if (pPresentationParameters->Windowed)
                            continue;
                        pPresentationParameters->FullScreen_RefreshRateInHz = 60;
                    }

                    for (uint iPresent = 0; iPresent < NUMELMS(presentIntervalList); iPresent++)
                    {
                        for (uint iSwap = 0; iSwap < NUMELMS(swapEffectList); iSwap++)
                        {
                            SFormat format = formatList[iColor];

                            for (uint iRt = 0; iRt < format.rtFormatListSize; iRt++)
                            {
                                for (uint iDepth = 0; iDepth < format.depthFormatListSize; iDepth++)
                                {
                                    pPresentationParameters->PresentationInterval = presentIntervalList[iPresent];
                                    pPresentationParameters->SwapEffect = swapEffectList[iSwap];
                                    pPresentationParameters->BackBufferFormat = format.rtFormatList[iRt];
                                    pPresentationParameters->AutoDepthStencilFormat = format.depthFormatList[iDepth];
        #ifndef MTA_DEBUG
                                    hResult = CreateDeviceInsist(2, 0, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters,
                                                                 ppReturnedDeviceInterface);
        #else
                                    WriteDebugEvent("--------------------------------");
                                    WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));
                                    WriteDebugEvent(SString("        32 result: %08x", hResult));
                                    hResult = -1;
        #endif
                                    if (hResult == D3D_OK)
                                    {
                                        WriteDebugEvent(SString("      Pass #4 SUCCESS with: {Res:%d, Color:%d, Refresh:%d}", iRes, iColor, iRefresh));
                                        WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));
                                        if (iRes == 1)
                                        {
                                        }
                                        if (iRes >= 3)
                                        {
                                            // Only test, so return as fail
                                            WriteDebugEvent("      Test success");
                                            goto failed;
                                        }
                                        return hResult;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
failed:
    WriteDebugEvent(SString("      Failed after %d creation attempts", ms_uiCreationAttempts));

    return hResultFail;
}

////////////////////////////////////////////////
//
// GetByteDiffDesc
//
// Get string describing differences between the two memory blocks
//
////////////////////////////////////////////////
SString GetByteDiffDesc(const void* pData1, const void* pData2, uint uiSize)
{
    SString strResult;
    for (uint i = 0; i < uiSize; i++)
    {
        uchar c1 = ((const uchar*)pData1)[i];
        uchar c2 = ((const uchar*)pData2)[i];
        if (c1 != c2)
        {
            strResult += SString("[%d(%02x/%02x)]", i, c1, c2);
        }
    }
    return strResult;
}

////////////////////////////////////////////////
//
// AddCapsReport
//
// Check caps seem correct
//
////////////////////////////////////////////////
void AddCapsReport(UINT Adapter, IDirect3D9* pDirect3D, IDirect3DDevice9* pD3DDevice9, bool bFixGTACaps)
{
    HRESULT hr;

    if (!pDirect3D || !IsValidComInterfacePointer(pDirect3D))
    {
        SString message;
        message.Format("AddCapsReport: invalid IDirect3D9 pointer %p", pDirect3D);
        AddReportLog(8748, message);
        return;
    }

    if (!pD3DDevice9 || !IsValidComInterfacePointer(pD3DDevice9))
    {
        SString message;
        message.Format("AddCapsReport: invalid IDirect3DDevice9 pointer %p", pD3DDevice9);
        AddReportLog(8749, message);
        return;
    }

    WriteDebugEvent(SString("ModuleFileName - %s ", *GetLaunchPathFilename()));

    // Get caps that GTA got
    D3DCAPS9* pGTACaps9 = (D3DCAPS9*)0x00C9BF00;
    WriteDebugEvent(SString("CapsReport GTACaps9 - %s ", *ToString(*pGTACaps9)));

    if (!pD3DDevice9)
        return;

    // Check device returns same D3D interface
    IDirect3D9* pDirect3DOther = NULL;
    pD3DDevice9->GetDirect3D(&pDirect3DOther);
    if (pDirect3DOther && !IsValidComInterfacePointer(pDirect3DOther))
    {
        SString message;
        message.Format("AddCapsReport: device returned invalid IDirect3D9 pointer %p", pDirect3DOther);
        AddReportLog(8753, message, 5);
    }
    else if (pDirect3DOther != pDirect3D)
    {
        WriteDebugEvent(SString("IDirect3D9 differs: %x %x", pDirect3D, pDirect3DOther));

        if (pDirect3DOther)
        {
            // Log graphic card name
            D3DADAPTER_IDENTIFIER9 AdapterIdent1;
            pDirect3D->GetAdapterIdentifier(Adapter, 0, &AdapterIdent1);
            WriteDebugEvent("pDirect3D:");
            WriteDebugEvent(ToString(AdapterIdent1));

            // Log graphic card name
            D3DADAPTER_IDENTIFIER9 AdapterIdent2;
            pDirect3DOther->GetAdapterIdentifier(Adapter, 0, &AdapterIdent2);
            WriteDebugEvent("pDirect3DOther:");
            WriteDebugEvent(ToString(AdapterIdent2));

            // Get caps from pDirect3DOther
            D3DCAPS9 D3DCaps9;
            hr = pDirect3DOther->GetDeviceCaps(Adapter, D3DDEVTYPE_HAL, &D3DCaps9);
            WriteDebugEvent(SString("pDirect3DOther CapsReport Caps9 - %s ", *ToString(D3DCaps9)));
        }
    }

    ReleaseInterface(pDirect3DOther, "AddCapsReport GetDirect3D");

    // Get caps from D3D
    D3DCAPS9 D3DCaps9;
    hr = pDirect3D->GetDeviceCaps(Adapter, D3DDEVTYPE_HAL, &D3DCaps9);
    WriteDebugEvent(SString("IDirect3D9 CapsReport Caps9 - %s ", *ToString(D3DCaps9)));

    // Get caps from Device
    D3DCAPS9 DeviceCaps9;
    hr = pD3DDevice9->GetDeviceCaps(&DeviceCaps9);
    WriteDebugEvent(SString("IDirect3DDevice9 CapsReport Caps9 - %s ", *ToString(DeviceCaps9)));

    // Test caps
    struct
    {
        DWORD CapsType;
        BYTE  VertexType;
    } DeclTypesList[] = {
        {D3DDTCAPS_UBYTE4, D3DDECLTYPE_UBYTE4},       {D3DDTCAPS_UBYTE4N, D3DDECLTYPE_UBYTE4N},   {D3DDTCAPS_SHORT2N, D3DDECLTYPE_SHORT2N},
        {D3DDTCAPS_SHORT4N, D3DDECLTYPE_SHORT4N},     {D3DDTCAPS_USHORT2N, D3DDECLTYPE_USHORT2N}, {D3DDTCAPS_USHORT4N, D3DDECLTYPE_USHORT4N},
        {D3DDTCAPS_UDEC3, D3DDECLTYPE_UDEC3},         {D3DDTCAPS_DEC3N, D3DDECLTYPE_DEC3N},       {D3DDTCAPS_FLOAT16_2, D3DDECLTYPE_FLOAT16_2},
        {D3DDTCAPS_FLOAT16_4, D3DDECLTYPE_FLOAT16_4},
    };

    // Try each vertex declaration type to see if it matches with what was advertised
    uint uiNumItems = NUMELMS(DeclTypesList);
    uint uiNumMatchesCaps = 0;
    for (uint i = 0; i < uiNumItems; i++)
    {
        // Try create
        D3DVERTEXELEMENT9 VertexElements[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()};
        VertexElements[0].Type = DeclTypesList[i].VertexType;
    IDirect3DVertexDeclaration9* pD3DVertexDecl = nullptr;
    hr = pD3DDevice9->CreateVertexDeclaration(VertexElements, &pD3DVertexDecl);
    ReleaseInterface(pD3DVertexDecl, "AddCapsReport CreateVertexDeclaration");

        // Check against device caps
        bool bCapsSaysOk = (DeviceCaps9.DeclTypes & DeclTypesList[i].CapsType) ? true : false;
        bool bMatchesCaps = (hr == D3D_OK) == (bCapsSaysOk == true);
        if (bMatchesCaps)
            uiNumMatchesCaps++;
        else
            WriteDebugEvent(SString("CapsReport - CreateVertexDeclaration %d/%d [MISMATCH] (VertexType:%d) result: %x (Matches caps:%d)", i, uiNumItems,
                                    DeclTypesList[i].VertexType, hr, bMatchesCaps));
    }
    WriteDebugEvent(SString("CapsReport - CreateVertexDeclarations MatchesCaps:%d/%d", uiNumMatchesCaps, uiNumItems));

    DWORD MaxActiveLights = std::min(DeviceCaps9.MaxActiveLights, pGTACaps9->MaxActiveLights);
    pGTACaps9->MaxActiveLights = MaxActiveLights;
    D3DCaps9.MaxActiveLights = MaxActiveLights;
    DeviceCaps9.MaxActiveLights = MaxActiveLights;

    DWORD MaxVertexBlendMatrixIndex = std::min(DeviceCaps9.MaxVertexBlendMatrixIndex, pGTACaps9->MaxVertexBlendMatrixIndex);
    pGTACaps9->MaxVertexBlendMatrixIndex = MaxVertexBlendMatrixIndex;
    D3DCaps9.MaxVertexBlendMatrixIndex = MaxVertexBlendMatrixIndex;
    DeviceCaps9.MaxVertexBlendMatrixIndex = MaxVertexBlendMatrixIndex;

    DWORD VertexProcessingCaps = DeviceCaps9.VertexProcessingCaps & pGTACaps9->VertexProcessingCaps;
    pGTACaps9->VertexProcessingCaps = VertexProcessingCaps;
    D3DCaps9.VertexProcessingCaps = VertexProcessingCaps;
    DeviceCaps9.VertexProcessingCaps = VertexProcessingCaps;

    bool DeviceCapsSameAsGTACaps = memcmp(&DeviceCaps9, pGTACaps9, sizeof(D3DCAPS9)) == 0;
    bool DeviceCapsSameAsD3DCaps9 = memcmp(&DeviceCaps9, &D3DCaps9, sizeof(D3DCAPS9)) == 0;

    WriteDebugEvent(SString("DeviceCaps==GTACaps:%d  DeviceCaps==D3DCaps9:%d", DeviceCapsSameAsGTACaps, DeviceCapsSameAsD3DCaps9));

    SString strDiffDesc1 = GetByteDiffDesc(&DeviceCaps9, pGTACaps9, sizeof(D3DCAPS9));
    if (!strDiffDesc1.empty())
        WriteDebugEvent(SString("DeviceCaps==GTACaps diff:%s", *strDiffDesc1.Left(500)));

    SString strDiffDesc2 = GetByteDiffDesc(&DeviceCaps9, &D3DCaps9, sizeof(D3DCAPS9));
    if (!strDiffDesc2.empty())
        WriteDebugEvent(SString("DeviceCaps==D3DCaps9 diff:%s", *strDiffDesc2.Left(500)));

    if (bFixGTACaps && !DeviceCapsSameAsGTACaps)
    {
        if (DeviceCaps9.DeclTypes > pGTACaps9->DeclTypes)
        {
            WriteDebugEvent("Not Fixing GTA caps as DeviceCaps is better");
        }
        else
        {
            WriteDebugEvent("Fixing GTA caps");
            memcpy(pGTACaps9, &DeviceCaps9, sizeof(D3DCAPS9));
        }
    }
}

////////////////////////////////////////////////
//
// CreateDeviceSecondCallCheck
//
// Check for, and handle subsequent calls to create device
// Know to occur with RTSSHooks.dll (EVGA Precision X on screen display)
//
////////////////////////////////////////////////
bool CreateDeviceSecondCallCheck(HRESULT& hOutResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                 D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    static uint uiCreateCount = 0;

    // Also check for invalid size
    if (pPresentationParameters->BackBufferWidth == 0)
    {
        WriteDebugEvent(SString(" Passing through call #%d to CreateDevice because size is invalid", uiCreateCount));
        hOutResult = pDirect3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
        return true;
    }

    // Also check for calls from other threads
    if (!IsMainThread())
    {
        SString strMessage(" Passing through call #%d to CreateDevice because not main thread", uiCreateCount);
        WriteDebugEvent(strMessage);
        AddReportLog(8627, strMessage);
        hOutResult = pDirect3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
        return true;
    }

    if (++uiCreateCount == 1)
        return false;
    WriteDebugEvent(SString(" Passing through call #%d to CreateDevice", uiCreateCount));
    hOutResult = pDirect3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
    return true;
}

////////////////////////////////////////////////
//
// HandleCreateDeviceResult
//
// Log result and possibly fix everything
//
////////////////////////////////////////////////
HRESULT HandleCreateDeviceResult(HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                 D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    // Log graphic card name
    D3DADAPTER_IDENTIFIER9 AdapterIdent;
    pDirect3D->GetAdapterIdentifier(Adapter, 0, &AdapterIdent);
    WriteDebugEvent(ToString(AdapterIdent));

    uint uiCurrentStatus = 0;            //  0-unknown  1-fail  2-success after retry  3-success

    if (hResult == D3D_OK)
    {
        // Log success and creation parameters
        WriteDebugEvent("CreateDevice success");
        uiCurrentStatus = CREATE_DEVICE_SUCCESS;
        WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));
    }

    if (hResult != D3D_OK)
    {
        // Handle failure of initial create device call
        WriteDebugEvent(SString("CreateDevice failed #0: %08x", hResult));

        // Try create device again
        hResult = DoCreateDevice(pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

        // Check if the result is correct (a custom d3d9.dll could return D3D_OK with a null pointer)
        if (hResult == D3D_OK)
        {
            IDirect3DDevice9* pCreatedDevice = *ppReturnedDeviceInterface;
            if (!pCreatedDevice)
            {
                AddReportLog(8755, SStringX("HandleCreateDeviceResult: DoCreateDevice returned nullptr device"), 5);
                hResult = D3DERR_INVALIDDEVICE;
            }
            else if (!IsValidComInterfacePointer(pCreatedDevice))
            {
                SString message;
                message.Format("HandleCreateDeviceResult: rejected invalid IDirect3DDevice9 pointer %p", pCreatedDevice);
                AddReportLog(8755, message, 5);
                ReleaseInterface(pCreatedDevice, "HandleCreateDeviceResult invalid return");
                *ppReturnedDeviceInterface = nullptr;
                hResult = D3DERR_INVALIDDEVICE;
            }
        }

        // Handle retry result
        if (hResult != D3D_OK)
        {
            WriteDebugEvent("--CreateDevice failed after retry");
            uiCurrentStatus = CREATE_DEVICE_FAIL;
        }
        else
        {
            WriteDebugEvent("++CreateDevice succeeded after retry");
            uiCurrentStatus = CREATE_DEVICE_RETRY_SUCCESS;

            // Apply input hook
            CMessageLoopHook::GetSingleton().ApplyHook(hFocusWindow);

            GetVideoModeManager()->PostCreateDevice(*ppReturnedDeviceInterface, pPresentationParameters);

            // We must first store the presentation values.
            CDirect3DData::GetSingleton().StoreViewport(0, 0, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

            // Calc and store readable depth format for shader use
            ERenderFormat ReadableDepthFormat = CDirect3DEvents9::DiscoverReadableDepthFormat(
                *ppReturnedDeviceInterface, pPresentationParameters->MultiSampleType, pPresentationParameters->Windowed != 0);
            CGraphics::GetSingleton().GetRenderItemManager()->SetDepthBufferFormat(ReadableDepthFormat);

            // Now create the proxy device.
            IDirect3DDevice9* pOriginalDevice = *ppReturnedDeviceInterface;
            *ppReturnedDeviceInterface = new CProxyDirect3DDevice9(pOriginalDevice);
            if (pOriginalDevice)
            {
                if (IsValidComInterfacePointer(pOriginalDevice))
                {
                    pOriginalDevice->Release();
                }
                else
                {
                    SString message;
                    message.Format("HandleCreateDeviceResult: skipping Release on invalid original device pointer %p", pOriginalDevice);
                    AddReportLog(8755, message, 5);
                }
            }

            // Debug output
            D3DDEVICE_CREATION_PARAMETERS parameters;
            (*ppReturnedDeviceInterface)->GetCreationParameters(&parameters);

            WriteDebugEvent(SString("    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x  ReadableDepth:%s", parameters.AdapterOrdinal, parameters.DeviceType,
                                    parameters.BehaviorFlags, ReadableDepthFormat ? std::string((char*)&ReadableDepthFormat, 4).c_str() : "None"));
        }
    }

    uint uiLastStatus = GetApplicationSettingInt("diagnostics", "createdevice-last-status");
    SetApplicationSettingInt("diagnostics", "createdevice-last-status", uiCurrentStatus);

    // Calc log level to use
    uint uiDiagnosticLogLevel = 0;
    if (uiLastStatus == CREATE_DEVICE_FAIL && uiCurrentStatus != CREATE_DEVICE_FAIL)
        uiDiagnosticLogLevel = 1;            // Log and continue - If changing from fail status
    if (uiCurrentStatus == CREATE_DEVICE_FAIL)
        uiDiagnosticLogLevel = 2;            // Log and wait - If fail status

    bool bDetectOptimus = (GetModuleHandle("nvd3d9wrap.dll") != NULL);

    bool bFixCaps = false;
    if (GetApplicationSettingInt("nvhacks", "optimus") || bDetectOptimus)
    {
        bFixCaps = true;
        if (uiDiagnosticLogLevel == 0)
            uiDiagnosticLogLevel = 1;
    }

    AddCapsReport(Adapter, pDirect3D, *ppReturnedDeviceInterface, bFixCaps);

    if (uiDiagnosticLogLevel)
    {
        // Prevent statup warning in loader
        WatchDogCompletedSection("L3");
        CCore::GetSingleton().GetNetwork()->ResetStub('dia3', *ms_strExtraLogBuffer, uiDiagnosticLogLevel);
    }
    ms_strExtraLogBuffer.clear();

    if (hResult != D3D_OK)
    {
        // Handle fatal error
        SString strMessage;
        strMessage += "There was a problem starting MTA:SA\n\n";
        strMessage += SString("Direct3D CreateDevice error: %08x", hResult);
        BrowseToSolution("d3dcreatedevice-fail", EXIT_GAME_FIRST | ASK_GO_ONLINE, strMessage);
    }

    return hResult;
}

namespace
{
    DWORD                 BehaviorFlagsOrig = 0;
    D3DPRESENT_PARAMETERS presentationParametersOrig;
}            // namespace

////////////////////////////////////////////////
//
// Hook CCore::OnPreCreateDevice
//
// Modify paramters
//
////////////////////////////////////////////////
void CCore::OnPreCreateDevice(IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD& BehaviorFlags,
                              D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    assert(0);

    if (!UsingAltD3DSetup())
        return;

    WriteDebugEvent("CCore::OnPreCreateDevice");

    uint uiPrevResult = GetApplicationSettingInt("diagnostics", "last-create-device-result");
    if (uiPrevResult)
    {
        // Failed last time, so as a test for logging, try a create device with no modifications
        WriteDebugEvent(SString("Previous CreateDevice failed with: %08x", uiPrevResult));
        WriteDebugEvent("  Test unmodified:");
        WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));
    IDirect3DDevice9* pReturnedDeviceInterface = NULL;
    HRESULT hResult = pDirect3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &pReturnedDeviceInterface);
    ReleaseInterface(pReturnedDeviceInterface, "CCore::OnPreCreateDevice temp release");
        WriteDebugEvent(SString("  Unmodified result is: %08x", hResult));
    }

    // Save original values for later
    BehaviorFlagsOrig = BehaviorFlags;
    presentationParametersOrig = *pPresentationParameters;

    WriteDebugEvent("  Original paramters:");
    WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));

    // Make sure DirectX Get...() calls will work
    BehaviorFlags &= ~D3DCREATE_PUREDEVICE;

    // Enable the auto depth stencil parameter
    pPresentationParameters->EnableAutoDepthStencil = true;

    GetVideoModeManager()->PreCreateDevice(pPresentationParameters);

    WriteDebugEvent("  Modified paramters:");
    WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));
}

////////////////////////////////////////////////
//
// Hook CCore::OnPostCreateDevice
//
// Wrap device or log failure
//
////////////////////////////////////////////////
HRESULT CCore::OnPostCreateDevice(HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                  D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    if (!UsingAltD3DSetup())
        return hResult;

    if (!ppReturnedDeviceInterface)
    {
        AddReportLog(8744, SStringX("CCore::OnPostCreateDevice - missing ppReturnedDeviceInterface pointer"));
        return hResult;
    }

    if (!SharedUtil::IsReadablePointer(ppReturnedDeviceInterface, sizeof(*ppReturnedDeviceInterface)))
    {
        SString message;
        message.Format("CCore::OnPostCreateDevice - invalid ppReturnedDeviceInterface pointer %p", ppReturnedDeviceInterface);
        AddReportLog(8745, message);
        return hResult;
    }

    if (!*ppReturnedDeviceInterface)
    {
        AddReportLog(8746, SStringX("CCore::OnPostCreateDevice - ppReturnedDeviceInterface dereferenced to nullptr"));
        return hResult;
    }

    if (!IsValidComInterfacePointer(*ppReturnedDeviceInterface))
    {
        SString message;
        message.Format("CCore::OnPostCreateDevice - invalid IDirect3DDevice9 pointer %p (via %p)", *ppReturnedDeviceInterface, ppReturnedDeviceInterface);
        AddReportLog(8747, message);
        return hResult;
    }

    IDirect3DDevice9* pDevice = *ppReturnedDeviceInterface;

    //
    // - Allow create device with no changes
    // - Check caps and report diff with GTA caps
    // - Release device
    //
    WriteDebugEvent("CCore::OnPostCreateDevice - Alt startup used");

    if (hResult != D3D_OK)
        WriteDebugEvent(SString("Initial CreateDevice failed: %08x", hResult));
    else
        WriteDebugEvent("Initial CreateDevice succeeded");

    AddCapsReport(Adapter, pDirect3D, pDevice, false);

    ReleaseInterface(pDevice, "CCore::OnPostCreateDevice temp release");
    *ppReturnedDeviceInterface = pDevice;

    //
    // - Create device with required changes
    // - Check caps and report diff with GTA caps
    // - Fix GTA caps if needed
    //

    // Save original values for later
    BehaviorFlagsOrig = BehaviorFlags;
    presentationParametersOrig = *pPresentationParameters;

    WriteDebugEvent("  Original paramters:");
    WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));

    // Make sure DirectX Get...() calls will work
    BehaviorFlags &= ~D3DCREATE_PUREDEVICE;

    // Enable the auto depth stencil parameter
    pPresentationParameters->EnableAutoDepthStencil = true;

    GetVideoModeManager()->PreCreateDevice(pPresentationParameters);

    WriteDebugEvent("  Modified paramters:");
    WriteDebugEvent(ToString(Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters));

    hResult = CreateDeviceInsist(2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

    // Check if the result is correct (a custom d3d9.dll could return D3D_OK with a null pointer)
    if (hResult == D3D_OK)
    {
        IDirect3DDevice9* pCreatedDevice = *ppReturnedDeviceInterface;
        if (!pCreatedDevice)
        {
            AddReportLog(8755,
                         "CCore::OnPostCreateDevice: CreateDeviceInsist returned nullptr device", 5);
            hResult = D3DERR_INVALIDDEVICE;
        }
        else if (!IsValidComInterfacePointer(pCreatedDevice))
        {
            AddReportLog(8755,
                         SString("CCore::OnPostCreateDevice: rejected invalid IDirect3DDevice9 pointer %p", pCreatedDevice), 5);
            ReleaseInterface(pCreatedDevice, "CCore::OnPostCreateDevice invalid return");
            *ppReturnedDeviceInterface = nullptr;
            hResult = D3DERR_INVALIDDEVICE;
        }
    }

    if (hResult != D3D_OK)
        WriteDebugEvent(SString("MTA CreateDevice failed: %08x", hResult));
    else
        WriteDebugEvent("MTA CreateDevice succeeded");

    if (hResult == D3D_OK)
        AddCapsReport(Adapter, pDirect3D, *ppReturnedDeviceInterface, true);

    // Change the window title to MTA: San Andreas
    #ifdef MTA_DEBUG
    SetWindowTextW(hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas [DEBUG]").c_str());
    #else
    SetWindowTextW(hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas").c_str());
    #endif

    // Log graphic card name
    D3DADAPTER_IDENTIFIER9 AdapterIdent;
    pDirect3D->GetAdapterIdentifier(Adapter, 0, &AdapterIdent);
    WriteDebugEvent(ToString(AdapterIdent));

    // Store the rendering window in the direct 3d data
    CDirect3DData::GetSingleton().StoreDeviceWindow(pPresentationParameters->hDeviceWindow);

    // Apply input hook
    CMessageLoopHook::GetSingleton().ApplyHook(hFocusWindow);

    // Make sure the object was created successfully.
    if (hResult == D3D_OK)
    {
        WriteDebugEvent("CreateDevice succeeded");
        GetVideoModeManager()->PostCreateDevice(*ppReturnedDeviceInterface, pPresentationParameters);

        // We must first store the presentation values.
        CDirect3DData::GetSingleton().StoreViewport(0, 0, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

        // Calc and store readable depth format for shader use
        ERenderFormat ReadableDepthFormat = CDirect3DEvents9::DiscoverReadableDepthFormat(*ppReturnedDeviceInterface, pPresentationParameters->MultiSampleType,
                                                                                          pPresentationParameters->Windowed != 0);
        CGraphics::GetSingleton().GetRenderItemManager()->SetDepthBufferFormat(ReadableDepthFormat);

        // Now create the proxy device.
        IDirect3DDevice9* pOriginalDevice = *ppReturnedDeviceInterface;
        *ppReturnedDeviceInterface = new CProxyDirect3DDevice9(pOriginalDevice);
        if (pOriginalDevice)
        {
            if (IsValidComInterfacePointer(pOriginalDevice))
            {
                pOriginalDevice->Release();
            }
            else
            {
                AddReportLog(8755,
                             SString("CCore::OnPostCreateDevice - skipping Release on invalid original device pointer %p", pOriginalDevice), 5);
            }
        }

        // Debug output
        D3DDEVICE_CREATION_PARAMETERS parameters;
        (*ppReturnedDeviceInterface)->GetCreationParameters(&parameters);

        WriteDebugEvent("   Used creation parameters:");
        WriteDebugEvent(SString("    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x  ReadableDepth:%s", parameters.AdapterOrdinal, parameters.DeviceType,
                                parameters.BehaviorFlags, ReadableDepthFormat ? std::string((char*)&ReadableDepthFormat, 4).c_str() : "None"));
    }

    bool bDetectOptimus = (GetModuleHandle("nvd3d9wrap.dll") != NULL);

    // Calc log level to use
    uint uiDiagnosticLogLevel = 0;
    if (GetApplicationSettingInt("nvhacks", "optimus") || bDetectOptimus)
        uiDiagnosticLogLevel = 1;            // Log and continue
    if (hResult != D3D_OK)
        uiDiagnosticLogLevel = 2;            // Log and wait - If fail status

    // Do diagnostic log now if needed
    if (uiDiagnosticLogLevel)
    {
        // Prevent statup warning in loader
        WatchDogCompletedSection("L3");

        // Run diagnostic
        CCore::GetSingleton().GetNetwork()->ResetStub('dia3', *ms_strExtraLogBuffer, uiDiagnosticLogLevel);
    }
    ms_strExtraLogBuffer.clear();

    // Handle fatal error
    if (hResult != D3D_OK)
    {
        // Inform user
        SString strMessage;
        strMessage += "There was a problem starting MTA:SA\n\n";
        strMessage += SString("Direct3D CreateDevice error: %08x", hResult);
        BrowseToSolution("d3dcreatedevice-fail", EXIT_GAME_FIRST | ASK_GO_ONLINE, strMessage);
    }

    return hResult;
}
