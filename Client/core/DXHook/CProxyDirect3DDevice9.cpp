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
#include "CProxyComHelpers.h"
#include "ComPtrValidation.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>
#include <thread>
#include <core/D3DProxyDeviceGuids.h>
#include <game/CSettings.h>

struct CProxyDirect3DDevice9;
extern std::atomic<bool> g_bInMTAScene;

void ApplyBorderlessColorCorrection(CProxyDirect3DDevice9* proxyDevice, const D3DPRESENT_PARAMETERS& presentationParameters);

namespace
{
    // SEH filter for pDevice->Reset(). Mirrors FilterException in CDirect3DEvents9.cpp.
    // Stores the exception code for OnCrashAverted ID encoding and flags hardware
    // faults so callers can break out of retry loops that won't recover.
    uint uiLastResetExceptionCode = 0;
    bool bResetHardwareFault = false;

    int FilterResetException(uint exceptionCode)
    {
        uiLastResetExceptionCode = exceptionCode;
        bResetHardwareFault = false;

        if (exceptionCode == EXCEPTION_ACCESS_VIOLATION)
            return EXCEPTION_EXECUTE_HANDLER;
        if (exceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION || exceptionCode == EXCEPTION_PRIV_INSTRUCTION)
        {
            bResetHardwareFault = true;
            char buf[80];
            _snprintf_s(buf, _countof(buf), _TRUNCATE, "FilterResetException: caught instruction fault %08x", exceptionCode);
            WriteDebugEvent(buf);
            return EXCEPTION_EXECUTE_HANDLER;
        }
        if (exceptionCode == EXCEPTION_IN_PAGE_ERROR)
        {
            bResetHardwareFault = true;
            WriteDebugEvent("FilterResetException: caught in-page error");
            return EXCEPTION_EXECUTE_HANDLER;
        }
        if (exceptionCode == 0xE06D7363)            // Microsoft C++ exception
        {
            WriteDebugEvent("FilterResetException: caught Microsoft C++ exception");
            return EXCEPTION_EXECUTE_HANDLER;
        }
        return EXCEPTION_CONTINUE_SEARCH;
    }
}  // unnamed namespace

namespace BorderlessGamma
{
    const float kGammaMin = 0.5f;
    const float kGammaMax = 2.0f;
    const float kBrightnessMin = 0.5f;
    const float kBrightnessMax = 2.0f;
    const float kContrastMin = 0.5f;
    const float kContrastMax = 2.0f;
    const float kSaturationMin = 0.5f;
    const float kSaturationMax = 2.0f;

    // Cache CVAR-derived values to avoid repeated string lookups on every frame.
    struct CachedSettings
    {
        std::mutex                           updateMutex;
        std::atomic<int>                     revision;
        std::atomic<const CClientVariables*> source;
        std::atomic<float>                   gammaPower;
        std::atomic<float>                   brightnessScale;
        std::atomic<float>                   contrastScale;
        std::atomic<float>                   saturationScale;
        std::atomic<bool>                    gammaEnabled;
        std::atomic<bool>                    brightnessEnabled;
        std::atomic<bool>                    contrastEnabled;
        std::atomic<bool>                    saturationEnabled;
        std::atomic<bool>                    applyWindowed;
        std::atomic<bool>                    applyFullscreen;

        CachedSettings()
            : revision(-1),
              source(nullptr),
              gammaPower(1.0f),
              brightnessScale(1.0f),
              contrastScale(1.0f),
              saturationScale(1.0f),
              gammaEnabled(false),
              brightnessEnabled(false),
              contrastEnabled(false),
              saturationEnabled(false),
              applyWindowed(false),
              applyFullscreen(false)
        {
        }
    };

    static CachedSettings g_cachedSettings;

    void RefreshCacheIfNeeded()
    {
        CClientVariables*       cvars = CClientVariables::GetSingletonPtr();
        const CClientVariables* currentSource = cvars;
        const int               currentRevision = cvars ? cvars->GetRevision() : -1;

        const CClientVariables* cachedSource = g_cachedSettings.source.load(std::memory_order_relaxed);
        const int               cachedRevision = g_cachedSettings.revision.load(std::memory_order_relaxed);

        if (cachedSource == currentSource && cachedRevision == currentRevision)
            return;

        std::lock_guard<std::mutex> guard(g_cachedSettings.updateMutex);

        const CClientVariables* doubleCheckSource = g_cachedSettings.source.load(std::memory_order_relaxed);
        const int               doubleCheckRevision = g_cachedSettings.revision.load(std::memory_order_relaxed);
        if (doubleCheckSource == currentSource && doubleCheckRevision == currentRevision)
            return;

        float gammaPower = 1.0f;
        float brightnessScale = 1.0f;
        float contrastScale = 1.0f;
        float saturationScale = 1.0f;
        bool  gammaEnabled = false;
        bool  brightnessEnabled = false;
        bool  contrastEnabled = false;
        bool  saturationEnabled = false;
        bool  applyWindowed = false;
        bool  applyFullscreen = false;

        if (cvars)
        {
            cvars->Get("borderless_gamma_power", gammaPower);
            cvars->Get("borderless_brightness_scale", brightnessScale);
            cvars->Get("borderless_contrast_scale", contrastScale);
            cvars->Get("borderless_saturation_scale", saturationScale);
            cvars->Get("borderless_gamma_enabled", gammaEnabled);
            cvars->Get("borderless_brightness_enabled", brightnessEnabled);
            cvars->Get("borderless_contrast_enabled", contrastEnabled);
            cvars->Get("borderless_saturation_enabled", saturationEnabled);
            cvars->Get("borderless_apply_windowed", applyWindowed);
            cvars->Get("borderless_apply_fullscreen", applyFullscreen);

            if (!cvars->Exists("borderless_apply_windowed"))
            {
                bool legacyEnable = false;
                cvars->Get("borderless_enable_srgb", legacyEnable);
                applyWindowed = legacyEnable;
            }

            if (!std::isfinite(gammaPower))
                gammaPower = 1.0f;
            if (!std::isfinite(brightnessScale))
                brightnessScale = 1.0f;
            if (!std::isfinite(contrastScale))
                contrastScale = 1.0f;
            if (!std::isfinite(saturationScale))
                saturationScale = 1.0f;

            gammaPower = std::clamp(gammaPower, kGammaMin, kGammaMax);
            brightnessScale = std::clamp(brightnessScale, kBrightnessMin, kBrightnessMax);
            contrastScale = std::clamp(contrastScale, kContrastMin, kContrastMax);
            saturationScale = std::clamp(saturationScale, kSaturationMin, kSaturationMax);
        }

        g_cachedSettings.gammaPower.store(gammaPower, std::memory_order_relaxed);
        g_cachedSettings.brightnessScale.store(brightnessScale, std::memory_order_relaxed);
        g_cachedSettings.contrastScale.store(contrastScale, std::memory_order_relaxed);
        g_cachedSettings.saturationScale.store(saturationScale, std::memory_order_relaxed);
        g_cachedSettings.gammaEnabled.store(gammaEnabled, std::memory_order_relaxed);
        g_cachedSettings.brightnessEnabled.store(brightnessEnabled, std::memory_order_relaxed);
        g_cachedSettings.contrastEnabled.store(contrastEnabled, std::memory_order_relaxed);
        g_cachedSettings.saturationEnabled.store(saturationEnabled, std::memory_order_relaxed);
        g_cachedSettings.applyWindowed.store(applyWindowed, std::memory_order_relaxed);
        g_cachedSettings.applyFullscreen.store(applyFullscreen, std::memory_order_relaxed);
        g_cachedSettings.source.store(currentSource, std::memory_order_relaxed);
        g_cachedSettings.revision.store(currentRevision, std::memory_order_release);
    }

    void FetchSettings(float& gammaPower, float& brightnessScale, float& contrastScale, float& saturationScale, bool& applyWindowed, bool& applyFullscreen)
    {
        RefreshCacheIfNeeded();

        const float cachedGammaPower = g_cachedSettings.gammaPower.load(std::memory_order_acquire);
        const float cachedBrightness = g_cachedSettings.brightnessScale.load(std::memory_order_acquire);
        const float cachedContrast = g_cachedSettings.contrastScale.load(std::memory_order_acquire);
        const float cachedSaturation = g_cachedSettings.saturationScale.load(std::memory_order_acquire);
        const bool  gammaEnabled = g_cachedSettings.gammaEnabled.load(std::memory_order_acquire);
        const bool  brightnessEnabled = g_cachedSettings.brightnessEnabled.load(std::memory_order_acquire);
        const bool  contrastEnabled = g_cachedSettings.contrastEnabled.load(std::memory_order_acquire);
        const bool  saturationEnabled = g_cachedSettings.saturationEnabled.load(std::memory_order_acquire);

        gammaPower = gammaEnabled ? cachedGammaPower : 1.0f;
        brightnessScale = brightnessEnabled ? cachedBrightness : 1.0f;
        contrastScale = contrastEnabled ? cachedContrast : 1.0f;
        saturationScale = saturationEnabled ? cachedSaturation : 1.0f;

        applyWindowed = g_cachedSettings.applyWindowed.load(std::memory_order_acquire);
        applyFullscreen = g_cachedSettings.applyFullscreen.load(std::memory_order_acquire);
    }

    bool ShouldApplyAdjustments(float gammaPower, float brightnessScale, float contrastScale, float saturationScale)
    {
        constexpr float epsilon = 0.001f;
        return std::fabs(gammaPower - 1.0f) > epsilon || std::fabs(brightnessScale - 1.0f) > epsilon || std::fabs(contrastScale - 1.0f) > epsilon ||
               std::fabs(saturationScale - 1.0f) > epsilon;
    }

}  // namespace BorderlessGamma

using namespace BorderlessGamma;
std::mutex            g_proxyDeviceMutex;
std::mutex            g_gammaStateMutex;
std::mutex            g_deviceStateMutex;
std::atomic<uint64_t> g_proxyRegistrationCounter{0};
std::atomic<uint>     g_gtaSceneActiveCount{0};
uint64_t              g_activeProxyRegistrationId = 0;

std::atomic<bool>                       g_bInGTAScene{false};
CProxyDirect3DDevice9*                  g_pProxyDevice = NULL;
CProxyDirect3DDevice9::SD3DDeviceState* g_pDeviceState = NULL;
SGammaState                             g_GammaState;

CProxyDirect3DDevice9::SMemoryState g_StaticMemoryState;

thread_local bool g_bSuspendDeviceStateCache{false};
thread_local uint g_uiSuspendDepth{0};

bool ShouldUpdateDeviceStateCache()
{
    if (g_bSuspendDeviceStateCache)
        return false;
    if (g_bInGTAScene.load(std::memory_order_acquire))
        return true;
    return !g_bInMTAScene.load(std::memory_order_acquire);
}

void SetSuspendDeviceStateCache(bool bSuspend)
{
    if (bSuspend)
    {
        g_uiSuspendDepth++;
        g_bSuspendDeviceStateCache = true;
    }
    else
    {
        if (g_uiSuspendDepth > 0)
            g_uiSuspendDepth--;
        g_bSuspendDeviceStateCache = (g_uiSuspendDepth > 0);
    }
}

namespace
{
    uint64_t RegisterProxyDevice(CProxyDirect3DDevice9* instance)
    {
        std::lock_guard<std::mutex> guard(g_proxyDeviceMutex);
        const uint64_t              registrationId = g_proxyRegistrationCounter.fetch_add(1, std::memory_order_relaxed) + 1;
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
}  // namespace

void IncrementGTASceneState()
{
    // Lockless atomic increment (called on every BeginScene)
    g_gtaSceneActiveCount.fetch_add(1, std::memory_order_relaxed);
    g_bInGTAScene.store(true, std::memory_order_release);
}

void DecrementGTASceneState()
{
    // Prevent underflow if BeginScene failed but EndScene still called
    uint expected = g_gtaSceneActiveCount.load(std::memory_order_acquire);
    while (expected > 0)
    {
        if (g_gtaSceneActiveCount.compare_exchange_weak(expected, expected - 1, std::memory_order_acq_rel, std::memory_order_acquire))
        {
            g_bInGTAScene.store(expected > 1, std::memory_order_release);
            return;
        }
    }
    g_bInGTAScene.store(false, std::memory_order_release);
}

void ResetGTASceneState()
{
    // Lockless atomic reset
    g_gtaSceneActiveCount.store(0, std::memory_order_relaxed);
    g_bInGTAScene.store(false, std::memory_order_release);
}

// Proxy constructor and destructor.
// Constructor performs heavy initialization; defer global registration until the end to avoid exposing a partially built object.
CProxyDirect3DDevice9::CProxyDirect3DDevice9(IDirect3DDevice9* pDevice)
    : m_pDevice(pDevice),
      m_pData(nullptr),
      m_lRefCount(1),
      m_deviceRefCount(0),
      m_bBeginSceneSuccess(false),
      m_registrationToken(0),
      m_lastTestCooperativeLevelResult(D3D_OK)
{
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
        m_deviceRefCount.fetch_add(1, std::memory_order_relaxed);
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
            bNeedRelease = true;  // GetDirect3D increments reference count
        }

        if (pD3D9)
        {
            D3DADAPTER_IDENTIFIER9 adaptIdent;
            HRESULT                hr = pD3D9->GetAdapterIdentifier(iAdapter, 0, &adaptIdent);

            if (SUCCEEDED(hr))
            {
                // GetAdapterIdentifier succeeded - use adapter info
                int iVideoCardMemoryKBTotal = GetWMIVideoAdapterMemorySize(adaptIdent.VendorId, adaptIdent.DeviceId) / 1024;

                // Just incase, fallback to using texture memory stats
                if (iVideoCardMemoryKBTotal < 16)
                    iVideoCardMemoryKBTotal = m_pDevice->GetAvailableTextureMem() / 1024;

                DeviceState.AdapterState.InstalledMemoryKB = iVideoCardMemoryKBTotal;

                // Get video card name
                DeviceState.AdapterState.Name = adaptIdent.Description;

                // Clipping is required for some graphic configurations - use direct C-string search to avoid heap allocation
                DeviceState.AdapterState.bRequiresClipping = (strstr(adaptIdent.Description, "Intel") != nullptr);
            }
            else
            {
                // GetAdapterIdentifier failed - use defaults
                DeviceState.AdapterState.InstalledMemoryKB = m_pDevice->GetAvailableTextureMem() / 1024;
                DeviceState.AdapterState.Name = "Unknown";
                DeviceState.AdapterState.bRequiresClipping = false;
            }

            // Release D3D9 interface if we obtained it via GetDirect3D
            if (bNeedRelease)
            {
                ReleaseInterface(pD3D9, 8799, "CProxyDirect3DDevice9 ctor GetDirect3D");
            }
        }
        else
        {
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

        // Adapter info collected in DeviceState
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

    // Give a default value for the streaming memory setting without assuming core lifetime
    if (CClientVariables* cvars = CClientVariables::GetSingletonPtr())
    {
        if (!cvars->Exists("streaming_memory"))
        {
            if (CCore* core = CCore::GetSingletonPtr())
                cvars->Set("streaming_memory", core->GetMaxStreamingMemory());
        }
    }

    // Note: Pass raw device (m_pDevice), NOT proxy (this), to CGraphics.
    //
    // CGraphics::m_pDevice is used by CloseActiveShader() to null vertex/pixel shaders
    // after effect passes complete. If we pass the proxy here, those SetVertexShader(nullptr)
    // and SetPixelShader(nullptr) calls go through the proxy's ShouldUpdateDeviceStateCache()
    // gating, which can prevent cache updates during MTA scenes.
    //
    // This causes a cache vs. real device desync: the cache thinks shaders are still set,
    // but the real device has them nulled. When GTA's state block is restored via
    // LeavingMTARenderZone(), the SAVE_RENDERSTATE_AND_SET macro reads stale cache values,
    // leading to catastrophic graphics corruption (pink/blue world, missing textures).
    //
    // By passing the raw device, CloseActiveShader's shader nulling bypasses the proxy entirely.
    CDirect3DEvents9::OnDirect3DDeviceCreate(m_pDevice);

    registrationGuard.Dismiss();
    deviceGuard.Dismiss();
}

CProxyDirect3DDevice9::~CProxyDirect3DDevice9()
{
    const bool bWasRegistered = UnregisterProxyDevice(this, m_registrationToken);

    bool         bRestoreGamma = false;
    UINT         lastSwapChain = 0;
    D3DGAMMARAMP originalGammaRamp{};

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
            g_GammaState.originalGammaRamp = {};
        }
    }

    if (bRestoreGamma && m_pDevice)
    {
        IDirect3DDevice9* pDevice = m_pDevice;
        pDevice->AddRef();
        pDevice->SetGammaRamp(lastSwapChain, 0, &originalGammaRamp);
        pDevice->Release();
    }

    // Release any cached COM references we held onto for diagnostics
    ReleaseCachedResources();

    // Release our reference(s) to the wrapped device
    LONG remainingRefs = m_deviceRefCount.exchange(0, std::memory_order_acq_rel);
    if (remainingRefs < 0)
    {
        remainingRefs = 0;
    }

    while (remainingRefs-- > 0)
    {
        m_pDevice->Release();
    }
    m_pDevice = nullptr;
}

/*** IUnknown methods ***/
HRESULT CProxyDirect3DDevice9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (!ppvObj)
        return E_POINTER;

    *ppvObj = nullptr;

    // Looking for me? (Proxy marker)
    if (riid == CProxyDirect3DDevice9_GUID)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }

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
    // Keep proxy and underlying device lifetimes aligned
    LONG lNewRefCount = m_lRefCount.fetch_add(1, std::memory_order_relaxed) + 1;
    if (m_pDevice)
    {
        m_pDevice->AddRef();
        m_deviceRefCount.fetch_add(1, std::memory_order_relaxed);
    }
    return static_cast<ULONG>(lNewRefCount);
}

ULONG CProxyDirect3DDevice9::Release()
{
    LONG lNewRefCount = m_lRefCount.fetch_sub(1, std::memory_order_acq_rel) - 1;

    // Handle underflow: normalize to 0 and continue to destruction check
    if (lNewRefCount < 0)
    {
        m_lRefCount.store(0, std::memory_order_release);
        lNewRefCount = 0;
    }

    // Release one device refcount (whether underflow or normal path)
    if (m_pDevice)
    {
        const LONG prevDeviceRefs = m_deviceRefCount.fetch_sub(1, std::memory_order_relaxed);
        if (prevDeviceRefs > 0)
        {
            m_pDevice->Release();
        }
        else
        {
            m_deviceRefCount.store(0, std::memory_order_relaxed);
        }
    }

    // Destroy proxy when refcount reaches zero
    if (lNewRefCount == 0)
    {
        CDirect3DEvents9::OnDirect3DDeviceDestroy(this);
        delete this;
        return 0;
    }

    return static_cast<ULONG>(lNewRefCount);
}

/*** IDirect3DDevice9 methods ***/
HRESULT CProxyDirect3DDevice9::TestCooperativeLevel()
{
    HRESULT hResult = m_pDevice->TestCooperativeLevel();

    if (hResult != m_lastTestCooperativeLevelResult)
    {
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
        hResult = D3DERR_DEVICELOST;
        __try
        {
            hResult = pDevice->Reset(pPresentationParameters);
        }
        __except (FilterResetException(GetExceptionCode()))
        {
            CCore::GetSingleton().OnCrashAverted((uiLastResetExceptionCode & 0xFFFF) + 24 * 1000000);
            hResult = D3DERR_DEVICELOST;
            // Instruction faults and paging errors won't clear on retry.
            if (bResetHardwareFault)
                break;
        }
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
    HRESULT hResult = D3DERR_DEVICELOST;
    __try
    {
        hResult = pDevice->Reset(pPresentationParameters);
    }
    __except (FilterResetException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastResetExceptionCode & 0xFFFF) + 23 * 1000000);
        hResult = D3DERR_DEVICELOST;
    }

    if (SUCCEEDED(hResult))
    {
        return hResult;
    }
    else if (FAILED(hResult))
    {
        // Handle failure of initial reset device call
        g_pCore->LogEvent(7124, "Direct3D", "Direct3DDevice9::Reset", SString("Fail0:%08x", hResult));

        // Try reset device again
        hResult = ResetDeviceInsist(5, 1000, pDevice, pPresentationParameters);

        // Handle retry result
        if (FAILED(hResult))
        {
            // Record problem
            g_pCore->LogEvent(7124, "Direct3D", "Direct3DDevice9::Reset", SString("Fail1:%08x", hResult));

            // Try with original presentation parameters
            *pPresentationParameters = presentationParametersOrig;
            hResult = ResetDeviceInsist(5, 1000, pDevice, pPresentationParameters);

            if (FAILED(hResult))
            {
                // Record problem
                g_pCore->LogEvent(7124, "Direct3D", "Direct3DDevice9::Reset", SString("Fail2:%08x", hResult));

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

            std::this_thread::yield();
        }
    }

    pEventQuery->Release();
    return bCompleted;
}

HRESULT CProxyDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    // Validate input parameters
    if (!pPresentationParameters)
    {
        return D3DERR_INVALIDCALL;
    }

    // Check cooperative level before attempting reset
    HRESULT hCoopLevel = m_pDevice->TestCooperativeLevel();
    if (hCoopLevel == D3DERR_DEVICELOST)
    {
        // The caller (e.g. RW's _rwD3D9TestState) may have already released GPU
        // resources before calling Reset. Run MTA invalidation so we don't hold
        // stale pointers to surfaces/textures that were freed on the RW side.
        CDirect3DEvents9::OnInvalidate(m_pDevice);
        return hCoopLevel;
    }
    else if (hCoopLevel != D3DERR_DEVICENOTRESET && hCoopLevel != D3D_OK)
    {
        return hCoopLevel;
    }

    // Preserve existing gamma snapshot so we can restore it if the reset fails.
    // Placed after cooperative level check so gamma is untouched on early returns.
    SGammaState previousGammaState;
    bool        gammaStateCleared = false;
    {
        std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
        previousGammaState = g_GammaState;
        g_GammaState = SGammaState();
        gammaStateCleared = true;
    }

    // Save presentation parameters
    D3DPRESENT_PARAMETERS presentationParametersOrig = *pPresentationParameters;

    CVideoModeManagerInterface* pVideoModeManager = GetVideoModeManager();
    if (pVideoModeManager)
    {
        pVideoModeManager->PreReset(pPresentationParameters);
    }

    HRESULT hResult;

    // Call our event handler.
    CDirect3DEvents9::OnInvalidate(m_pDevice);

    // Unbind all device state before releasing cached resources.
    // D3D9 keeps internal refs to bound textures, streams, shaders, etc. If GTA SA
    // freed any of them before Reset, drivers may crash iterating stale entries.
    {
        IDirect3DDevice9* const pDevice = m_pDevice;

        for (DWORD i = 0; i < NUMELMS(DeviceState.TextureState); ++i)
            pDevice->SetTexture(i, nullptr);

        for (DWORD i = 0; i < NUMELMS(DeviceState.VertexStreams); ++i)
            pDevice->SetStreamSource(i, nullptr, 0, 0);

        for (DWORD i = 1; i < 4; ++i)
            pDevice->SetRenderTarget(i, nullptr);

        pDevice->SetIndices(nullptr);
        pDevice->SetVertexShader(nullptr);
        pDevice->SetPixelShader(nullptr);
        pDevice->SetVertexDeclaration(nullptr);
        pDevice->SetDepthStencilSurface(nullptr);
    }

    // Release cached state so lingering references can't block Reset on default-pool resources
    ReleaseCachedResources();

    // Give GPU driver time to complete any pending operations. We keep the light Sleep as a
    // conservative fallback but first rely on an event query, as recommended in Microsoft's GPU
    // synchronization guidance, to ensure outstanding commands finish.
    WaitForGpuIdle(m_pDevice);

    Sleep(1);

    // The device may have gone from DEVICENOTRESET to DEVICELOST during cleanup.
    // Some drivers crash if Reset is called on a fully lost device.
    if (const HRESULT hRecheck = m_pDevice->TestCooperativeLevel(); hRecheck == D3DERR_DEVICELOST)
    {
        if (gammaStateCleared)
        {
            std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
            g_GammaState = previousGammaState;
        }
        return D3DERR_DEVICELOST;
    }

    // Call the real reset routine.
    hResult = DoResetDevice(m_pDevice, pPresentationParameters, presentationParametersOrig);

    if (SUCCEEDED(hResult))
    {
        // Store actual present parameters used
        IDirect3DSwapChain9* pSwapChain = nullptr;
        HRESULT              hrSwapChain = m_pDevice->GetSwapChain(0, &pSwapChain);
        if (FAILED(hrSwapChain))
        {
        }
        else if (!pSwapChain)
        {
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
        ReleaseInterface(pSwapChain, 8799, "CProxyDirect3DDevice9::Reset GetSwapChain");

        if (FAILED(hrCreationParams))
        {
        }

        // Only perform post-reset operations on successful reset
        g_pCore->LogEvent(7123, "Direct3D", "Direct3DDevice9::Reset", "Success");
        if (pVideoModeManager)
        {
            pVideoModeManager->PostReset(pPresentationParameters);
        }

        ApplyBorderlessColorCorrection(this, *pPresentationParameters);

        // Update our data. Some windowed resets use 0x0 backbuffer sizes, so query the device.
        const DWORD  prevViewportWidth = m_pData->GetViewportWidth();
        const DWORD  prevViewportHeight = m_pData->GetViewportHeight();
        D3DVIEWPORT9 viewport = {};
        bool         haveViewport = false;
        if (SUCCEEDED(m_pDevice->GetViewport(&viewport)) && viewport.Width > 0 && viewport.Height > 0)
        {
            haveViewport = true;
            m_pData->StoreViewport(viewport.X, viewport.Y, viewport.Width, viewport.Height);
        }
        else if (pPresentationParameters->BackBufferWidth > 0 && pPresentationParameters->BackBufferHeight > 0)
        {
            m_pData->StoreViewport(0, 0, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
            haveViewport = true;
        }
        else
        {
            IDirect3DSurface9* pBackBuffer = nullptr;
            if (SUCCEEDED(m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer)) && pBackBuffer)
            {
                D3DSURFACE_DESC backBufferDesc = {};
                if (SUCCEEDED(pBackBuffer->GetDesc(&backBufferDesc)) && backBufferDesc.Width > 0 && backBufferDesc.Height > 0)
                {
                    m_pData->StoreViewport(0, 0, backBufferDesc.Width, backBufferDesc.Height);
                    haveViewport = true;
                }
                pBackBuffer->Release();
            }
        }

        if (!haveViewport && prevViewportWidth > 0 && prevViewportHeight > 0)
        {
            m_pData->StoreViewport(0, 0, prevViewportWidth, prevViewportHeight);
        }
        else if (!haveViewport)
        {
            CGraphics::GetSingleton().MarkViewportRefreshPending();
        }

        // Ensure scene state is properly restored
        // Call our event handler.
        CDirect3DEvents9::OnRestore(m_pDevice);

        // Additional sync point for GPU driver
        if (BeginSceneWithoutProxy(m_pDevice, ESceneOwner::MTA))
        {
            EndSceneWithoutProxy(m_pDevice, ESceneOwner::MTA);
        }
    }
    else
    {
        if (gammaStateCleared)
        {
            std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
            g_GammaState = previousGammaState;
        }
    }

    return hResult;
}

HRESULT CProxyDirect3DDevice9::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    static thread_local bool s_inPresent = false;
    if (s_inPresent)
    {
        return CDirect3DEvents9::PresentGuarded(m_pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    }

    struct PresentGuard
    {
        bool& flag;
        explicit PresentGuard(bool& inFlag) : flag(inFlag) { flag = true; }
        ~PresentGuard() { flag = false; }
    } guard(s_inPresent);

    // Reset frame stat counters - using memset for efficiency
    memset(&DeviceState.FrameStats, 0, sizeof(DeviceState.FrameStats));

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCoopLevel = D3DERR_INVALIDCALL;
    if (!CDirect3DEvents9::IsDeviceOperational(m_pDevice, &bDeviceTemporarilyLost, &hrCoopLevel))
    {
        return (hrCoopLevel != D3D_OK) ? hrCoopLevel : D3DERR_INVALIDCALL;
    }

    CDirect3DEvents9::OnPresent(m_pDevice, static_cast<IDirect3DDevice9*>(this));

    // A fog flicker fix for some ATI cards
    const D3DMATRIX* pCachedProjection = m_pData->GetTransformPtr(D3DTS_PROJECTION);
    if (pCachedProjection)
    {
        m_pDevice->SetTransform(D3DTS_PROJECTION, pCachedProjection);
    }

    TIMING_GRAPH("Present");
    HRESULT hr = CDirect3DEvents9::PresentGuarded(m_pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    TIMING_GRAPH("PostPresent");
    return hr;
}

HRESULT CProxyDirect3DDevice9::PresentWithoutProxy(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    return CDirect3DEvents9::PresentGuarded(m_pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
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
        return;
    }

    // Validate swap chain index
    if (iSwapChain >= GetNumberOfSwapChains())
    {
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

    // Fast-path: check cached state before expensive settings fetch
    {
        std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
        if (g_GammaState.bOriginalGammaStored && g_GammaState.bLastWasBorderless == bIsBorderlessMode && g_GammaState.lastSwapChain == iSwapChain)
        {
            // State hasn't changed, apply ramp directly
            m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
            return;
        }
    }

    float gammaPower = 1.0f;
    float brightnessScale = 1.0f;
    float contrastScale = 1.0f;
    float saturationScale = 1.0f;
    bool  applyWindowed = true;
    bool  applyFullscreen = false;
    FetchSettings(gammaPower, brightnessScale, contrastScale, saturationScale, applyWindowed, applyFullscreen);

    const bool adjustmentsRequested = bIsBorderlessMode ? applyWindowed : applyFullscreen;

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

    if (adjustmentsRequested)
    {
        // Let the tone-mapping pass handle all presentation adjustments for this mode.
        m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
        return;
    }

    if (bIsBorderlessMode)
    {
        // Windowed/borderless always rely on the tone-mapping pass.
        m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
        return;
    }

    // In exclusive fullscreen mode with no adjustments requested, fall back to the original gamma ramp
    m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void CProxyDirect3DDevice9::ApplyBorderlessPresentationTuning()
{
    if (!m_pDevice)
        return;

    D3DPRESENT_PARAMETERS presentationParameters{};
    bool                  havePresentationParameters = false;
    {
        std::lock_guard<std::mutex> stateLock(g_deviceStateMutex);
        if (g_pDeviceState)
        {
            presentationParameters = g_pDeviceState->CreationState.PresentationParameters;
            havePresentationParameters = true;
        }
    }

    bool bIsBorderlessMode = false;
    if (havePresentationParameters)
    {
        bIsBorderlessMode = (presentationParameters.Windowed != 0);
    }

    if (!havePresentationParameters)
    {
        if (CVideoModeManagerInterface* pVideoModeManager = GetVideoModeManager())
            bIsBorderlessMode = pVideoModeManager->IsDisplayModeWindowed() || pVideoModeManager->IsDisplayModeFullScreenWindow();

        // Provide a best-effort set of presentation parameters so sRGB state refreshes consistently.
        presentationParameters.Windowed = bIsBorderlessMode ? TRUE : FALSE;
    }

    ApplyBorderlessColorCorrection(this, presentationParameters);

    if (bIsBorderlessMode)
    {
        // Borderless tone mapping handles gamma/brightness adjustments when enabled.
        return;
    }

    UINT         targetSwapChain = 0;
    D3DGAMMARAMP baseRamp{};
    bool         haveBaseRamp = false;

    {
        std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
        if (g_GammaState.bOriginalGammaStored)
        {
            baseRamp = g_GammaState.originalGammaRamp;
            targetSwapChain = g_GammaState.lastSwapChain;
            haveBaseRamp = true;
        }
    }

    if (!haveBaseRamp)
    {
        m_pDevice->GetGammaRamp(targetSwapChain, &baseRamp);

        std::lock_guard<std::mutex> gammaLock(g_gammaStateMutex);
        g_GammaState.originalGammaRamp = baseRamp;
        g_GammaState.lastSwapChain = targetSwapChain;
        g_GammaState.bOriginalGammaStored = true;
    }

    D3DGAMMARAMP rampCopy = baseRamp;
    SetGammaRamp(targetSwapChain, 0, &rampCopy);
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
    return CDirect3DEvents9::CreateDepthStencilSurfaceGuarded(m_pDevice, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface,
                                                              pSharedHandle);
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
    if (!pSourceSurface || !pDestSurface)
        return D3DERR_INVALIDCALL;

    static thread_local bool s_inStretchRect = false;
    if (s_inStretchRect)
    {
        return m_pDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
    }

    struct StretchRectGuard
    {
        bool& flag;
        explicit StretchRectGuard(bool& inFlag) : flag(inFlag) { flag = true; }
        ~StretchRectGuard() { flag = false; }
    } guard(s_inStretchRect);

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
    {
        IncrementGTASceneState();
        m_bBeginSceneSuccess.store(true, std::memory_order_release);
    }

    // Call our event handler.
    CDirect3DEvents9::OnBeginScene(m_pDevice);

    // Possible fix for missing textures on some chipsets
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    return hResult;
}

HRESULT CProxyDirect3DDevice9::EndScene()
{
    const bool bShouldDecrement = m_bBeginSceneSuccess.exchange(false, std::memory_order_acq_rel);

    // Call our event handler.
    if (CDirect3DEvents9::OnEndScene(m_pDevice))
    {
        // Call real routine.
        HRESULT hResult = m_pDevice->EndScene();

        CGraphics::GetSingleton().GetRenderItemManager()->SaveReadableDepthBuffer();
        if (bShouldDecrement)
            DecrementGTASceneState();
        return hResult;
    }

    if (bShouldDecrement)
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
        return D3DERR_INVALIDCALL;
    }

    // Store the matrix
    m_pData->StoreTransform(State, pMatrix);

    if (ShouldUpdateDeviceStateCache())
        DeviceState.TransformState.Raw(State) = *pMatrix;

    // Call original
    return m_pDevice->SetTransform(State, pMatrix);
}

HRESULT CProxyDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
    // Use cached data if available to avoid expensive device query
    if (pMatrix)
    {
        const D3DMATRIX* pCached = m_pData->GetTransformPtr(State);
        if (pCached)
        {
            *pMatrix = *pCached;
            return D3D_OK;
        }
    }
    // Fallback to device query for unsupported transform types
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
        return D3DERR_INVALIDCALL;
    }

    // Update cache if material has changed (avoid 68-byte copy)
    if (ShouldUpdateDeviceStateCache())
    {
        if (memcmp(&DeviceState.Material, pMaterial, sizeof(D3DMATERIAL9)) != 0)
            DeviceState.Material = *pMaterial;
    }
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
        return D3DERR_INVALIDCALL;
    }

    // Update cache if light has changed (avoid 104-byte copy)
    if (ShouldUpdateDeviceStateCache())
    {
        if (Index < NUMELMS(DeviceState.Lights) && memcmp(&DeviceState.Lights[Index], pLight, sizeof(D3DLIGHT9)) != 0)
            DeviceState.Lights[Index] = *pLight;
    }
    return m_pDevice->SetLight(Index, pLight);
}

HRESULT CProxyDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
    return m_pDevice->GetLight(Index, pLight);
}

HRESULT CProxyDirect3DDevice9::LightEnable(DWORD Index, BOOL Enable)
{
    if (ShouldUpdateDeviceStateCache())
    {
        if (Index < NUMELMS(DeviceState.LightEnableState))
            DeviceState.LightEnableState[Index].Enable = Enable;
    }
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
    // Update cache for state tracking
    if (ShouldUpdateDeviceStateCache())
    {
        if (State < NUMELMS(DeviceState.RenderState.Raw))
            DeviceState.RenderState.Raw[State] = Value;
    }

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
    CDirect3DEvents9::CloseActiveShader(true, static_cast<IDirect3DDevice9*>(this));
    if (ShouldUpdateDeviceStateCache())
    {
        if (Stage < NUMELMS(DeviceState.TextureState))
        {
            // Fast-path: avoid AddRef/Release if texture hasn't changed
            if (DeviceState.TextureState[Stage].Texture != pTexture)
            {
                ReplaceInterface(DeviceState.TextureState[Stage].Texture, pTexture);
            }
        }
    }
    return m_pDevice->SetTexture(Stage, CDirect3DEvents9::GetRealTexture(pTexture));
}

HRESULT CProxyDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
    return m_pDevice->GetTextureStageState(Stage, Type, pValue);
}

HRESULT CProxyDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    // Update cache for state tracking
    if (ShouldUpdateDeviceStateCache())
    {
        if (Stage < NUMELMS(DeviceState.StageState))
            if (Type < NUMELMS(DeviceState.StageState[Stage].Raw))
                DeviceState.StageState[Stage].Raw[Type] = Value;
    }

    return m_pDevice->SetTextureStageState(Stage, Type, Value);
}

HRESULT CProxyDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
    return m_pDevice->GetSamplerState(Sampler, Type, pValue);
}

HRESULT CProxyDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    // Update cache for state tracking
    if (ShouldUpdateDeviceStateCache())
    {
        if (Sampler < NUMELMS(DeviceState.SamplerState))
            if (Type < NUMELMS(DeviceState.SamplerState[Sampler].Raw))
                DeviceState.SamplerState[Sampler].Raw[Type] = Value;
    }

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
    HRESULT hr = CDirect3DEvents9::OnDrawPrimitive(m_pDevice, static_cast<IDirect3DDevice9*>(this), PrimitiveType, StartVertex, PrimitiveCount);
    SetCallType(SCallState::NONE);
    return hr;
}

HRESULT CProxyDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex,
                                                    UINT primCount)
{
    SetCallType(SCallState::DRAW_INDEXED_PRIMITIVE, {PrimitiveType, BaseVertexIndex, static_cast<int>(MinVertexIndex), static_cast<int>(NumVertices),
                                                     static_cast<int>(startIndex), static_cast<int>(primCount)});
    HRESULT hr = CDirect3DEvents9::OnDrawIndexedPrimitive(m_pDevice, static_cast<IDirect3DDevice9*>(this), PrimitiveType, BaseVertexIndex, MinVertexIndex,
                                                          NumVertices, startIndex, primCount);
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
    return CDirect3DEvents9::DrawIndexedPrimitiveUPGuarded(m_pDevice, PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat,
                                                           pVertexStreamZeroData, VertexStreamZeroStride);
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
    // Avoid validation overhead since declarations come from D3D9 driver
    if (ShouldUpdateDeviceStateCache())
    {
        if (DeviceState.VertexDeclaration != pDecl)
            ReplaceInterface(DeviceState.VertexDeclaration, pDecl);
    }
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
    // Avoid validation overhead since shaders come from D3D9 driver
    if (ShouldUpdateDeviceStateCache())
    {
        if (DeviceState.VertexShader != pShader)
            ReplaceInterface(DeviceState.VertexShader, pShader);
    }
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
    if (StreamNumber < NUMELMS(DeviceState.VertexStreams) && ShouldUpdateDeviceStateCache())
    {
        // Avoid validation overhead since vertex buffers come from D3D9 driver
        if (DeviceState.VertexStreams[StreamNumber].StreamData != pStreamData)
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
    // Avoid validation overhead since index buffers come from D3D9 driver
    if (ShouldUpdateDeviceStateCache())
    {
        if (DeviceState.IndexBufferData != pIndexData)
            ReplaceInterface(DeviceState.IndexBufferData, pIndexData);
    }
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
    // Avoid validation overhead since shaders come from D3D9 driver
    if (ShouldUpdateDeviceStateCache())
    {
        if (DeviceState.PixelShader != pShader)
            ReplaceInterface(DeviceState.PixelShader, pShader);
    }
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
        ReleaseInterface(DeviceState.TextureState[i].Texture, 8799, "ReleaseCachedResources Texture");
    }

    for (uint i = 0; i < NUMELMS(DeviceState.VertexStreams); ++i)
    {
        ReleaseInterface(DeviceState.VertexStreams[i].StreamData, 8799, "ReleaseCachedResources VertexStream");
        DeviceState.VertexStreams[i].StreamOffset = 0;
        DeviceState.VertexStreams[i].StreamStride = 0;
    }

    ReleaseInterface(DeviceState.VertexDeclaration, 8799, "ReleaseCachedResources VertexDecl");
    ReleaseInterface(DeviceState.VertexShader, 8799, "ReleaseCachedResources VertexShader");
    ReleaseInterface(DeviceState.PixelShader, 8799, "ReleaseCachedResources PixelShader");
    ReleaseInterface(DeviceState.IndexBufferData, 8799, "ReleaseCachedResources IndexBuffer");
    ReleaseInterface(DeviceState.MainSceneState.DepthBuffer, 8799, "ReleaseCachedResources DepthBuffer");
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
        if (owner == ESceneOwner::GTA)
        {
            ResetGTASceneState();
        }
        else if (owner == ESceneOwner::MTA)
        {
            g_bInMTAScene.store(false, std::memory_order_release);
        }
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

CScopedActiveProxyDevice::CScopedActiveProxyDevice() : m_pProxy(AcquireActiveProxyDevice())
{
}

CScopedActiveProxyDevice::~CScopedActiveProxyDevice()
{
    ReleaseActiveProxyDevice(m_pProxy);
    m_pProxy = nullptr;
}
