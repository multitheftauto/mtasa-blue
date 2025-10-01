/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CProxyDirect3DDevice9.cpp
 *  PURPOSE:     Direct3D 9 device function hooking proxy
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>
#include <game/CSettings.h>

class CProxyDirect3DDevice9;
extern std::atomic<bool> g_bInMTAScene;

namespace
{
template <typename T>
void ReleaseInterface(T*& pointer)
{
    if (pointer)
    {
        pointer->Release();
        pointer = nullptr;
    }
}

template <typename T>
void ReplaceInterface(T*& destination, T* source)
{
    if (destination == source)
        return;

    ReleaseInterface(destination);
    destination = source;
    if (destination)
        destination->AddRef();
}
std::mutex               g_proxyDeviceMutex;
std::mutex               g_gammaStateMutex;
std::mutex               g_deviceStateMutex;
std::atomic<uint64_t>    g_proxyRegistrationCounter{0};
std::mutex               g_sceneStateMutex;
uint32_t                 g_gtaSceneActiveCount = 0;
uint64_t                 g_activeProxyRegistrationId = 0;

uint64_t RegisterProxyDevice(CProxyDirect3DDevice9* instance);
bool      UnregisterProxyDevice(CProxyDirect3DDevice9* instance, uint64_t registrationId);
}

std::atomic<bool>                       g_bInGTAScene{false};
CProxyDirect3DDevice9*                  g_pProxyDevice = NULL;
CProxyDirect3DDevice9::SD3DDeviceState* g_pDeviceState = NULL;
SGammaState                             g_GammaState;

void IncrementGTASceneState()
{
    std::lock_guard<std::mutex> lock(g_sceneStateMutex);
    ++g_gtaSceneActiveCount;
    g_bInGTAScene.store(true, std::memory_order_release);
}

void DecrementGTASceneState()
{
    std::lock_guard<std::mutex> lock(g_sceneStateMutex);
    if (g_gtaSceneActiveCount > 0)
        --g_gtaSceneActiveCount;
    g_bInGTAScene.store(g_gtaSceneActiveCount > 0, std::memory_order_release);
}

void ResetGTASceneState()
{
    std::lock_guard<std::mutex> lock(g_sceneStateMutex);
    g_gtaSceneActiveCount = 0;
    g_bInGTAScene.store(false, std::memory_order_release);
}

// Proxy constructor and destructor.
// Constructor performs heavy initialization; defer global registration until the end to avoid exposing a partially built object.
CProxyDirect3DDevice9::CProxyDirect3DDevice9(IDirect3DDevice9* pDevice)
    : m_pDevice(pDevice)
    , m_pData(nullptr)
    , m_ulRefCount(1)
    , m_registrationToken(0)
    , m_lastTestCooperativeLevelResult(D3D_OK)
{
    WriteDebugEvent(SString("CProxyDirect3DDevice9::CProxyDirect3DDevice9 %08x (device: %08x)", this, pDevice));

    struct DeviceRefGuard
    {
        IDirect3DDevice9* device;
        bool              active;
        DeviceRefGuard(IDirect3DDevice9* d) : device(d), active(true) {}
        ~DeviceRefGuard()
        {
            if (active && device)
                device->Release();
        }
        void Dismiss() { active = false; }
    } deviceGuard(m_pDevice);

    if (m_pDevice)
    {
        m_pDevice->AddRef();
    }

    m_pData = CDirect3DData::GetSingletonPtr();

    if (m_pDevice)
    {
        m_pDevice->GetDeviceCaps(&DeviceState.DeviceCaps);

        D3DDEVICE_CREATION_PARAMETERS creationParameters;
        m_pDevice->GetCreationParameters(&creationParameters);
        int iAdapter = creationParameters.AdapterOrdinal;

        IDirect3D9* pD3D9 = CProxyDirect3D9::StaticGetDirect3D();
        bool        bNeedRelease = false;
        if (!pD3D9)
        {
            m_pDevice->GetDirect3D(&pD3D9);
            bNeedRelease = true;        // GetDirect3D increments reference count
        }

        if (pD3D9)
        {
            D3DADAPTER_IDENTIFIER9 adaptIdent;
            ZeroMemory(&adaptIdent, sizeof(D3DADAPTER_IDENTIFIER9));
            pD3D9->GetAdapterIdentifier(iAdapter, 0, &adaptIdent);

            int iVideoCardMemoryKBTotal = GetWMIVideoAdapterMemorySize(adaptIdent.VendorId, adaptIdent.DeviceId) / 1024;

            // Just incase, fallback to using texture memory stats
            if (iVideoCardMemoryKBTotal < 16)
                iVideoCardMemoryKBTotal = m_pDevice->GetAvailableTextureMem() / 1024;

            DeviceState.AdapterState.InstalledMemoryKB = iVideoCardMemoryKBTotal;

            // Get video card name
            DeviceState.AdapterState.Name = adaptIdent.Description;

            // Clipping is required for some graphic configurations
            DeviceState.AdapterState.bRequiresClipping = SStringX(adaptIdent.Description).Contains("Intel");

            // Release D3D9 interface if we obtained it via GetDirect3D
            if (bNeedRelease)
            {
                SAFE_RELEASE(pD3D9);
            }
        }
        else
        {
            WriteDebugEvent("Warning: Could not obtain IDirect3D9 interface for adapter info");

            // Set default values
            DeviceState.AdapterState.InstalledMemoryKB = m_pDevice->GetAvailableTextureMem() / 1024;
            DeviceState.AdapterState.Name = "Unknown";
            DeviceState.AdapterState.bRequiresClipping = false;
        }

        // Get max anisotropic setting
        DeviceState.AdapterState.MaxAnisotropicSetting = 0;

        // Make sure device can do anisotropic minification and trilinear filtering
        if ((DeviceState.DeviceCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) &&
            (DeviceState.DeviceCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
        {
            int iLevel = std::max<int>(1, DeviceState.DeviceCaps.MaxAnisotropy);
            // Convert level 1/2/4/8/16 into setting 0/1/2/3/4
            while (iLevel >>= 1)
                DeviceState.AdapterState.MaxAnisotropicSetting++;
        }

        WriteDebugEvent(SString("*** Using adapter: %s (Mem:%d KB, MaxAnisotropy:%d)", (const char*)DeviceState.AdapterState.Name,
                                DeviceState.AdapterState.InstalledMemoryKB, DeviceState.AdapterState.MaxAnisotropicSetting));
    }

    const uint64_t registrationToken = RegisterProxyDevice(this);
    struct RegistrationGuard
    {
        CProxyDirect3DDevice9* instance;
        uint64_t               token;
        bool                   active;
        RegistrationGuard(CProxyDirect3DDevice9* inst, uint64_t tok) : instance(inst), token(tok), active(true) {}
        ~RegistrationGuard()
        {
            if (active && token != 0)
                UnregisterProxyDevice(instance, token);
        }
        void Dismiss() { active = false; }
    } registrationGuard(this, registrationToken);

    m_registrationToken = registrationToken;

    // Give a default value for the streaming memory setting
    if (g_pCore->GetCVars()->Exists("streaming_memory") == false)
        g_pCore->GetCVars()->Set("streaming_memory", g_pCore->GetMaxStreamingMemory());

    CDirect3DEvents9::OnDirect3DDeviceCreate(m_pDevice);

    registrationGuard.Dismiss();
    deviceGuard.Dismiss();
}

CProxyDirect3DDevice9::~CProxyDirect3DDevice9()
{
    WriteDebugEvent(SString("CProxyDirect3DDevice9::~CProxyDirect3DDevice9 %08x", this));
    const bool bWasRegistered = UnregisterProxyDevice(this, m_registrationToken);

    bool          bRestoreGamma = false;
    UINT          lastSwapChain = 0;
    D3DGAMMARAMP  originalGammaRamp{};

    if (bWasRegistered)
    {
        ResetGTASceneState();

        std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
        if (g_GammaState.bOriginalGammaStored)
        {
            originalGammaRamp = g_GammaState.originalGammaRamp;
            lastSwapChain = g_GammaState.lastSwapChain;
            bRestoreGamma = true;

            g_GammaState.bOriginalGammaStored = false;
            g_GammaState.bLastWasBorderless = false;
            ZeroMemory(&g_GammaState.originalGammaRamp, sizeof(g_GammaState.originalGammaRamp));
        }
    }

    if (bRestoreGamma && m_pDevice)
    {
        IDirect3DDevice9* pDevice = m_pDevice;
        pDevice->AddRef();
        pDevice->SetGammaRamp(lastSwapChain, 0, &originalGammaRamp);
        pDevice->Release();
        WriteDebugEvent("Restored original gamma ramp on device destruction");
    }

    // Release any cached COM references we held onto for diagnostics
    ReleaseCachedResources();

    // Release our reference to the wrapped device
    // Note: We don't check m_ulRefCount here because destructor is only called
    // when Release() determined the count reached 0
    if (m_pDevice)
    {
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
    
}

/*** IUnknown methods ***/
HRESULT CProxyDirect3DDevice9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (!ppvObj)
        return E_POINTER;
        
    *ppvObj = nullptr;
    
    // Check if its for IUnknown or IDirect3DDevice9
    if (riid == IID_IUnknown || riid == IID_IDirect3DDevice9)
    {
        *ppvObj = static_cast<IDirect3DDevice9*>(this);
        AddRef();
        return S_OK;
    }
    
    // For any other interface, forward to underlying device
    // But this means the caller gets the underlying device, not our proxy
    return m_pDevice->QueryInterface(riid, ppvObj);
}

ULONG CProxyDirect3DDevice9::AddRef()
{
    // Only increment proxy reference count
    // We keep a single reference to the underlying device throughout the lifetime
    return InterlockedIncrement(&m_ulRefCount);
}

ULONG CProxyDirect3DDevice9::Release()
{
    // Only decrement proxy reference count  
    ULONG ulNewRefCount = InterlockedDecrement(&m_ulRefCount);
    
    if (ulNewRefCount == 0)
    {
        WriteDebugEvent("Releasing IDirect3DDevice9 Proxy...");
        // Call event handler
        CDirect3DEvents9::OnDirect3DDeviceDestroy(m_pDevice);
        delete this;
        return 0;
    }

    return ulNewRefCount;
}

/*** IDirect3DDevice9 methods ***/
HRESULT CProxyDirect3DDevice9::TestCooperativeLevel()
{
    HRESULT hResult = m_pDevice->TestCooperativeLevel();
    
    // Log device state transitions for debugging GPU driver issues
    if (hResult != m_lastTestCooperativeLevelResult)
    {
        WriteDebugEvent(SString("CProxyDirect3DDevice9::TestCooperativeLevel - Device state changed: %08x -> %08x", m_lastTestCooperativeLevelResult, hResult));
        m_lastTestCooperativeLevelResult = hResult;
        
        // Additional safety for when transitioning to/from lost states
        if (hResult == D3DERR_DEVICELOST || hResult == D3DERR_DEVICENOTRESET)
        {
            // Give the driver a moment to stabilize
            Sleep(1);
        }
    }
    
    return hResult;
}

UINT CProxyDirect3DDevice9::GetAvailableTextureMem()
{
    return m_pDevice->GetAvailableTextureMem();
}

HRESULT CProxyDirect3DDevice9::EvictManagedResources()
{
    return m_pDevice->EvictManagedResources();
}

HRESULT CProxyDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9)
{
    return m_pDevice->GetDirect3D(ppD3D9);
}

HRESULT CProxyDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps)
{
    return m_pDevice->GetDeviceCaps(pCaps);
}

HRESULT CProxyDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
    return m_pDevice->GetDisplayMode(iSwapChain, pMode);
}

HRESULT CProxyDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
    return m_pDevice->GetCreationParameters(pParameters);
}

HRESULT CProxyDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
    return m_pDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void CProxyDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags)
{
    m_pDevice->SetCursorPosition(X, Y, Flags);
}

BOOL CProxyDirect3DDevice9::ShowCursor(BOOL bShow)
{
    return m_pDevice->ShowCursor(bShow);
}

HRESULT CProxyDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
    return CDirect3DEvents9::CreateAdditionalSwapChainGuarded(m_pDevice, pPresentationParameters, pSwapChain);
}

HRESULT CProxyDirect3DDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
    return m_pDevice->GetSwapChain(iSwapChain, pSwapChain);
}

UINT CProxyDirect3DDevice9::GetNumberOfSwapChains()
{
    return m_pDevice->GetNumberOfSwapChains();
}

////////////////////////////////////////////////
//
// ResetDeviceInsist
//
// Keep trying reset device for a little bit
//
////////////////////////////////////////////////
HRESULT ResetDeviceInsist(uint uiMinTries, uint uiTimeout, IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    HRESULT      hResult;
    CElapsedTime retryTimer;
    uint         uiRetryCount = 0;
    do
    {
        hResult = pDevice->Reset(pPresentationParameters);
        if (hResult == D3D_OK)
        {
            WriteDebugEvent(SString("   -- ResetDeviceInsist succeeded on try #%d", uiRetryCount + 1));
            break;
        }
        Sleep(100);
    } while (++uiRetryCount < uiMinTries || retryTimer.Get() < uiTimeout);

    return hResult;
}

////////////////////////////////////////////////
//
// DoResetDevice
//
// In various ways
//
////////////////////////////////////////////////
HRESULT DoResetDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DPRESENT_PARAMETERS& presentationParametersOrig)
{
    HRESULT hResult;
    hResult = pDevice->Reset(pPresentationParameters);

    if (SUCCEEDED(hResult))
    {
        // Log success
        WriteDebugEvent("Reset success");
    }
    else if (FAILED(hResult))
    {
        // Handle failure of initial reset device call
        g_pCore->LogEvent(7124, "Direct3D", "Direct3DDevice9::Reset", SString("Fail0:%08x", hResult));
        WriteDebugEvent(SString("Reset failed #0: %08x", hResult));

        // Try reset device again
        hResult = ResetDeviceInsist(5, 1000, pDevice, pPresentationParameters);

        // Handle retry result
        if (FAILED(hResult))
        {
            // Record problem
            g_pCore->LogEvent(7124, "Direct3D", "Direct3DDevice9::Reset", SString("Fail1:%08x", hResult));
            WriteDebugEvent(SString("Direct3DDevice9::Reset  Fail1:%08x", hResult));

            // Try with original presentation parameters
            *pPresentationParameters = presentationParametersOrig;
            hResult = ResetDeviceInsist(5, 1000, pDevice, pPresentationParameters);

            if (FAILED(hResult))
            {
                // Record problem
                g_pCore->LogEvent(7124, "Direct3D", "Direct3DDevice9::Reset", SString("Fail2:%08x", hResult));
                WriteDebugEvent(SString("Direct3DDevice9::Reset  Fail2:%08x", hResult));

                // Prevent statup warning in loader
                WatchDogCompletedSection("L3");

                // Try removing anti-aliasing for next time
                if (CGame* pGame = g_pCore->GetGame())
                {
                    CGameSettings* pGameSettings = pGame->GetSettings();
                    int            iAntiAliasing = pGameSettings->GetAntiAliasing();
                    if (iAntiAliasing > 1)
                    {
                        pGameSettings->SetAntiAliasing(1, true);
                        pGameSettings->Save();
                    }
                }

                // Handle fatal error
                SString strMessage;
                strMessage += "There was a problem resetting Direct3D\n\n";
                strMessage += SString("Direct3DDevice9 Reset error: %08x", hResult);
                BrowseToSolution("d3dresetdevice-fail", EXIT_GAME_FIRST | ASK_GO_ONLINE, strMessage);
                // Won't get here as BrowseToSolution will terminate the process
            }
        }
    }
    return hResult;
}

static bool WaitForGpuIdle(IDirect3DDevice9* pDevice)
{
    if (!pDevice)
        return false;

    IDirect3DQuery9* pEventQuery = nullptr;
    const HRESULT    hrCreate = pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &pEventQuery);
    if (FAILED(hrCreate) || !pEventQuery)
        return false;

    bool bCompleted = false;
    if (SUCCEEDED(pEventQuery->Issue(D3DISSUE_END)))
    {
        constexpr int kMaxAttempts = 16;
        for (int attempt = 0; attempt < kMaxAttempts; ++attempt)
        {
            const HRESULT hrData = pEventQuery->GetData(nullptr, 0, D3DGETDATA_FLUSH);
            if (hrData == S_OK)
            {
                bCompleted = true;
                break;
            }
            if (hrData == D3DERR_DEVICELOST)
            {
                break;
            }

            Sleep(0);
        }
    }

    pEventQuery->Release();
    return bCompleted;
}

HRESULT CProxyDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    WriteDebugEvent("CProxyDirect3DDevice9::Reset");

    // Validate input parameters
    if (!pPresentationParameters)
    {
        WriteDebugEvent("CProxyDirect3DDevice9::Reset - Invalid presentation parameters");
        return D3DERR_INVALIDCALL;
    }
    
    // Reset gamma state since display mode might change
    {
        std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
        g_GammaState.bOriginalGammaStored = false;
        g_GammaState.bLastWasBorderless = false;
        ZeroMemory(&g_GammaState.originalGammaRamp, sizeof(g_GammaState.originalGammaRamp));
    }
    WriteDebugEvent("Reset gamma state due to device reset");
    
    // Check cooperative level before attempting reset
    HRESULT hCoopLevel = m_pDevice->TestCooperativeLevel();
    if (hCoopLevel == D3DERR_DEVICELOST)
    {
        WriteDebugEvent("CProxyDirect3DDevice9::Reset - Device still lost, cannot reset yet");
        return hCoopLevel;
    }
    else if (hCoopLevel != D3DERR_DEVICENOTRESET && hCoopLevel != D3D_OK)
    {
        WriteDebugEvent(SString("CProxyDirect3DDevice9::Reset - Device in unexpected state: %08x", hCoopLevel));
        return hCoopLevel;
    }

    // Save presentation parameters
    D3DPRESENT_PARAMETERS presentationParametersOrig = *pPresentationParameters;

    CVideoModeManagerInterface* pVideoModeManager = GetVideoModeManager();
    if (pVideoModeManager)
    {
        pVideoModeManager->PreReset(pPresentationParameters);
    }
    else
    {
        WriteDebugEvent("CProxyDirect3DDevice9::Reset - Video mode manager unavailable during PreReset");
    }

    HRESULT hResult;

    // Call our event handler.
    CDirect3DEvents9::OnInvalidate(m_pDevice);

    // Release cached state so lingering references can't block Reset on default-pool resources
    ReleaseCachedResources();

    // Give GPU driver time to complete any pending operations. We keep the light Sleep as a
    // conservative fallback but first rely on an event query, as recommended in Microsoft's GPU
    // synchronization guidance, to ensure outstanding commands finish.
    if (!WaitForGpuIdle(m_pDevice))
        WriteDebugEvent("CProxyDirect3DDevice9::Reset - WaitForGpuIdle timed out or failed");

    Sleep(1);
    
    // Call the real reset routine.
    hResult = DoResetDevice(m_pDevice, pPresentationParameters, presentationParametersOrig);

    if (SUCCEEDED(hResult))
    {
        // Store actual present parameters used
        IDirect3DSwapChain9* pSwapChain = nullptr;
        HRESULT              hrSwapChain = m_pDevice->GetSwapChain(0, &pSwapChain);
        if (FAILED(hrSwapChain))
        {
            WriteDebugEvent(SString("Warning: Failed to get swap chain for parameter storage: %08x", hrSwapChain));
        }
        else if (!pSwapChain)
        {
            WriteDebugEvent("Warning: GetSwapChain succeeded but returned null swap chain");
        }

        // Store device creation parameters as well, keeping both updates atomic relative to device state changes
        HRESULT hrCreationParams = D3D_OK;
        {
            std::lock_guard<std::mutex> stateGuard(g_deviceStateMutex);
            if (g_pDeviceState)
            {
                if (SUCCEEDED(hrSwapChain) && pSwapChain)
                {
                    pSwapChain->GetPresentParameters(&g_pDeviceState->CreationState.PresentationParameters);
                }
                hrCreationParams = m_pDevice->GetCreationParameters(&g_pDeviceState->CreationState.CreationParameters);
            }
            else
            {
                hrCreationParams = D3DERR_INVALIDCALL;
            }
        }

        // Always release the swap chain if it was obtained, regardless of success/failure
        ReleaseInterface(pSwapChain);

        if (FAILED(hrCreationParams))
        {
            WriteDebugEvent(SString("Warning: Failed to get creation parameters: %08x", hrCreationParams));
        }

        // Only perform post-reset operations on successful reset
        g_pCore->LogEvent(7123, "Direct3D", "Direct3DDevice9::Reset", "Success");
        if (pVideoModeManager)
        {
            pVideoModeManager->PostReset(pPresentationParameters);
        }
        else
        {
            WriteDebugEvent("CProxyDirect3DDevice9::Reset - Skipping PostReset because video mode manager is unavailable");
        }

    // Update our data.
    m_pData->StoreViewport(0, 0, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

        // Ensure scene state is properly restored
        // Call our event handler.
        CDirect3DEvents9::OnRestore(m_pDevice);
        
        // Additional sync point for GPU driver
        if (!BeginSceneWithoutProxy(m_pDevice, ESceneOwner::MTA))
        {
            WriteDebugEvent("CProxyDirect3DDevice9::Reset - Failed to begin scene for driver sync");
        }
        else if (!EndSceneWithoutProxy(m_pDevice, ESceneOwner::MTA))
        {
            WriteDebugEvent("CProxyDirect3DDevice9::Reset - Failed to end scene for driver sync");
        }

    WriteDebugEvent(SString("    BackBufferWidth:%d  Height:%d  Format:%d  Count:%d", pPresentationParameters->BackBufferWidth,
                            pPresentationParameters->BackBufferHeight, pPresentationParameters->BackBufferFormat, pPresentationParameters->BackBufferCount));

    WriteDebugEvent(SString("    MultiSampleType:%d  Quality:%d", pPresentationParameters->MultiSampleType, pPresentationParameters->MultiSampleQuality));

    WriteDebugEvent(SString("    SwapEffect:%d  Windowed:%d  EnableAutoDepthStencil:%d  AutoDepthStencilFormat:%d  Flags:0x%x",
                            pPresentationParameters->SwapEffect, pPresentationParameters->Windowed, pPresentationParameters->EnableAutoDepthStencil,
                            pPresentationParameters->AutoDepthStencilFormat, pPresentationParameters->Flags));

    WriteDebugEvent(SString("    FullScreen_RefreshRateInHz:%d  PresentationInterval:0x%08x", pPresentationParameters->FullScreen_RefreshRateInHz,
                            pPresentationParameters->PresentationInterval));

    {
        std::lock_guard<std::mutex> stateGuard(g_deviceStateMutex);
        if (g_pDeviceState)
        {
            const D3DDEVICE_CREATION_PARAMETERS& parameters = g_pDeviceState->CreationState.CreationParameters;
            WriteDebugEvent(SString("    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x", parameters.AdapterOrdinal, parameters.DeviceType,
                                    parameters.BehaviorFlags));
        }
    }
    }
    else
    {
        WriteDebugEvent(SString("CProxyDirect3DDevice9::Reset failed with HRESULT: %08x", hResult));
    }

    return hResult;
}


HRESULT CProxyDirect3DDevice9::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    // Reset frame stat counters
    memset(&DeviceState.FrameStats, 0, sizeof(DeviceState.FrameStats));

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCoopLevel = D3DERR_INVALIDCALL;
    if (!CDirect3DEvents9::IsDeviceOperational(m_pDevice, &bDeviceTemporarilyLost, &hrCoopLevel))
    {
        if (bDeviceTemporarilyLost)
            WriteDebugEvent(SString("CProxyDirect3DDevice9::Present skipped due to device state: %08x", hrCoopLevel));
        else if (hrCoopLevel != D3D_OK)
            WriteDebugEvent(SString("CProxyDirect3DDevice9::Present unexpected cooperative level: %08x", hrCoopLevel));
        else
            WriteDebugEvent("CProxyDirect3DDevice9::Present invalid device state");

        return (hrCoopLevel != D3D_OK) ? hrCoopLevel : D3DERR_INVALIDCALL;
    }

    CDirect3DEvents9::OnPresent(m_pDevice);

    // A fog flicker fix for some ATI cards
    D3DMATRIX projMatrix;
    m_pData->GetTransform(D3DTS_PROJECTION, &projMatrix);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &projMatrix);

    TIMING_GRAPH("Present");
    HRESULT hr = CDirect3DEvents9::PresentGuarded(m_pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    TIMING_GRAPH("PostPresent");
    return hr;
}

HRESULT CProxyDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
    return m_pDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT CProxyDirect3DDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
    return m_pDevice->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT CProxyDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
    return m_pDevice->SetDialogBoxMode(bEnableDialogs);
}

VOID CProxyDirect3DDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
    // Validate inputs first
    if (!pRamp)
    {
        WriteDebugEvent("CProxyDirect3DDevice9::SetGammaRamp - Invalid gamma ramp pointer");
        return;
    }

    // Validate swap chain index
    if (iSwapChain >= GetNumberOfSwapChains())
    {
        WriteDebugEvent(SString("CProxyDirect3DDevice9::SetGammaRamp - Invalid swap chain index: %d", iSwapChain));
        return;
    }

    // Get current display mode state - use a timeout to avoid race conditions during mode transitions
    CVideoModeManagerInterface* pVideoModeManager = GetVideoModeManager();
    if (!pVideoModeManager)
    {
        // Fallback to original behavior if video mode manager is unavailable
        m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
        return;
    }

    bool bIsBorderlessMode = pVideoModeManager->IsDisplayModeWindowed() || pVideoModeManager->IsDisplayModeFullScreenWindow();

    // Store original gamma ramp on first call or mode change
    {
        std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
        if (!g_GammaState.bOriginalGammaStored || g_GammaState.lastSwapChain != iSwapChain || g_GammaState.bLastWasBorderless != bIsBorderlessMode)
        {
            if (!g_GammaState.bOriginalGammaStored)
            {
                // Capture the original gamma only once per session
                g_GammaState.originalGammaRamp = *pRamp;
                g_GammaState.bOriginalGammaStored = true;
                g_GammaState.lastSwapChain = iSwapChain;
            }
            g_GammaState.bLastWasBorderless = bIsBorderlessMode;
        }
    }

    if (bIsBorderlessMode)
    {
        // Apply brightness and contrast adjustment to match fullscreen mode
        D3DGAMMARAMP adjustedRamp = *pRamp;

        // Use lighter gamma correction to prevent darkening effect
        // and optimize brightness boost for better visibility
        const float fGammaCorrection = 1.0f / 1.3f;
        const float fBrightnessBoost = 1.4f;

        const auto convertNormalizedToGammaWord = [](float normalized) -> WORD {
            const float scaled = std::clamp(normalized * 65535.0f, 0.0f, 65535.0f);
            const float rounded = std::floor(scaled + 0.5f);
            return static_cast<WORD>(rounded);
        };

        for (int i = 0; i < 256; i++)
        {
            // Convert to normalized float (0.0 - 1.0)
            float fRed = static_cast<float>(pRamp->red[i]) / 65535.0f;
            float fGreen = static_cast<float>(pRamp->green[i]) / 65535.0f;
            float fBlue = static_cast<float>(pRamp->blue[i]) / 65535.0f;

            // Clamp input values to valid range
            fRed = std::max(0.0f, std::min(1.0f, fRed));
            fGreen = std::max(0.0f, std::min(1.0f, fGreen));
            fBlue = std::max(0.0f, std::min(1.0f, fBlue));

            // Apply gentler gamma correction first
            fRed = powf(fRed, fGammaCorrection);
            fGreen = powf(fGreen, fGammaCorrection);
            fBlue = powf(fBlue, fGammaCorrection);

            // Then apply balanced brightness boost
            fRed = std::min(1.0f, fRed * fBrightnessBoost);
            fGreen = std::min(1.0f, fGreen * fBrightnessBoost);
            fBlue = std::min(1.0f, fBlue * fBrightnessBoost);

            // Convert back to WORD values with proper rounding
            adjustedRamp.red[i] = convertNormalizedToGammaWord(fRed);
            adjustedRamp.green[i] = convertNormalizedToGammaWord(fGreen);
            adjustedRamp.blue[i] = convertNormalizedToGammaWord(fBlue);
        }

        // Set the adjusted gamma ramp
        m_pDevice->SetGammaRamp(iSwapChain, Flags, &adjustedRamp);
    }
    else
    {
        // In exclusive fullscreen mode, use the original gamma ramp
        m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
        
        WriteDebugEvent("Applied original gamma ramp for fullscreen mode");
    }
}

VOID CProxyDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
    return m_pDevice->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT CProxyDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture,
                                             HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateTexture(m_pDevice, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                   IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateVolumeTextureGuarded(m_pDevice, Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                 IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateCubeTextureGuarded(m_pDevice, EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer,
                                                  HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateVertexBuffer(m_pDevice, Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer,
                                                 HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateIndexBuffer(m_pDevice, Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality,
                                                  BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateRenderTargetGuarded(m_pDevice, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality,
                                                         BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateDepthStencilSurfaceGuarded(m_pDevice, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface,
                                             CONST POINT* pDestPoint)
{
    return CDirect3DEvents9::UpdateSurfaceGuarded(m_pDevice, pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT CProxyDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
    return CDirect3DEvents9::UpdateTextureGuarded(m_pDevice, pSourceTexture, pDestinationTexture);
}

HRESULT CProxyDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
    return CDirect3DEvents9::GetRenderTargetDataGuarded(m_pDevice, pRenderTarget, pDestSurface);
}

HRESULT CProxyDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
    return CDirect3DEvents9::GetFrontBufferDataGuarded(m_pDevice, iSwapChain, pDestSurface);
}

HRESULT CProxyDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect,
                                           D3DTEXTUREFILTERTYPE Filter)
{
    return CGraphics::GetSingleton().GetRenderItemManager()->HandleStretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT CProxyDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
    return CDirect3DEvents9::ColorFillGuarded(m_pDevice, pSurface, pRect, color);
}

HRESULT CProxyDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface,
                                                           HANDLE* pSharedHandle)
{
    return CDirect3DEvents9::CreateOffscreenPlainSurfaceGuarded(m_pDevice, Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT CProxyDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
    return CDirect3DEvents9::SetRenderTargetGuarded(m_pDevice, RenderTargetIndex, pRenderTarget);
}

HRESULT CProxyDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
    return m_pDevice->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT CProxyDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
    return CDirect3DEvents9::SetDepthStencilSurfaceGuarded(m_pDevice, pNewZStencil);
}

HRESULT CProxyDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
    return m_pDevice->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT CProxyDirect3DDevice9::BeginScene()
{
    HRESULT hResult;

    // Call the real routine.
    hResult = m_pDevice->BeginScene();
    if (hResult == D3D_OK)
        IncrementGTASceneState();

    // Call our event handler.
    CDirect3DEvents9::OnBeginScene(m_pDevice);

    // Possible fix for missing textures on some chipsets
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    return hResult;
}

HRESULT CProxyDirect3DDevice9::EndScene()
{
    // Call our event handler.
    if (CDirect3DEvents9::OnEndScene(m_pDevice))
    {
        // Call real routine.
        HRESULT hResult = m_pDevice->EndScene();

        CGraphics::GetSingleton().GetRenderItemManager()->SaveReadableDepthBuffer();
        DecrementGTASceneState();
        return hResult;
    }

    DecrementGTASceneState();
    return D3D_OK;
}

HRESULT CProxyDirect3DDevice9::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    // If clearing z buffer, make sure we save it first
    if (Flags & D3DCLEAR_ZBUFFER)
        CGraphics::GetSingleton().GetRenderItemManager()->SaveReadableDepthBuffer();

    return CDirect3DEvents9::ClearGuarded(m_pDevice, Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT CProxyDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
    if (!pMatrix)
    {
        WriteDebugEvent("CProxyDirect3DDevice9::SetTransform - Null matrix pointer");
        return D3DERR_INVALIDCALL;
    }

    // Store the matrix
    m_pData->StoreTransform(State, pMatrix);

    DeviceState.TransformState.Raw(State) = *pMatrix;

    // Call original
    return m_pDevice->SetTransform(State, pMatrix);
}

HRESULT CProxyDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
    return m_pDevice->GetTransform(State, pMatrix);
}

HRESULT CProxyDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
    return m_pDevice->MultiplyTransform(State, pMatrix);
}

HRESULT CProxyDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    // Store matrix in m_Data
    m_pData->StoreViewport(pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height);

    // Call original
    return m_pDevice->SetViewport(pViewport);
}

HRESULT CProxyDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{
    return m_pDevice->GetViewport(pViewport);
}

HRESULT CProxyDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
    if (!pMaterial)
    {
        WriteDebugEvent("CProxyDirect3DDevice9::SetMaterial - Null material pointer");
        return D3DERR_INVALIDCALL;
    }

    DeviceState.Material = *pMaterial;
    return m_pDevice->SetMaterial(pMaterial);
}

HRESULT CProxyDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial)
{
    return m_pDevice->GetMaterial(pMaterial);
}

HRESULT CProxyDirect3DDevice9::SetLight(DWORD Index, CONST D3DLIGHT9* pLight)
{
    if (!pLight)
    {
        WriteDebugEvent("CProxyDirect3DDevice9::SetLight - Null light pointer");
        return D3DERR_INVALIDCALL;
    }

    if (Index < NUMELMS(DeviceState.Lights))
        DeviceState.Lights[Index] = *pLight;
    return m_pDevice->SetLight(Index, pLight);
}

HRESULT CProxyDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
    return m_pDevice->GetLight(Index, pLight);
}

HRESULT CProxyDirect3DDevice9::LightEnable(DWORD Index, BOOL Enable)
{
    if (Index < NUMELMS(DeviceState.LightEnableState))
        DeviceState.LightEnableState[Index].Enable = Enable;
    return m_pDevice->LightEnable(Index, Enable);
}

HRESULT CProxyDirect3DDevice9::GetLightEnable(DWORD Index, BOOL* pEnable)
{
    return m_pDevice->GetLightEnable(Index, pEnable);
}

HRESULT CProxyDirect3DDevice9::SetClipPlane(DWORD Index, CONST float* pPlane)
{
    return m_pDevice->SetClipPlane(Index, pPlane);
}

HRESULT CProxyDirect3DDevice9::GetClipPlane(DWORD Index, float* pPlane)
{
    return m_pDevice->GetClipPlane(Index, pPlane);
}

HRESULT CProxyDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    if (State < NUMELMS(DeviceState.RenderState.Raw))
        DeviceState.RenderState.Raw[State] = Value;
    return m_pDevice->SetRenderState(State, Value);
}

HRESULT CProxyDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
    return m_pDevice->GetRenderState(State, pValue);
}

HRESULT CProxyDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
    return m_pDevice->CreateStateBlock(Type, ppSB);
}

HRESULT CProxyDirect3DDevice9::BeginStateBlock()
{
    return m_pDevice->BeginStateBlock();
}

HRESULT CProxyDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
    return m_pDevice->EndStateBlock(ppSB);
}

HRESULT CProxyDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
    return m_pDevice->SetClipStatus(pClipStatus);
}

HRESULT CProxyDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
    return m_pDevice->GetClipStatus(pClipStatus);
}

HRESULT CProxyDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
    return m_pDevice->GetTexture(Stage, ppTexture);
}

HRESULT CProxyDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
    CDirect3DEvents9::CloseActiveShader();
    if (Stage < NUMELMS(DeviceState.TextureState))
        ReplaceInterface(DeviceState.TextureState[Stage].Texture, pTexture);
    return m_pDevice->SetTexture(Stage, CDirect3DEvents9::GetRealTexture(pTexture));
}

HRESULT CProxyDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
    return m_pDevice->GetTextureStageState(Stage, Type, pValue);
}

HRESULT CProxyDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    if (Stage < NUMELMS(DeviceState.StageState))
        if (Type < NUMELMS(DeviceState.StageState[Stage].Raw))
            DeviceState.StageState[Stage].Raw[Type] = Value;
    return m_pDevice->SetTextureStageState(Stage, Type, Value);
}

HRESULT CProxyDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
    return m_pDevice->GetSamplerState(Sampler, Type, pValue);
}

HRESULT CProxyDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    if (Sampler < NUMELMS(DeviceState.SamplerState))
        if (Type < NUMELMS(DeviceState.SamplerState[Sampler].Raw))
            DeviceState.SamplerState[Sampler].Raw[Type] = Value;
    return m_pDevice->SetSamplerState(Sampler, Type, Value);
}

HRESULT CProxyDirect3DDevice9::ValidateDevice(DWORD* pNumPasses)
{
    return m_pDevice->ValidateDevice(pNumPasses);
}

HRESULT CProxyDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
    return m_pDevice->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT CProxyDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
    return m_pDevice->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT CProxyDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
    return m_pDevice->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT CProxyDirect3DDevice9::GetCurrentTexturePalette(UINT* PaletteNumber)
{
    return m_pDevice->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT CProxyDirect3DDevice9::SetScissorRect(CONST RECT* pRect)
{
    return m_pDevice->SetScissorRect(pRect);
}

HRESULT CProxyDirect3DDevice9::GetScissorRect(RECT* pRect)
{
    return m_pDevice->GetScissorRect(pRect);
}

HRESULT CProxyDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
    return m_pDevice->SetSoftwareVertexProcessing(bSoftware);
}

BOOL CProxyDirect3DDevice9::GetSoftwareVertexProcessing()
{
    return m_pDevice->GetSoftwareVertexProcessing();
}

HRESULT CProxyDirect3DDevice9::SetNPatchMode(float nSegments)
{
    return m_pDevice->SetNPatchMode(nSegments);
}

FLOAT CProxyDirect3DDevice9::GetNPatchMode()
{
    return m_pDevice->GetNPatchMode();
}

HRESULT CProxyDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    SetCallType(SCallState::DRAW_PRIMITIVE, {PrimitiveType, static_cast<int>(StartVertex), static_cast<int>(PrimitiveCount)});
    HRESULT hr = CDirect3DEvents9::OnDrawPrimitive(m_pDevice, PrimitiveType, StartVertex, PrimitiveCount);
    SetCallType(SCallState::NONE);
    return hr;
}

HRESULT CProxyDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex,
                                                    UINT primCount)
{
    SetCallType(SCallState::DRAW_INDEXED_PRIMITIVE,
                {PrimitiveType, BaseVertexIndex, static_cast<int>(MinVertexIndex), static_cast<int>(NumVertices), static_cast<int>(startIndex),
                 static_cast<int>(primCount)});
    HRESULT hr = CDirect3DEvents9::OnDrawIndexedPrimitive(m_pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    SetCallType(SCallState::NONE);
    return hr;
}

HRESULT CProxyDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData,
                                               UINT VertexStreamZeroStride)
{
    return CDirect3DEvents9::DrawPrimitiveUPGuarded(m_pDevice, PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT CProxyDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount,
                                                      CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData,
                                                      UINT VertexStreamZeroStride)
{
    return CDirect3DEvents9::DrawIndexedPrimitiveUPGuarded(m_pDevice, PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData,
                                                          IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT CProxyDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer,
                                               IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
    return CDirect3DEvents9::ProcessVerticesGuarded(m_pDevice, SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT CProxyDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
    return CDirect3DEvents9::CreateVertexDeclaration(m_pDevice, pVertexElements, ppDecl);
}

HRESULT CProxyDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
    ReplaceInterface(DeviceState.VertexDeclaration, pDecl);
    return CDirect3DEvents9::SetVertexDeclaration(m_pDevice, pDecl);
}

HRESULT CProxyDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
    return m_pDevice->GetVertexDeclaration(ppDecl);
}

HRESULT CProxyDirect3DDevice9::SetFVF(DWORD FVF)
{
    return m_pDevice->SetFVF(FVF);
}

HRESULT CProxyDirect3DDevice9::GetFVF(DWORD* pFVF)
{
    return m_pDevice->GetFVF(pFVF);
}

HRESULT CProxyDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
    return m_pDevice->CreateVertexShader(pFunction, ppShader);
}

HRESULT CProxyDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
    ReplaceInterface(DeviceState.VertexShader, pShader);
    return m_pDevice->SetVertexShader(pShader);
}

HRESULT CProxyDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
    return m_pDevice->GetVertexShader(ppShader);
}

HRESULT CProxyDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return m_pDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT CProxyDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return m_pDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT CProxyDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return m_pDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT CProxyDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return m_pDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT CProxyDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    return m_pDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT CProxyDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return m_pDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT CProxyDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    if (StreamNumber < NUMELMS(DeviceState.VertexStreams))
    {
        ReplaceInterface(DeviceState.VertexStreams[StreamNumber].StreamData, pStreamData);
        DeviceState.VertexStreams[StreamNumber].StreamOffset = OffsetInBytes;
        DeviceState.VertexStreams[StreamNumber].StreamStride = Stride;
    }
    return m_pDevice->SetStreamSource(StreamNumber, CDirect3DEvents9::GetRealVertexBuffer(pStreamData), OffsetInBytes, Stride);
}

HRESULT CProxyDirect3DDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
    return m_pDevice->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT CProxyDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
    return m_pDevice->SetStreamSourceFreq(StreamNumber, Setting);
}

HRESULT CProxyDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
{
    return m_pDevice->GetStreamSourceFreq(StreamNumber, pSetting);
}

HRESULT CProxyDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
    ReplaceInterface(DeviceState.IndexBufferData, pIndexData);
    return m_pDevice->SetIndices(CDirect3DEvents9::GetRealIndexBuffer(pIndexData));
}

HRESULT CProxyDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
    return m_pDevice->GetIndices(ppIndexData);
}

HRESULT CProxyDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
    return m_pDevice->CreatePixelShader(pFunction, ppShader);
}

HRESULT CProxyDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{
    ReplaceInterface(DeviceState.PixelShader, pShader);
    return m_pDevice->SetPixelShader(pShader);
}

HRESULT CProxyDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
    return m_pDevice->GetPixelShader(ppShader);
}

HRESULT CProxyDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return m_pDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT CProxyDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return m_pDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT CProxyDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return m_pDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT CProxyDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return m_pDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT CProxyDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    return m_pDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT CProxyDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return m_pDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT CProxyDirect3DDevice9::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
    return CDirect3DEvents9::DrawRectPatchGuarded(m_pDevice, Handle, pNumSegs, pRectPatchInfo);
}

HRESULT CProxyDirect3DDevice9::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
    return CDirect3DEvents9::DrawTriPatchGuarded(m_pDevice, Handle, pNumSegs, pTriPatchInfo);
}

HRESULT CProxyDirect3DDevice9::DeletePatch(UINT Handle)
{
    return m_pDevice->DeletePatch(Handle);
}

HRESULT CProxyDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
    return m_pDevice->CreateQuery(Type, ppQuery);
}

void CProxyDirect3DDevice9::ReleaseCachedResources()
{
    for (uint i = 0; i < NUMELMS(DeviceState.TextureState); ++i)
    {
        ReleaseInterface(DeviceState.TextureState[i].Texture);
    }

    for (uint i = 0; i < NUMELMS(DeviceState.VertexStreams); ++i)
    {
        ReleaseInterface(DeviceState.VertexStreams[i].StreamData);
        DeviceState.VertexStreams[i].StreamOffset = 0;
        DeviceState.VertexStreams[i].StreamStride = 0;
    }

    ReleaseInterface(DeviceState.VertexDeclaration);
    ReleaseInterface(DeviceState.VertexShader);
    ReleaseInterface(DeviceState.PixelShader);
    ReleaseInterface(DeviceState.IndexBufferData);
    ReleaseInterface(DeviceState.MainSceneState.DepthBuffer);
}

// For debugging
void CProxyDirect3DDevice9::SetCallType(SCallState::eD3DCallType callType, std::initializer_list<int> args)
{
    DeviceState.CallState.callType = callType;

    const uint maxArgs = static_cast<uint>(NUMELMS(DeviceState.CallState.args));
    const uint clampedCount = std::min<uint>(static_cast<uint>(args.size()), maxArgs);
    DeviceState.CallState.uiNumArgs = clampedCount;

    uint index = 0;
    for (int value : args)
    {
        if (index >= maxArgs)
            break;
        DeviceState.CallState.args[index++] = value;
    }

    for (; index < maxArgs; ++index)
    {
        DeviceState.CallState.args[index] = 0;
    }
}

namespace
{
uint64_t RegisterProxyDevice(CProxyDirect3DDevice9* instance)
{
    std::lock_guard<std::mutex> guard(g_proxyDeviceMutex);
    const uint64_t registrationId = g_proxyRegistrationCounter.fetch_add(1, std::memory_order_relaxed) + 1;
    g_pProxyDevice = instance;
    g_activeProxyRegistrationId = registrationId;
    {
        std::lock_guard<std::mutex> stateGuard(g_deviceStateMutex);
        g_pDeviceState = instance ? &instance->DeviceState : nullptr;
    }
    return registrationId;
}

bool UnregisterProxyDevice(CProxyDirect3DDevice9* instance, uint64_t registrationId)
{
    std::lock_guard<std::mutex> guard(g_proxyDeviceMutex);
    if (g_pProxyDevice != instance)
        return false;

    if (g_activeProxyRegistrationId != registrationId)
        return false;

    g_pProxyDevice = nullptr;
    g_activeProxyRegistrationId = 0;
    {
        std::lock_guard<std::mutex> stateGuard(g_deviceStateMutex);
        g_pDeviceState = nullptr;
    }
    return true;
}
}        // namespace

CProxyDirect3DDevice9* AcquireActiveProxyDevice()
{
    std::lock_guard<std::mutex> guard(g_proxyDeviceMutex);
    if (g_pProxyDevice)
        g_pProxyDevice->AddRef();
    return g_pProxyDevice;
}

void ReleaseActiveProxyDevice(CProxyDirect3DDevice9* pProxyDevice)
{
    if (pProxyDevice)
        pProxyDevice->Release();
}

bool BeginSceneWithoutProxy(IDirect3DDevice9* pDevice, ESceneOwner owner)
{
    if (!pDevice || owner == ESceneOwner::None)
        return false;

    const HRESULT hr = pDevice->BeginScene();
    if (FAILED(hr))
    {
        WriteDebugEvent(SString("BeginSceneWithoutProxy failed: %08x", hr));
        return false;
    }

    if (owner == ESceneOwner::GTA)
    {
        IncrementGTASceneState();
    }
    else if (owner == ESceneOwner::MTA)
    {
        g_bInMTAScene.store(true, std::memory_order_release);
    }

    return true;
}

bool EndSceneWithoutProxy(IDirect3DDevice9* pDevice, ESceneOwner owner)
{
    if (!pDevice || owner == ESceneOwner::None)
        return false;

    const HRESULT hr = pDevice->EndScene();
    if (FAILED(hr))
    {
        WriteDebugEvent(SString("EndSceneWithoutProxy failed: %08x", hr));
        return false;
    }

    if (owner == ESceneOwner::GTA)
    {
        DecrementGTASceneState();
    }
    else if (owner == ESceneOwner::MTA)
    {
        g_bInMTAScene.store(false, std::memory_order_release);
    }

    return true;
}

CScopedActiveProxyDevice::CScopedActiveProxyDevice()
    : m_pProxy(AcquireActiveProxyDevice())
{
}

CScopedActiveProxyDevice::~CScopedActiveProxyDevice()
{
    ReleaseActiveProxyDevice(m_pProxy);
    m_pProxy = nullptr;
}

