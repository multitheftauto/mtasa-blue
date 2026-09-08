/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CJoystickManager.cpp
 *  PURPOSE:     Joystick related operations
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CPad.h>
#include "XInput.h"
#include <dinputd.h>
#include <atomic>
#include <thread>
#include <vector>

using std::string;

extern IDirectInput8* g_pDirectInput8;

//////////////////////////////////////////////////////////
//
// Helper stuff
//

#ifndef NUMELMS  // in DShow.h
    #define NUMELMS(aa) (sizeof(aa) / sizeof((aa)[0]))
#endif

#define VALID_INDEX_FOR(array, index) (index >= 0 && index < NUMELMS(array))

// How long to wait before retrying a failed joystick detection, and how long a device can go without
// responding before we consider it unplugged
constexpr uint JOYSTICK_RETRY_DELAY_MS = 3000;

// How long to wait for a burst of Windows device-change notifications to go quiet before actually
// re-scanning; a single physical plug/unplug can fire several of these in a row, one per device
// interface
constexpr uint JOYSTICK_DEVICE_CHANGE_SETTLE_MS = 300;

// DirectInput Acquire() can block while Windows is still enumerating USB devices; don't hammer it every frame.
constexpr uint JOYSTICK_ACQUIRE_RETRY_MS = 500;

// Stick/trigger deadzone is a percentage of throw. Saturation 0-100 keeps the old mapping
// (100 = linear, lower = reaches max sooner). Values 101-200 multiply beyond linear so a
// worn stick that never hits the physical extreme can still produce a full GTA axis.
constexpr int JOYSTICK_DEADZONE_MAX = 49;
constexpr int JOYSTICK_SATURATION_MAX = 200;
constexpr int JOYSTICK_DEFAULT_TRIGGER_DEADZONE = 5;

SString GUIDToString(const GUID& g)
{
    return SString("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4],
                   g.Data4[5], g.Data4[6], g.Data4[7]);
}

bool StringToGUID(const char* szGuid, GUID& g)
{
    unsigned int d1 = 0, d2 = 0, d3 = 0, b[8] = {};
    if (!szGuid ||
        sscanf(szGuid, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", &d1, &d2, &d3, &b[0], &b[1], &b[2], &b[3], &b[4], &b[5], &b[6], &b[7]) != 11)
        return false;

    g.Data1 = d1;
    g.Data2 = static_cast<WORD>(d2);
    g.Data3 = static_cast<WORD>(d3);
    for (int i = 0; i < 8; i++)
        g.Data4[i] = static_cast<BYTE>(b[i]);
    return true;
}

struct SParsedControllerId
{
    enum eType
    {
        Auto,
        XInput,
        DirectInput,
    } type = Auto;
    int  iXInputIndex = 0;
    GUID guidInstance{};
    bool bHasGuid = false;
};

SParsedControllerId ParseControllerId(const string& strId)
{
    SParsedControllerId parsed;
    if (strId.empty() || strId == "auto")
        return parsed;

    if (strId.compare(0, 7, "xinput:") == 0)
    {
        parsed.type = SParsedControllerId::XInput;
        parsed.iXInputIndex = Clamp(0, atoi(strId.c_str() + 7), 3);
        return parsed;
    }

    if (strId.compare(0, 7, "dinput:") == 0)
    {
        parsed.type = SParsedControllerId::DirectInput;
        parsed.bHasGuid = StringToGUID(strId.c_str() + 7, parsed.guidInstance);
        return parsed;
    }

    return parsed;
}

SString GetXInputDisplayName(int iIndex, const XINPUT_CAPABILITIES& Capabilities)
{
    const char* subTypeNames[] = {"Unknown", "Gamepad", "Wheel", "Arcade stick", "Flight stick", "Dance pad", "Guitar", "Drum kit"};
    SString     strType;
    if (Capabilities.SubType < NUMELMS(subTypeNames))
        strType = subTypeNames[Capabilities.SubType];
    else
        strType = SString("Subtype %d", Capabilities.SubType);

    return SString(_("%s (controller %d)"), strType.c_str(), iIndex + 1);
}

DEFINE_GUID(GUID_Xbox360Controller, 0x028E045E, 0x0000, 0x0000, 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44);

//////////////////////////////////////////////////////////
//
// Mapping
//
enum eJoy
{
    eJoyX,
    eJoyY,
    eJoyZ,
    eJoyRx,
    eJoyRy,
    eJoyRz,
    eJoyS1,
    eJoyMax
};

enum eDir
{
    eDirNeg,
    eDirPos,
    eDirNegToPos,
    eDirMax
};

enum eStick
{
    eLeftStickX,
    eLeftStickY,
    eRightStickX,
    eRightStickY,
    eAccelerate,
    eBrake,
    eStickMax
};

struct SMappingLine
{
    eJoy   SourceAxisIndex;  // 0 - 7
    eDir   SourceAxisDir;    // 0 - 2
    eStick OutputAxisIndex;  // 0/1 2/3 4 5
    eDir   OutputAxisDir;    // 0 - 1
    bool   bEnabled;
    int    MaxValue;
};

//////////////////////////////////////////////////////////
//
// DeviceInfo
//
struct SInputDeviceInfo
{
    IDirectInputDevice8A* pDevice;
    bool                  bDoneEnumAxes;
    int                   iAxisCount;
    int                   iDeadZone;
    int                   iSaturation;
    int                   iTriggerDeadZone;
    int                   iTriggerSaturation;
    GUID                  guidProduct;
    string                strGuid;
    string                strProductName;

    struct
    {
        bool  bEnabled;
        long  lMax;
        long  lMin;
        DWORD dwType;
        float fAutoDeadZoneSample;
    } axis[7];
};

//////////////////////////////////////////////////////////
//
// Result of a background DirectInput device scan (discovery + axis enumeration). Built up
// entirely on the scan thread from a freshly created device no one else can see yet, then handed
// over as a finished value so the main thread only ever has to copy it into place.
//
struct SJoystickScanResult
{
    IDirectInputDevice8A*                pDevice = nullptr;
    int                                  iAxisCount = 0;
    GUID                                 guidProduct{};
    GUID                                 guidInstance{};
    string                               strProductName;
    std::vector<std::pair<GUID, string>> listedDevices;

    struct
    {
        bool  bEnabled = false;
        long  lMax = 0;
        long  lMin = 0;
        DWORD dwType = 0;
    } axis[7];
};

// Internal state
struct SJoystickState
{
    float rgfAxis[7];     /* axis positions     -1.f to 1.f       */
    DWORD rgdwPOV[4];     /* POV directions                       */
    BYTE  rgbButtons[32]; /* 32 buttons                           */
    BYTE  rgbButtonsWas[32];
    BYTE  povButtonsWas[4];
    BYTE  axisButtonsWas[14];  // Axis as buttons
};

///////////////////////////////////////////////////////////////
//
// CJoystickManager class
//
///////////////////////////////////////////////////////////////
class CJoystickManager : public CJoystickManagerInterface
{
public:
    ZERO_ON_NEW
    CJoystickManager();
    ~CJoystickManager();

    // CJoystickManagerInterface methods
    virtual void OnSetDataFormat(IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a) {}
    virtual void RemoveDevice(IDirectInputDevice8A* pDevice);
    virtual void DoPulse();
    virtual void ApplyAxes(CControllerState& cs, bool bInVehicle);

    // Status
    virtual bool IsJoypadConnected();
    virtual void OnPossibleDeviceChange();

    // Settings
    virtual string                             GetControllerName();
    virtual int                                GetDeadZone();
    virtual int                                GetSaturation();
    virtual int                                GetTriggerDeadZone();
    virtual int                                GetTriggerSaturation();
    virtual void                               SetDeadZone(int iDeadZone);
    virtual void                               SetSaturation(int iSaturation);
    virtual void                               SetTriggerDeadZone(int iDeadZone);
    virtual void                               SetTriggerSaturation(int iSaturation);
    virtual bool                               GetVibrationEnabled();
    virtual void                               SetVibrationEnabled(bool bEnabled);
    virtual string                             GetSelectedControllerId();
    virtual void                               SetSelectedControllerId(const string& strId);
    virtual std::vector<SJoystickDeviceChoice> GetAvailableControllers();
    virtual int                                GetSettingsRevision();
    virtual int                                GetDeviceListRevision();
    virtual void                               SetDefaults();
    virtual bool                               SaveToXML();

    // Binding
    virtual int    GetOutputCount();
    virtual string GetOutputName(int iOutputIndex);
    virtual string GetOutputInputName(int iOutputIndex);
    virtual bool   BindNextUsedAxisToOutput(int iOutputIndex);
    virtual bool   IsAxisBindComplete();
    virtual bool   IsCapturingAxis();
    virtual void   CancelCaptureAxis(bool bClearBinding);

private:
    bool      ReadInputSubsystem(DIJOYSTATE2& js);
    bool      HandleXInputGetState(XINPUT_STATE& XInputState);
    bool      IsXInputDeviceAttached();
    bool      IsJoypadValid();
    void      StartDirectInputScan();
    void      CollectDirectInputScanResult();
    void      StartDirectInputListRefresh();
    void      CollectDirectInputListRefresh();
    void      BumpDeviceListRevision();
    void      ReadCurrentState();
    CXMLNode* GetConfigNode(bool bCreateIfRequired);
    bool      LoadFromXML();
    bool      IsTriggerSourceAxis(int iAxisIndex) const;
    void      ApplyAxisResponse(float& fResult, int iSaturation);
    void      ApplyVibration();
    void      StopVibration();
    void      SyncGtaVibrationPref();
    void      ReleaseDirectInputDevice();
    void      ApplyControllerSelection(bool bReleaseCurrent);
    void      RefreshDirectInputDeviceList();
    int       FindFirstXInputIndex() const;

    bool                                 m_bDoneInit;
    int                                  m_SettingsRevision;
    int                                  m_iDeviceListRevision;
    bool                                 m_bPendingDeviceListRefresh;
    SInputDeviceInfo                     m_DevInfo;
    SJoystickState                       m_JoystickState;
    SMappingLine                         m_currentMapping[10];
    bool                                 m_bUseXInput;
    bool                                 m_bXInputDeviceAttached;
    int                                  m_iXInputUserIndex;
    uint                                 m_uiXInputReattachDelay;
    CElapsedTime                         m_XInputReattachTimer;
    uint                                 m_uiDirectInputReattachDelay;
    CElapsedTime                         m_DirectInputReattachTimer;
    CElapsedTime                         m_PollFailTimer;
    CElapsedTime                         m_AcquireRetryTimer;
    uint                                 m_uiAcquireRetryDelay;
    bool                                 m_bAutoDeadZoneEnabled;
    int                                  m_iAutoDeadZoneCounter;
    bool                                 m_bLoggedNoJoystick;
    string                               m_strSelectedControllerId;
    bool                                 m_bVibrationEnabled;
    bool                                 m_bVibrationWasActive;
    CElapsedTime                         m_VibrationTimer;
    bool                                 m_bPreferredInstanceValid;
    GUID                                 m_PreferredInstanceGuid;
    std::vector<std::pair<GUID, string>> m_ListedDInputDevices;

    // Used during axis binding
    bool           m_bCaptureAxis;
    int            m_iCaptureOutputIndex;
    SJoystickState m_PreBindJoystickState;

    // DirectInput device discovery (including axis enumeration, which used to run on the main
    // thread the moment the device was first polled) runs on a background thread so none of it
    // ever costs a frame; DoPulse only ever picks up a finished scan, it never blocks on one.
    std::thread         m_ScanThread;
    std::atomic<bool>   m_bScanRunning{false};
    std::atomic<bool>   m_bScanReady{false};
    SJoystickScanResult m_ScanResult;

    // Lightweight background refresh of the DirectInput device list for the settings combo.
    std::thread                          m_ListRefreshThread;
    std::atomic<bool>                    m_bListRefreshRunning{false};
    std::atomic<bool>                    m_bListRefreshReady{false};
    std::vector<std::pair<GUID, string>> m_ListRefreshResult;
};

///////////////////////////////////////////////////////////////
//
// CJoystickManager instantiation
//
///////////////////////////////////////////////////////////////
CJoystickManagerInterface* NewJoystickManager()
{
    return new CJoystickManager();
}

// This is nice so there
CJoystickManagerInterface* g_pJoystickManager = NULL;

CJoystickManagerInterface* GetJoystickManager()
{
    if (!g_pJoystickManager)
        g_pJoystickManager = NewJoystickManager();
    return g_pJoystickManager;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager implementation
//
///////////////////////////////////////////////////////////////
CJoystickManager::CJoystickManager()
{
    m_iAutoDeadZoneCounter = 20;
    m_bAutoDeadZoneEnabled = true;
    m_strSelectedControllerId = "auto";
    m_bVibrationEnabled = true;
    m_iXInputUserIndex = 0;
    m_uiAcquireRetryDelay = JOYSTICK_ACQUIRE_RETRY_MS;

    // CVars may not be loaded yet if this is constructed very early; ApplyControllerSelection
    // then just auto-picks the first XInput slot like we used to.
    if (CClientVariables::GetSingletonPtr())
    {
        std::string strDevice;
        if (CVARS_GET("controller_device", strDevice) && !strDevice.empty())
            m_strSelectedControllerId = strDevice;
        CVARS_GET("controller_vibration", m_bVibrationEnabled);
    }

    ApplyControllerSelection(false);
    SetDefaults();
}

CJoystickManager::~CJoystickManager()
{
    StopVibration();

    // Let a scan still in flight finish before the members it writes into go away
    if (m_ScanThread.joinable())
        m_ScanThread.join();
    if (m_ListRefreshThread.joinable())
        m_ListRefreshThread.join();
}

///////////////////////////////////////////////////////////////
//
// Background joystick discovery
//
// Runs entirely off the CJoystickManager instance: g_pDirectInput8 is otherwise only ever
// touched from the main thread, and CJoystickManager guarantees at most one scan is in flight
// at a time (see m_bScanRunning), so this never races another EnumDevices/CreateDevice call.
// The device it creates is brand new and not visible to anything else until the main thread
// adopts it in CollectDirectInputScanResult(), so there's nothing here for the main thread to
// race with either.
//
///////////////////////////////////////////////////////////////
namespace
{
    struct SJoystickEnumContext
    {
        bool                                 bPreferredValid = false;
        GUID                                 preferredGuidInstance{};
        IDirectInputDevice8A*                pDevice = nullptr;
        std::vector<std::pair<GUID, string>> listedDevices;
    };

    BOOL CALLBACK EnumJoysticksCallbackAsync(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
    {
        auto* pCtx = static_cast<SJoystickEnumContext*>(pContext);

        pCtx->listedDevices.push_back({pdidInstance->guidInstance, pdidInstance->tszProductName});

        // If the user picked a specific DirectInput pad, skip everything else. Otherwise take
        // the first device (Windows' preferred joystick when that GUID was supplied).
        if (pCtx->pDevice)
            return DIENUM_CONTINUE;

        if (pCtx->bPreferredValid && !IsEqualGUID(pdidInstance->guidInstance, pCtx->preferredGuidInstance))
            return DIENUM_CONTINUE;

        // Obtain an interface to the enumerated Joystick. (Maybe the user unplugged it while we
        // were in the middle of enumerating it, in which case this just fails and we move on.)
        if (FAILED(g_pDirectInput8->CreateDevice(pdidInstance->guidInstance, &pCtx->pDevice, NULL)))
            return DIENUM_CONTINUE;

        return DIENUM_CONTINUE;
    }

    BOOL CALLBACK EnumJoysticksListCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
    {
        auto* pList = static_cast<std::vector<std::pair<GUID, string>>*>(pContext);
        pList->push_back({pdidInstance->guidInstance, pdidInstance->tszProductName});
        return DIENUM_CONTINUE;
    }

    // Same axis setup CJoystickManager used to do synchronously the moment a device's first Poll()
    // succeeded (range/deadzone/saturation properties, axis index mapping); moved here so it runs
    // before the device is ever handed to the main thread instead of costing it a frame later.
    BOOL CALLBACK EnumAxesCallbackAsync(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext)
    {
        auto* pResult = static_cast<SJoystickScanResult*>(pContext);

        if (!(pdidoi->dwType & DIDFT_AXIS))
            return DIENUM_CONTINUE;

        DIPROPRANGE range;
        range.diph.dwSize = sizeof(DIPROPRANGE);
        range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        range.diph.dwHow = DIPH_BYID;
        range.diph.dwObj = pdidoi->dwType;
        range.lMin = -1000;
        range.lMax = +1000;

        if (FAILED(pResult->pDevice->SetProperty(DIPROP_RANGE, &range.diph)))
            return DIENUM_CONTINUE;
        if (FAILED(pResult->pDevice->GetProperty(DIPROP_RANGE, &range.diph)))
            return DIENUM_CONTINUE;

        // Remove Deadzone and Saturation
        DIPROPDWORD dead, sat;
        dead.diph.dwSize = sizeof dead;
        dead.diph.dwHeaderSize = sizeof dead.diph;
        dead.diph.dwHow = DIPH_BYID;
        dead.diph.dwObj = pdidoi->dwType;
        dead.dwData = 0;  // No Deadzone

        sat = dead;
        sat.dwData = 10000;  // No Saturation

        pResult->pDevice->SetProperty(DIPROP_DEADZONE, &dead.diph);
        pResult->pDevice->SetProperty(DIPROP_SATURATION, &sat.diph);

        // Figure out the axis index
        int axisIndex = -1;
        if (pdidoi->guidType == GUID_XAxis)
            axisIndex = eJoyX;
        else if (pdidoi->guidType == GUID_YAxis)
            axisIndex = eJoyY;
        else if (pdidoi->guidType == GUID_ZAxis)
            axisIndex = eJoyZ;
        else if (pdidoi->guidType == GUID_RxAxis)
            axisIndex = eJoyRx;
        else if (pdidoi->guidType == GUID_RyAxis)
            axisIndex = eJoyRy;
        else if (pdidoi->guidType == GUID_RzAxis)
            axisIndex = eJoyRz;
        else if (pdidoi->guidType == GUID_Slider)
            axisIndex = eJoyS1;

        if (axisIndex >= 0 && axisIndex < NUMELMS(pResult->axis) && range.lMin < range.lMax && !pResult->axis[axisIndex].bEnabled)
        {
            pResult->axis[axisIndex].lMin = range.lMin;
            pResult->axis[axisIndex].lMax = range.lMax;
            pResult->axis[axisIndex].bEnabled = true;
            pResult->axis[axisIndex].dwType = pdidoi->dwType;
            pResult->iAxisCount++;
        }

        return DIENUM_CONTINUE;
    }

    SJoystickScanResult ScanForDirectInputDevice(HWND hWindow, bool bPreferredValid, const GUID& preferredGuidInstance)
    {
        SJoystickScanResult  result;
        SJoystickEnumContext ctx;
        ctx.bPreferredValid = bPreferredValid;
        ctx.preferredGuidInstance = preferredGuidInstance;

        if (!bPreferredValid)
        {
            IDirectInputJoyConfig8* pJoyConfig = NULL;
            if (SUCCEEDED(g_pDirectInput8->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig)))
            {
                DIJOYCONFIG PreferredJoyCfg = {0};
                PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);
                if (SUCCEEDED(pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE)))  // Expected to fail if no Joystick is attached
                {
                    ctx.bPreferredValid = true;
                    ctx.preferredGuidInstance = PreferredJoyCfg.guidInstance;
                }
                SAFE_RELEASE(pJoyConfig);
            }
        }

        g_pDirectInput8->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallbackAsync, &ctx, DIEDFL_ATTACHEDONLY);
        result.listedDevices = std::move(ctx.listedDevices);

        if (!ctx.pDevice)
            return result;

        // In case device did not identify itself as a joysitck during creation,
        // set flag again to ensure input data will not be dropped when the mouse cursor is showing.
        CProxyDirectInputDevice8* pProxyInputDevice = dynamic_cast<CProxyDirectInputDevice8*>(ctx.pDevice);
        if (pProxyInputDevice)
            pProxyInputDevice->m_bDropDataIfInputGoesToGUI = false;

        // Set the data format to "simple Joystick" - a predefined data format telling DInput we'll
        // be passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
        ctx.pDevice->SetDataFormat(&c_dfDIJoystick2);

        // Set the cooperative level to let DInput know how this device should
        // interact with the system and with other DInput applications.
        ctx.pDevice->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

        result.pDevice = ctx.pDevice;

        // Enumerate the joystick's axes and set their range/deadzone/saturation properties
        ctx.pDevice->EnumObjects(EnumAxesCallbackAsync, &result, DIDFT_ALL);

        DIDEVICEINSTANCE didi;
        didi.dwSize = sizeof didi;
        if (SUCCEEDED(ctx.pDevice->GetDeviceInfo(&didi)))
        {
            result.guidProduct = didi.guidProduct;
            result.guidInstance = didi.guidInstance;
            result.strProductName = didi.tszProductName;
        }

        return result;
    }

    std::vector<std::pair<GUID, string>> EnumerateDirectInputDeviceList()
    {
        std::vector<std::pair<GUID, string>> list;
        if (g_pDirectInput8)
            g_pDirectInput8->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksListCallback, &list, DIEDFL_ATTACHEDONLY);
        return list;
    }
}  // namespace

///////////////////////////////////////////////////////////////
//
// CJoystickManager::RemoveDevice
//
// When notifed that a device is being removed, remove it from our list here.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::RemoveDevice(IDirectInputDevice8A* pDevice)
{
    if (m_DevInfo.pDevice == pDevice)
        m_DevInfo.pDevice = NULL;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::StartDirectInputScan
//
// Kicks off a background scan for a joystick device, if one isn't running already
//
///////////////////////////////////////////////////////////////
void CJoystickManager::StartDirectInputScan()
{
    if (m_bUseXInput || m_bScanRunning)
        return;

    m_bScanRunning = true;
    m_bScanReady = false;

    HWND hWindow = g_pCore->GetHookedWindow();
    bool bPreferredValid = m_bPreferredInstanceValid;
    GUID preferredGuid = m_PreferredInstanceGuid;
    m_ScanThread = std::thread(
        [this, hWindow, bPreferredValid, preferredGuid]()
        {
            m_ScanResult = ScanForDirectInputDevice(hWindow, bPreferredValid, preferredGuid);
            m_bScanReady.store(true, std::memory_order_release);
        });
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::CollectDirectInputScanResult
//
// Picks up the result of a background scan once it has finished. Called every DoPulse, so it
// never blocks - if the scan isn't ready yet, this just returns and tries again next tick.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::CollectDirectInputScanResult()
{
    if (!m_bScanReady.load(std::memory_order_acquire))
        return;

    m_ScanThread.join();
    SJoystickScanResult result = std::move(m_ScanResult);
    m_ScanResult = SJoystickScanResult();
    m_bScanReady = false;
    m_bScanRunning = false;

    m_ListedDInputDevices = std::move(result.listedDevices);
    BumpDeviceListRevision();

    if (!result.pDevice)
    {
        // Retries every few seconds for hotplug; only log the first miss (or after a device was lost)
        if (!m_bLoggedNoJoystick)
        {
            WriteDebugEvent("InitDirectInput - No Joystick found");
            m_bLoggedNoJoystick = true;
        }
        return;
    }

    m_bLoggedNoJoystick = false;

    if (m_bUseXInput)
    {
        // An XInput pad showed up while this scan was in flight, so it's not needed anymore
        result.pDevice->Release();
        return;
    }

    if (m_bPreferredInstanceValid && !IsEqualGUID(result.guidInstance, m_PreferredInstanceGuid))
    {
        // User picked a different DirectInput pad while this scan was in flight
        result.pDevice->Release();
        StartDirectInputScan();
        return;
    }

    m_DevInfo.pDevice = result.pDevice;
    m_DevInfo.iAxisCount = result.iAxisCount;
    m_DevInfo.guidProduct = result.guidProduct;
    m_DevInfo.strProductName = result.strProductName;
    m_DevInfo.strGuid = GUIDToString(m_DevInfo.guidProduct);

    for (int i = 0; i < NUMELMS(m_DevInfo.axis) && i < NUMELMS(result.axis); i++)
    {
        m_DevInfo.axis[i].bEnabled = result.axis[i].bEnabled;
        m_DevInfo.axis[i].lMin = result.axis[i].lMin;
        m_DevInfo.axis[i].lMax = result.axis[i].lMax;
        m_DevInfo.axis[i].dwType = result.axis[i].dwType;
        m_DevInfo.axis[i].fAutoDeadZoneSample = 0.f;
    }

    if (!LoadFromXML())
        SetDefaults();

    m_DevInfo.bDoneEnumAxes = true;

    // The device isn't Acquired yet, so its first Poll() is expected to fail. Start the fail timer
    // from here, otherwise it would count the time since the scan started and could drop the
    // device as unresponsive before it was ever given a chance to be polled.
    m_PollFailTimer.Reset();
}

void CJoystickManager::BumpDeviceListRevision()
{
    m_iDeviceListRevision++;
}

void CJoystickManager::StartDirectInputListRefresh()
{
    if (m_bListRefreshRunning || m_bScanRunning || !g_pDirectInput8)
        return;

    m_bListRefreshRunning = true;
    m_bListRefreshReady = false;

    m_ListRefreshThread = std::thread(
        [this]()
        {
            m_ListRefreshResult = EnumerateDirectInputDeviceList();
            m_bListRefreshReady.store(true, std::memory_order_release);
        });
}

void CJoystickManager::CollectDirectInputListRefresh()
{
    if (!m_bListRefreshReady.load(std::memory_order_acquire))
        return;

    if (m_ListRefreshThread.joinable())
        m_ListRefreshThread.join();

    m_ListedDInputDevices = std::move(m_ListRefreshResult);
    m_ListRefreshResult.clear();
    m_bListRefreshReady = false;
    m_bListRefreshRunning = false;
    BumpDeviceListRevision();
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::DoPulse
//
// Updates the joystick state and sends keydown/up messages for any
// buttons state changes.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::DoPulse()
{
    if (!m_bDoneInit)
    {
        if (!g_pDirectInput8)
            return;

        // Kick off the first scan in the background; DoPulse picks up the result once it's ready
        m_bDoneInit = true;
        StartDirectInputScan();
    }
    else
    {
        CollectDirectInputScanResult();
        CollectDirectInputListRefresh();

        // After a USB plug/unplug burst settles, refresh the device list off-thread so the
        // settings combo can update without blocking gameplay on DirectInput enumeration.
        if (m_bPendingDeviceListRefresh && m_DirectInputReattachTimer.Get() >= m_uiDirectInputReattachDelay)
        {
            m_bPendingDeviceListRefresh = false;
            StartDirectInputListRefresh();
        }

        if (!m_bUseXInput && !m_DevInfo.pDevice)
        {
            // Not using XInput yet and no DirectInput joystick either. Auto mode may pick up an
            // XInput pad that appeared after startup; a user-picked DirectInput device stays put.
            SParsedControllerId parsed = ParseControllerId(m_strSelectedControllerId);
            if (parsed.type != SParsedControllerId::DirectInput)
            {
                int iXInputIndex = (parsed.type == SParsedControllerId::XInput) ? parsed.iXInputIndex : FindFirstXInputIndex();
                if (iXInputIndex >= 0)
                {
                    m_iXInputUserIndex = iXInputIndex;
                    m_bUseXInput = true;
                }
            }

            if (!m_bUseXInput && !m_bScanRunning && m_DirectInputReattachTimer.Get() >= m_uiDirectInputReattachDelay)
            {
                StartDirectInputScan();
                m_DirectInputReattachTimer.Reset();
                m_uiDirectInputReattachDelay = JOYSTICK_RETRY_DELAY_MS;
            }
        }
    }

    // Stop if no joystick
    if (!IsJoypadConnected())
    {
        ApplyVibration();
        return;
    }

    //
    // Try to read current state
    //
    ReadCurrentState();

    //
    // Process current state
    //
    HWND hWnd = CMessageLoopHook::GetSingleton().GetHookedWindowHandle();

    // Turn the button presses into messages
    for (int i = 0; i < 32; i++)
    {
        const BYTE& NowPress = m_JoystickState.rgbButtons[i];
        BYTE&       WasPress = m_JoystickState.rgbButtonsWas[i];

        // Edge detection
        if (NowPress != WasPress)
        {
            WasPress = NowPress;

            if (NowPress)
                SendMessage(hWnd, WM_KEYDOWN, VK_JOY(i + 1), 0x00000001);
            else
                SendMessage(hWnd, WM_KEYUP, VK_JOY(i + 1), 0xC0000001);
        }
    }

    // Turn the pov into messages
    {
        BYTE povButtons[4] = {0, 0, 0, 0};

        int pov = m_JoystickState.rgdwPOV[0];

        if (pov != -1)
        {
            if (pov > 0 - 6000 && pov < 0 + 6000)
                povButtons[0] = 1;
            if (pov > 9000 - 6000 && pov < 9000 + 6000)
                povButtons[1] = 1;
            if (pov > 18000 - 6000 && pov < 18000 + 6000)
                povButtons[2] = 1;
            if (pov > 27000 - 6000 && pov < 27000 + 6000)
                povButtons[3] = 1;
            if (pov > 36000 - 6000 && pov < 36000 + 6000)
                povButtons[0] = 1;
        }

        for (int i = 0; i < 4; i++)
        {
            const BYTE& NowPress = povButtons[i];
            BYTE&       WasPress = m_JoystickState.povButtonsWas[i];

            // Edge detection
            if (NowPress != WasPress)
            {
                WasPress = NowPress;

                if (NowPress)
                    SendMessage(hWnd, WM_KEYDOWN, VK_POV(i + 1), 0x00000001);
                else
                    SendMessage(hWnd, WM_KEYUP, VK_POV(i + 1), 0xC0000001);
            }
        }
    }

    // Turn axis movement into button style messages
    {
        for (uint i = 0; i < NUMELMS(m_JoystickState.axisButtonsWas); i++)
        {
            uint uiAxisIndex = i >> 1;
            uint uiAxisDir = i & 1;

            if (uiAxisIndex >= NUMELMS(m_JoystickState.rgfAxis))
                break;

            BYTE NowPress;
            if (uiAxisDir)
                NowPress = m_JoystickState.rgfAxis[uiAxisIndex] > 0.75f;
            else
                NowPress = m_JoystickState.rgfAxis[uiAxisIndex] < -0.75f;

            BYTE& WasPress = m_JoystickState.axisButtonsWas[i];

            // Edge detection
            if (NowPress != WasPress)
            {
                WasPress = NowPress;

                if (NowPress)
                    SendMessage(hWnd, WM_KEYDOWN, VK_AXIS(i + 1), 0x00000001);
                else
                    SendMessage(hWnd, WM_KEYUP, VK_AXIS(i + 1), 0xC0000001);
            }
        }
    }

    // Handle capture and binding
    if (m_bCaptureAxis)
    {
        // See if any axes have changed to over 0.75
        for (int i = 0; i < NUMELMS(m_JoystickState.rgfAxis); i++)
        {
            // Half axis movement (0 to 1)
            if (fabs(m_JoystickState.rgfAxis[i]) > 0.75f)
                if (fabs(m_PreBindJoystickState.rgfAxis[i]) < 0.75f)
                {
                    m_bCaptureAxis = false;
                    // Save the mapping
                    m_currentMapping[m_iCaptureOutputIndex].bEnabled = true;
                    m_currentMapping[m_iCaptureOutputIndex].SourceAxisIndex = (eJoy)i;
                    m_currentMapping[m_iCaptureOutputIndex].SourceAxisDir = m_JoystickState.rgfAxis[i] < 0.f ? eDirNeg : eDirPos;
                    m_SettingsRevision++;
                }

            // Full axis movement (-1 to 1)
            if (m_JoystickState.rgfAxis[i] > 0.75f)
                if (m_PreBindJoystickState.rgfAxis[i] < -0.75f)
                {
                    m_bCaptureAxis = false;
                    // Save the mapping
                    m_currentMapping[m_iCaptureOutputIndex].bEnabled = true;
                    m_currentMapping[m_iCaptureOutputIndex].SourceAxisIndex = (eJoy)i;
                    m_currentMapping[m_iCaptureOutputIndex].SourceAxisDir = eDirNegToPos;
                    m_SettingsRevision++;
                }
        }
    }

    ApplyVibration();
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::ReadCurrentState
//
// Puts current state of the joystick into m_JoystickState.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::ReadCurrentState()
{
    // Clear current state
    for (int i = 0; i < NUMELMS(m_JoystickState.rgfAxis); i++)
        m_JoystickState.rgfAxis[i] = 0;
    for (int i = 0; i < 4; i++)
        m_JoystickState.rgdwPOV[i] = -1;
    for (int i = 0; i < 32; i++)
        m_JoystickState.rgbButtons[i] = 0;

    DIJOYSTATE2 js;  // DInput joystick state

    if (ReadInputSubsystem(js))
    {
        SString strStatus;
        bool    bOutputStatus = (g_pCore->GetDiagnosticDebug() == EDiagnosticDebug::JOYSTICK_0000) && !g_pCore->IsConnected();
        if (bOutputStatus)
        {
            strStatus += SString("iSaturation:%d iDeadZone:%d iTriggerSaturation:%d iTriggerDeadZone:%d\n", m_DevInfo.iSaturation, m_DevInfo.iDeadZone,
                                 m_DevInfo.iTriggerSaturation, m_DevInfo.iTriggerDeadZone);
        }

        if (m_iAutoDeadZoneCounter)
            m_iAutoDeadZoneCounter--;

        // Read axes
        for (int a = 0; a < NUMELMS(m_DevInfo.axis) && a < NUMELMS(m_JoystickState.rgfAxis); a++)
        {
            if (m_DevInfo.axis[a].bEnabled)
            {
                LONG lMin = m_DevInfo.axis[a].lMin;
                LONG lMax = m_DevInfo.axis[a].lMax;
                LONG lSize = lMax - lMin;

                // Re-range: -1.f to 1.f
                // (-min - half(size)) * 2.f / size
                float fResult = ((&js.lX)[a] - lMin - lSize / 2) * 2.f / lSize;

                const bool bTriggerAxis = IsTriggerSourceAxis(a);
                const int  iSaturation = bTriggerAxis ? m_DevInfo.iTriggerSaturation : m_DevInfo.iSaturation;
                const int  iDeadZoneSetting = bTriggerAxis ? m_DevInfo.iTriggerDeadZone : m_DevInfo.iDeadZone;

                ApplyAxisResponse(fResult, iSaturation);

                // Handle dead zone
                float DeadZone = iDeadZoneSetting * (1 / 100.f);

                // Handle auto dead zone detection (sticks only — trigger rest is 0, not centered)
                if (!bTriggerAxis)
                {
                    if (m_iAutoDeadZoneCounter > 1)
                    {
                        // Sample phase - Record lowest axis value
                        if (abs(fResult) < m_DevInfo.axis[a].fAutoDeadZoneSample || m_DevInfo.axis[a].fAutoDeadZoneSample == 0.f)
                            m_DevInfo.axis[a].fAutoDeadZoneSample = abs(fResult);
                    }
                    else
                    {
                        // Use auto dead zone if required
                        int iAutoDeadZone = m_DevInfo.axis[a].fAutoDeadZoneSample * 110;
                        if (iAutoDeadZone < 30 && iAutoDeadZone > m_DevInfo.iDeadZone && m_bAutoDeadZoneEnabled)
                        {
                            DeadZone = iAutoDeadZone * (1 / 100.f);
                            if (m_iAutoDeadZoneCounter == 1)
                                WriteDebugEvent(
                                    SString("CJoystickManager - Changing deadzone for axis %d from %d to %d", a, m_DevInfo.iDeadZone, iAutoDeadZone));
                        }
                    }
                }

                // Apply dead zone
                if (fResult >= 0.f)
                    fResult = std::max(0.f, fResult - DeadZone);
                else
                    fResult = std::min(0.f, fResult + DeadZone);

                fResult = fResult * (1 / (1 - DeadZone));

                // Clamp range: -1.f to 1.f
                m_JoystickState.rgfAxis[a] = Clamp(-1.f, fResult, 1.f);

                if (bOutputStatus)
                {
                    DIPROPRANGE range;
                    range.diph.dwSize = sizeof(DIPROPRANGE);
                    range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
                    range.diph.dwHow = DIPH_BYID;
                    range.diph.dwObj = m_DevInfo.axis[a].dwType;  // Specify the enumerated axis
                    range.lMin = -2001;
                    range.lMax = +2001;

                    if (m_DevInfo.pDevice)
                        m_DevInfo.pDevice->GetProperty(DIPROP_RANGE, &range.diph);

                    // Remove Deadzone and Saturation
                    DIPROPDWORD dead, sat;

                    dead.diph.dwSize = sizeof dead;
                    dead.diph.dwHeaderSize = sizeof dead.diph;
                    dead.diph.dwHow = DIPH_BYID;
                    dead.diph.dwObj = m_DevInfo.axis[a].dwType;
                    dead.dwData = 1;

                    sat = dead;
                    sat.dwData = 9999;

                    if (m_DevInfo.pDevice)
                    {
                        m_DevInfo.pDevice->GetProperty(DIPROP_DEADZONE, &dead.diph);
                        m_DevInfo.pDevice->GetProperty(DIPROP_SATURATION, &sat.diph);
                    }

                    strStatus += SString("Axis:%d lMin:%d lMax:%d dead:%d sat:%d raw:%d result:%1.4f\n", a, range.lMin, range.lMax, dead.dwData, sat.dwData,
                                         (&js.lX)[a], fResult);
                }
            }
            else
            {
                if (bOutputStatus)
                {
                    strStatus += SString("Axis:%d raw:%d\n", a, (&js.lX)[a]);
                }
            }
        }

        // Read POV
        for (int i = 0; i < 4; i++)
            m_JoystickState.rgdwPOV[i] = js.rgdwPOV[i];

        // Read buttons
        for (int i = 0; i < 32; i++)
            m_JoystickState.rgbButtons[i] = js.rgbButtons[i];

        if (bOutputStatus)
        {
            CGraphicsInterface* pGraphics = CCore::GetSingleton().GetGraphics();
            int                 x = 20;
            int                 y = 20;  // pGraphics->GetViewportHeight() / 2;
            pGraphics->DrawRectQueued(x, y, 350, 150, 0xaf000000, true);
            pGraphics->DrawStringQueued(x + 10, y + 10, 0, 0, 0xFFFFFFFF, strStatus, 1, 1, DT_NOCLIP, NULL, true);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::ReadInputSubsystem
//
//
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::ReadInputSubsystem(DIJOYSTATE2& js)
{
    if (!m_bUseXInput)
    {
        //
        // DirectInput
        //

        if (!m_DevInfo.pDevice)
            return false;

        // Try to poll
        if (FAILED(m_DevInfo.pDevice->Poll()))
        {
            if (m_PollFailTimer.Get() > JOYSTICK_RETRY_DELAY_MS)
            {
                // Been failing to respond for a while, most likely unplugged, so forget it and look for a replacement
                m_DevInfo.pDevice->Release();
                m_DevInfo.pDevice = nullptr;
                m_DevInfo.bDoneEnumAxes = false;
                memset(m_DevInfo.axis, 0, sizeof(m_DevInfo.axis));
                m_DevInfo.iAxisCount = 0;
            }
            else if (m_AcquireRetryTimer.Get() >= m_uiAcquireRetryDelay)
            {
                // Acquire() can block while Windows is still enumerating unrelated USB devices,
                // so retry occasionally instead of hammering it every frame.
                m_DevInfo.pDevice->Acquire();
                m_AcquireRetryTimer.Reset();
            }
            return false;
        }
        m_PollFailTimer.Reset();
        m_AcquireRetryTimer.Reset();

        if (FAILED(m_DevInfo.pDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
            return false;
    }
    else
    {
        //
        // XInput
        //

        XINPUT_STATE XInputState;
        if (!HandleXInputGetState(XInputState))
            return false;

        memset(&js, 0, sizeof(DIJOYSTATE2));
        LONG* pAxes = (&js.lX);
        pAxes[0] = XInputState.Gamepad.sThumbLX;
        pAxes[1] = XInputState.Gamepad.sThumbLY;
        pAxes[2] = XInputState.Gamepad.bLeftTrigger;
        pAxes[3] = XInputState.Gamepad.sThumbRX;
        pAxes[4] = XInputState.Gamepad.sThumbRY;
        pAxes[5] = XInputState.Gamepad.bRightTrigger;

        js.rgbButtons[0] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1 : 0;
        js.rgbButtons[1] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1 : 0;
        js.rgbButtons[2] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1 : 0;
        js.rgbButtons[3] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1 : 0;
        js.rgbButtons[4] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1 : 0;
        js.rgbButtons[5] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1 : 0;
        js.rgbButtons[6] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1 : 0;
        js.rgbButtons[7] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1 : 0;
        js.rgbButtons[8] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1 : 0;
        js.rgbButtons[9] = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1 : 0;

        bool bPovUp = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1 : 0;
        bool bPovRight = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1 : 0;
        bool bPovDown = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1 : 0;
        bool bPovLeft = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1 : 0;

        if (bPovUp)
        {
            if (bPovRight)
                js.rgdwPOV[0] = 0 + 4500;
            else if (bPovLeft)
                js.rgdwPOV[0] = 36000 - 4500;
            else
                js.rgdwPOV[0] = 0;
        }
        else if (bPovDown)
        {
            if (bPovRight)
                js.rgdwPOV[0] = 18000 - 4500;
            else if (bPovLeft)
                js.rgdwPOV[0] = 18000 + 4500;
            else
                js.rgdwPOV[0] = 18000;
        }
        else
        {
            if (bPovRight)
                js.rgdwPOV[0] = 9000;
            else if (bPovLeft)
                js.rgdwPOV[0] = 27000;
            else
                js.rgdwPOV[0] = -1;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::HandleXInputGetState
//
//
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::HandleXInputGetState(XINPUT_STATE& XInputState)
{
    if (!IsXInputDeviceAttached())
        return false;

    DWORD dwStatus = XInputGetState(m_iXInputUserIndex, &XInputState);

    if (dwStatus == ERROR_DEVICE_NOT_CONNECTED)
    {
        m_bXInputDeviceAttached = false;
        return false;
    }

    if (dwStatus != ERROR_SUCCESS)
        return false;

    return true;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::IsXInputDeviceAttached
//
// Also attempts reattach if required
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::IsXInputDeviceAttached()
{
    if (!m_bXInputDeviceAttached)
    {
        // Delay before reattached for performance
        if (m_XInputReattachTimer.Get() < m_uiXInputReattachDelay)
            return false;
        m_XInputReattachTimer.Reset();
        m_uiXInputReattachDelay = JOYSTICK_RETRY_DELAY_MS;

        XINPUT_CAPABILITIES Capabilities;
        DWORD               dwStatus = XInputGetCapabilities(m_iXInputUserIndex, XINPUT_FLAG_GAMEPAD, &Capabilities);
        if (dwStatus != ERROR_SUCCESS)
            return false;

        m_bXInputDeviceAttached = true;

        // Axis ranges for XInput devices
        m_DevInfo.axis[0].bEnabled = 1;
        m_DevInfo.axis[0].lMin = -32767;
        m_DevInfo.axis[0].lMax = 32767;

        m_DevInfo.axis[1].bEnabled = 1;
        m_DevInfo.axis[1].lMin = -32767;
        m_DevInfo.axis[1].lMax = 32767;

        m_DevInfo.axis[2].bEnabled = 1;
        m_DevInfo.axis[2].lMin = -255;
        m_DevInfo.axis[2].lMax = 255;

        m_DevInfo.axis[3].bEnabled = 1;
        m_DevInfo.axis[3].lMin = -32767;
        m_DevInfo.axis[3].lMax = 32767;

        m_DevInfo.axis[4].bEnabled = 1;
        m_DevInfo.axis[4].lMin = -32767;
        m_DevInfo.axis[4].lMax = 32767;

        m_DevInfo.axis[5].bEnabled = 1;
        m_DevInfo.axis[5].lMin = -255;
        m_DevInfo.axis[5].lMax = 255;

        m_DevInfo.axis[6].bEnabled = 0;

        // Compose a guid for saving config
        m_DevInfo.guidProduct.Data1 = 0x12345678;
        m_DevInfo.guidProduct.Data2 = Capabilities.Type;
        m_DevInfo.guidProduct.Data3 = Capabilities.SubType;

        // Compose a product name
        m_DevInfo.strProductName = GetXInputDisplayName(m_iXInputUserIndex, Capabilities);

        // Config is keyed by product GUID, so a brief disconnect/reconnect doesn't need a reload.
        const string strNewGuid = GUIDToString(m_DevInfo.guidProduct);
        const bool   bHadConfig = (strNewGuid == m_DevInfo.strGuid);
        m_DevInfo.strGuid = strNewGuid;
        if (!bHadConfig)
        {
            if (!LoadFromXML())
                SetDefaults();
        }
    }

    return m_bXInputDeviceAttached;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::ApplyAxes
//
// Map physical axes into CControllerState
//
///////////////////////////////////////////////////////////////
void CJoystickManager::ApplyAxes(CControllerState& cs, bool bInVehicle)
{
    if (!IsJoypadConnected())
        return;

    int leftStickX = cs.LeftStickX;
    int leftStickY = cs.LeftStickY;
    int rightStickX = cs.RightStickX;
    int rightStickY = cs.RightStickY;
    int buttonCross = cs.ButtonCross;
    int buttonSquare = cs.ButtonSquare;

    // Map each half axis
    for (int i = 0; i < NUMELMS(m_currentMapping); i++)
    {
        const SMappingLine& line = m_currentMapping[i];

        if (!line.bEnabled)
            continue;

        float value = m_JoystickState.rgfAxis[line.SourceAxisIndex];

        if (line.SourceAxisDir == eDirPos)
            value = std::max(0.f, value);
        else if (line.SourceAxisDir == eDirNeg)
            value = -std::min(0.f, value);
        else if (line.SourceAxisDir == eDirNegToPos)
            value = value * 0.5f + 0.5f;

        if (line.OutputAxisDir == eDirNeg)
            value = -value;

        auto sValue = static_cast<short>(Round(value * line.MaxValue));

        if (line.OutputAxisIndex == eLeftStickX)
            leftStickX += sValue;
        else if (line.OutputAxisIndex == eLeftStickY)
            leftStickY += sValue;
        else if (line.OutputAxisIndex == eRightStickX)
            rightStickX += sValue;
        else if (line.OutputAxisIndex == eRightStickY)
            rightStickY += sValue;
        else if (line.OutputAxisIndex == eAccelerate && bInVehicle)
            buttonCross += sValue;
        else if (line.OutputAxisIndex == eBrake && bInVehicle)
            buttonSquare += sValue;
    }

    // Keep everything in range
    cs.LeftStickX = static_cast<short>(Clamp<int>(-128, leftStickX, 128));
    cs.LeftStickY = static_cast<short>(Clamp<int>(-128, leftStickY, 128));
    cs.RightStickX = static_cast<short>(Clamp<int>(-128, rightStickX, 128));
    cs.RightStickY = static_cast<short>(Clamp<int>(-128, rightStickY, 128));

    cs.ButtonCross = static_cast<short>(Clamp<int>(0, buttonCross, 255));
    cs.ButtonSquare = static_cast<short>(Clamp<int>(0, buttonSquare, 255));

    // Debug output
#ifdef MTA_DEBUG
    #if 0

    SString strBuffer = SString::Printf ( "LeftShoulder1: %u\n"
                                "LeftShoulder2: %u\n"
                                "RightShoulder1: %u\n"
                                "RightShoulder2: %u\n"
                                "DPadUp: %u\n"
                                "DPadDown: %u\n"
                                "DPadLeft: %u\n"
                                "DPadRight: %u\n"
                                "Start: %u\n"
                                "Select: %u\n"
                                "ButtonSquare: %u\n"
                                "ButtonTriangle: %u\n"
                                "ButtonCross: %u\n"
                                "ButtonCircle: %u\n"
                                "ShockButtonL: %u\n"
                                "ShockButtonR: %u\n"
                                "PedWalk: %u\n",
                                cs.LeftShoulder1,
                                cs.LeftShoulder2,
                                cs.RightShoulder1,
                                cs.RightShoulder2,
                                cs.DPadUp,
                                cs.DPadDown,
                                cs.DPadLeft,
                                cs.DPadRight,
                                cs.Start,
                                cs.Select,
                                cs.ButtonSquare,
                                cs.ButtonTriangle,
                                cs.ButtonCross,
                                cs.ButtonCircle,
                                cs.ShockButtonL,
                                cs.ShockButtonR,
                                cs.m_bPedWalk );

    CCore::GetSingleton ().GetGraphics ()->DrawString ( 20, 150, 0xFFFFFFFF, 1, strBuffer );

    strBuffer = SString::Printf ( "VehicleMouseLook: %u\n"
                                "LeftStickX: %u\n"
                                "LeftStickY: %u\n"
                                "RightStickX: %u\n"
                                "RightStickY: %u",
                                cs.m_bVehicleMouseLook,
                                cs.LeftStickX,
                                cs.LeftStickY,
                                cs.RightStickX,
                                cs.RightStickY );

    CCore::GetSingleton ().GetGraphics ()->DrawString ( 20, 450, 0xFFFFFFFF, 1, strBuffer );


    strBuffer = SString::Printf (
                                "AxisX: %5.8f\n"
                                "AxisY: %5.8f\n"
                                "AxisZ: %5.8f\n"
                                "AxisRx: %5.8f\n"
                                "AxisRy: %5.8f\n"
                                "AxisRz: %5.8f\n"
                                "AxisSld: %5.8f",
                                m_JoystickState.rgfAxis[0],
                                m_JoystickState.rgfAxis[1],
                                m_JoystickState.rgfAxis[2],
                                m_JoystickState.rgfAxis[3],
                                m_JoystickState.rgfAxis[4],
                                m_JoystickState.rgfAxis[5],
                                m_JoystickState.rgfAxis[6]
                            );

    CCore::GetSingleton ().GetGraphics ()->DrawString ( 20, 550, 0xFFFFFFFF, 1, strBuffer );

    #endif
#endif
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager Status
//
///////////////////////////////////////////////////////////////

bool CJoystickManager::IsJoypadConnected()
{
    if (m_bUseXInput)
        return IsXInputDeviceAttached();
    return m_DevInfo.pDevice != NULL;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::OnPossibleDeviceChange
//
// Called when Windows tells us a HID device was plugged in or removed, so we don't have to wait
// for the next scheduled retry to notice.
//
// A single physical connect or disconnect can fire several of these in a row, one per device
// interface, so this pushes the check back a bit instead of running it right away. Each extra
// notification in the same burst just pushes it back again, and it only actually runs once
// things go quiet, by which point Windows is done updating the device list.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::OnPossibleDeviceChange()
{
    m_bPendingDeviceListRefresh = true;
    m_uiDirectInputReattachDelay = JOYSTICK_DEVICE_CHANGE_SETTLE_MS;
    m_DirectInputReattachTimer.Reset();

    m_uiXInputReattachDelay = JOYSTICK_DEVICE_CHANGE_SETTLE_MS;
    m_XInputReattachTimer.Reset();
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager Settings
//
///////////////////////////////////////////////////////////////

string CJoystickManager::GetControllerName()
{
    return m_DevInfo.strProductName;
}

int CJoystickManager::GetDeadZone()
{
    return m_DevInfo.iDeadZone;
}

int CJoystickManager::GetSaturation()
{
    return m_DevInfo.iSaturation;
}

int CJoystickManager::GetTriggerDeadZone()
{
    return m_DevInfo.iTriggerDeadZone;
}

int CJoystickManager::GetTriggerSaturation()
{
    return m_DevInfo.iTriggerSaturation;
}

void CJoystickManager::SetDeadZone(int iDeadZone)
{
    m_SettingsRevision++;
    if (iDeadZone != m_DevInfo.iDeadZone)
        m_bAutoDeadZoneEnabled = false;  // Disable auto dead zone on change (user edit)
    m_DevInfo.iDeadZone = Clamp(0, iDeadZone, JOYSTICK_DEADZONE_MAX);
}

void CJoystickManager::SetSaturation(int iSaturation)
{
    m_SettingsRevision++;
    m_DevInfo.iSaturation = Clamp(0, iSaturation, JOYSTICK_SATURATION_MAX);
}

void CJoystickManager::SetTriggerDeadZone(int iDeadZone)
{
    m_SettingsRevision++;
    m_DevInfo.iTriggerDeadZone = Clamp(0, iDeadZone, JOYSTICK_DEADZONE_MAX);
}

void CJoystickManager::SetTriggerSaturation(int iSaturation)
{
    m_SettingsRevision++;
    m_DevInfo.iTriggerSaturation = Clamp(0, iSaturation, JOYSTICK_SATURATION_MAX);
}

bool CJoystickManager::GetVibrationEnabled()
{
    return m_bVibrationEnabled;
}

void CJoystickManager::SetVibrationEnabled(bool bEnabled)
{
    if (m_bVibrationEnabled == bEnabled)
        return;

    m_bVibrationEnabled = bEnabled;
    CVARS_SET("controller_vibration", bEnabled);
    m_SettingsRevision++;
    SyncGtaVibrationPref();
    if (!bEnabled)
        StopVibration();
}

string CJoystickManager::GetSelectedControllerId()
{
    return m_strSelectedControllerId;
}

void CJoystickManager::SetSelectedControllerId(const string& strId)
{
    string strNew = strId.empty() ? "auto" : strId;
    if (strNew == m_strSelectedControllerId)
        return;

    StopVibration();
    m_strSelectedControllerId = strNew;
    CVARS_SET("controller_device", m_strSelectedControllerId);
    m_SettingsRevision++;
    ApplyControllerSelection(true);
}

std::vector<SJoystickDeviceChoice> CJoystickManager::GetAvailableControllers()
{
    std::vector<SJoystickDeviceChoice> list;
    list.push_back({"auto", _("Automatic")});

    for (int i = 0; i < 4; i++)
    {
        XINPUT_CAPABILITIES Capabilities;
        if (XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &Capabilities) == ERROR_SUCCESS)
            list.push_back({SString("xinput:%d", i), GetXInputDisplayName(i, Capabilities)});
    }

    for (const auto& device : m_ListedDInputDevices)
        list.push_back({SString("dinput:%s", GUIDToString(device.first).c_str()), device.second});

    return list;
}

int CJoystickManager::GetSettingsRevision()
{
    return m_SettingsRevision;
}

int CJoystickManager::GetDeviceListRevision()
{
    return m_iDeviceListRevision;
}

bool CJoystickManager::IsJoypadValid()
{
    if (m_bUseXInput)
        return IsXInputDeviceAttached();
    return m_DevInfo.pDevice != NULL && m_DevInfo.bDoneEnumAxes && m_DevInfo.strGuid.size() > 0;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetConfigNode
//
// Get the main node for load/saving data for the current joypad.
//
///////////////////////////////////////////////////////////////
CXMLNode* CJoystickManager::GetConfigNode(bool bCreateIfRequired)
{
    // Get the root node
    CXMLNode* pRoot = CCore::GetSingleton().GetConfig();
    if (!pRoot)
        return NULL;

    // Get the top joypad config node
    CXMLNode* pSectionNode = pRoot->FindSubNode(CONFIG_NODE_JOYPAD);
    if (!pSectionNode)
    {
        if (!bCreateIfRequired)
            return NULL;

        // Non-existant, create a new node
        pSectionNode = pRoot->CreateSubNode(CONFIG_NODE_JOYPAD);
    }

    // Get the node for this joystick's GUID

    CXMLNode* pItemNode = NULL;
    // Find existing node
    for (int i = 0; true; i++)
    {
        CXMLNode* pNode = pSectionNode->FindSubNode("product", i);

        if (!pNode)
            break;

        CXMLAttributes* pAttributes = &pNode->GetAttributes();

        if (CXMLAttribute* pA = pAttributes->Find("guid"))
        {
            string value = pA->GetValue();
            if (value == m_DevInfo.strGuid)
            {
                pItemNode = pNode;
                break;
            }
        }
    }

    if (!pItemNode)
    {
        if (!bCreateIfRequired)
            return NULL;

        // Non-existant, create a new node
        pItemNode = pSectionNode->CreateSubNode("product");

        if (pItemNode)
        {
            CXMLAttributes* pAttributes = &pItemNode->GetAttributes();

            CXMLAttribute* pA = NULL;
            pA = pAttributes->Create("guid");
            pA->SetValue(m_DevInfo.strGuid.c_str());
        }
    }

    return pItemNode;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::SetDefaults
//
// Set the default axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::SetDefaults()
{
    m_SettingsRevision++;

    const SMappingLine defaultMappingStd[] = {
        eJoyX,  eDirNeg, eLeftStickX,  eDirNeg, true,  128, eJoyX,  eDirPos, eLeftStickX,  eDirPos, true,  128,
        eJoyY,  eDirNeg, eLeftStickY,  eDirNeg, true,  128, eJoyY,  eDirPos, eLeftStickY,  eDirPos, true,  128,
        eJoyZ,  eDirNeg, eRightStickX, eDirNeg, true,  128, eJoyZ,  eDirPos, eRightStickX, eDirPos, true,  128,
        eJoyRz, eDirNeg, eRightStickY, eDirNeg, true,  128, eJoyRz, eDirPos, eRightStickY, eDirPos, true,  128,
        eJoyZ,  eDirNeg, eAccelerate,  eDirPos, false, 255, eJoyZ,  eDirPos, eBrake,       eDirPos, false, 255,
    };

    const SMappingLine defaultMapping360[] = {
        eJoyX,  eDirNeg, eLeftStickX,  eDirNeg, true, 128, eJoyX,  eDirPos, eLeftStickX,  eDirPos, true, 128, eJoyY,  eDirNeg, eLeftStickY,  eDirNeg, true, 128,
        eJoyY,  eDirPos, eLeftStickY,  eDirPos, true, 128, eJoyRx, eDirNeg, eRightStickX, eDirNeg, true, 128, eJoyRx, eDirPos, eRightStickX, eDirPos, true, 128,
        eJoyRy, eDirNeg, eRightStickY, eDirNeg, true, 128, eJoyRy, eDirPos, eRightStickY, eDirPos, true, 128, eJoyZ,  eDirNeg, eAccelerate,  eDirPos, true, 255,
        eJoyZ,  eDirPos, eBrake,       eDirPos, true, 255,
    };

    const SMappingLine defaultMappingXInput[] = {
        eJoyX,  eDirNeg, eLeftStickX,  eDirNeg, true, 128, eJoyX,  eDirPos, eLeftStickX,  eDirPos, true, 128, eJoyY,  eDirPos, eLeftStickY,  eDirNeg, true, 128,
        eJoyY,  eDirNeg, eLeftStickY,  eDirPos, true, 128, eJoyRx, eDirNeg, eRightStickX, eDirNeg, true, 128, eJoyRx, eDirPos, eRightStickX, eDirPos, true, 128,
        eJoyRy, eDirPos, eRightStickY, eDirNeg, true, 128, eJoyRy, eDirNeg, eRightStickY, eDirPos, true, 128, eJoyRz, eDirPos, eAccelerate,  eDirPos, true, 255,
        eJoyZ,  eDirPos, eBrake,       eDirPos, true, 255,
    };

    memset(m_currentMapping, 0, sizeof(m_currentMapping));

    // Select defaultMapping to use

    if (m_bUseXInput)
    {
        // If XInput device, then use default XInput mapping
        for (int i = 0; i < NUMELMS(m_currentMapping); i++)
            m_currentMapping[i] = defaultMappingXInput[i];

        m_DevInfo.iDeadZone = 20;
        m_DevInfo.iSaturation = 99;
        m_DevInfo.iTriggerDeadZone = JOYSTICK_DEFAULT_TRIGGER_DEADZONE;
        m_DevInfo.iTriggerSaturation = 99;
    }
    else if (m_DevInfo.pDevice && m_DevInfo.iAxisCount == 5 && m_DevInfo.guidProduct == GUID_Xbox360Controller)
    {
        // If GUID matches published 360 controller GUID and device has 5 axes, then use 360 mapping
        for (int i = 0; i < NUMELMS(m_currentMapping); i++)
            m_currentMapping[i] = defaultMapping360[i];

        m_DevInfo.iDeadZone = 18;
        m_DevInfo.iSaturation = 99;
        m_DevInfo.iTriggerDeadZone = JOYSTICK_DEFAULT_TRIGGER_DEADZONE;
        m_DevInfo.iTriggerSaturation = 99;
    }
    else
    {
        // Otherwise use generic dual axis mapping
        for (int i = 0; i < NUMELMS(m_currentMapping); i++)
            m_currentMapping[i] = defaultMappingStd[i];

        m_DevInfo.iDeadZone = 12;
        m_DevInfo.iSaturation = 99;
        m_DevInfo.iTriggerDeadZone = JOYSTICK_DEFAULT_TRIGGER_DEADZONE;
        m_DevInfo.iTriggerSaturation = 99;
    }
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::LoadFromXML
//
// Load axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::LoadFromXML()
{
    m_SettingsRevision++;

    // Try load
    CXMLNode* pMainNode = GetConfigNode(false);

    if (!pMainNode)
        return false;

    int iErrors = 0;

    {
        // Find the 'info' node
        CXMLNode* pNode = pMainNode->FindSubNode("info");

        // If it was found
        if (pNode)
        {
            CXMLAttributes* pAttributes = &pNode->GetAttributes();

            CXMLAttribute* pA = NULL;
            if (pA = pAttributes->Find("deadzone"))
                m_DevInfo.iDeadZone = Clamp(0, atoi(pA->GetValue().c_str()), JOYSTICK_DEADZONE_MAX);
            else
                iErrors++;

            if (pA = pAttributes->Find("saturation"))
                m_DevInfo.iSaturation = Clamp(0, atoi(pA->GetValue().c_str()), JOYSTICK_SATURATION_MAX);
            else
                iErrors++;

            // Missing trigger fields keep the stick values so existing configs don't change feel.
            if (pA = pAttributes->Find("trigger_deadzone"))
                m_DevInfo.iTriggerDeadZone = Clamp(0, atoi(pA->GetValue().c_str()), JOYSTICK_DEADZONE_MAX);
            else
                m_DevInfo.iTriggerDeadZone = m_DevInfo.iDeadZone;

            if (pA = pAttributes->Find("trigger_saturation"))
                m_DevInfo.iTriggerSaturation = Clamp(0, atoi(pA->GetValue().c_str()), JOYSTICK_SATURATION_MAX);
            else
                m_DevInfo.iTriggerSaturation = m_DevInfo.iSaturation;
        }
        else
            iErrors++;
    }

    // Iterate the binds reading them from the XML tree
    for (int i = 0; i < NUMELMS(m_currentMapping); i++)
    {
        SMappingLine& line = m_currentMapping[i];

        // Find the 'axis' node
        CXMLNode* pNode = pMainNode->FindSubNode("axis", i);

        // If it was found
        if (pNode)
        {
            CXMLAttributes* pAttributes = &pNode->GetAttributes();

            CXMLAttribute* pA = NULL;
            if (pA = pAttributes->Find("source_index"))
                line.SourceAxisIndex = (eJoy)Clamp<int>(0, atoi(pA->GetValue().c_str()), eJoyMax);
            else
                iErrors++;

            if (pA = pAttributes->Find("source_dir"))
                line.SourceAxisDir = (eDir)Clamp<int>(0, atoi(pA->GetValue().c_str()), eDirMax);
            else
                iErrors++;

            if (pA = pAttributes->Find("output_index"))
                line.OutputAxisIndex = (eStick)Clamp<int>(0, atoi(pA->GetValue().c_str()), eStickMax);
            else
                iErrors++;

            if (pA = pAttributes->Find("output_dir"))
                line.OutputAxisDir = (eDir)Clamp<int>(0, atoi(pA->GetValue().c_str()), eDirMax);
            else
                iErrors++;

            if (pA = pAttributes->Find("enabled"))
                line.bEnabled = atoi(pA->GetValue().c_str()) ? true : false;
            else
                iErrors++;

            if (pA = pAttributes->Find("max_value"))
                line.MaxValue = atoi(pA->GetValue().c_str());
            else
                iErrors++;
        }
        else
            iErrors++;
    }

    if (iErrors)
        if (CCore::GetSingleton().GetConsole())
            CCore::GetSingleton().GetConsole()->Printf("Warning: %d errors reading joypad configuration.", iErrors);

    return true;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::SaveToXML
//
// Save axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::SaveToXML()
{
    if (!IsJoypadValid())
        return false;

    m_SettingsRevision++;

    CXMLNode* pMainNode = GetConfigNode(true);

    // Add the current settings
    if (pMainNode)
    {
        // Clear our current bind nodes
        pMainNode->DeleteAllSubNodes();

        {
            // Create a new 'info' node
            CXMLNode* pNode = pMainNode->CreateSubNode("info");

            // If it was created
            if (pNode)
            {
                CXMLAttributes* pAttributes = &pNode->GetAttributes();

                CXMLAttribute* pA = NULL;
                pA = pAttributes->Create("deadzone");
                pA->SetValue(m_DevInfo.iDeadZone);

                pA = pAttributes->Create("saturation");
                pA->SetValue(m_DevInfo.iSaturation);

                pA = pAttributes->Create("trigger_deadzone");
                pA->SetValue(m_DevInfo.iTriggerDeadZone);

                pA = pAttributes->Create("trigger_saturation");
                pA->SetValue(m_DevInfo.iTriggerSaturation);

                pA = pAttributes->Create("product_name");
                pA->SetValue(m_DevInfo.strProductName.c_str());
            }
        }

        // Iterate the binds adding them to the XML tree
        for (int i = 0; i < NUMELMS(m_currentMapping); i++)
        {
            const SMappingLine& line = m_currentMapping[i];

            // Create the new 'axis' node
            CXMLNode* pNode = pMainNode->CreateSubNode("axis");

            // If it was created
            if (pNode)
            {
                CXMLAttributes* pAttributes = &pNode->GetAttributes();

                CXMLAttribute* pA = NULL;
                pA = pAttributes->Create("source_index");
                pA->SetValue(line.SourceAxisIndex);

                pA = pAttributes->Create("source_dir");
                pA->SetValue(line.SourceAxisDir);

                pA = pAttributes->Create("output_index");
                pA->SetValue(line.OutputAxisIndex);

                pA = pAttributes->Create("output_dir");
                pA->SetValue(line.OutputAxisDir);

                pA = pAttributes->Create("enabled");
                pA->SetValue(line.bEnabled);

                pA = pAttributes->Create("max_value");
                pA->SetValue(line.MaxValue);
            }
        }
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::IsTriggerSourceAxis
//
// True when this physical axis is mapped to accelerate or brake, so we can apply
// trigger-specific deadzone/saturation instead of the stick settings.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::IsTriggerSourceAxis(int iAxisIndex) const
{
    for (int i = 0; i < NUMELMS(m_currentMapping); i++)
    {
        const SMappingLine& line = m_currentMapping[i];
        if (line.bEnabled && line.SourceAxisIndex == iAxisIndex && (line.OutputAxisIndex == eAccelerate || line.OutputAxisIndex == eBrake))
            return true;
    }
    return false;
}

void CJoystickManager::ApplyAxisResponse(float& fResult, int iSaturation)
{
    float Saturation = iSaturation * (1 / 100.f);
    if (Saturation <= 1.f)
        fResult += fResult * (1 - Saturation);
    else
        fResult *= Saturation;
}

void CJoystickManager::StopVibration()
{
    XINPUT_VIBRATION vibration{};
    XInputSetState(m_iXInputUserIndex, &vibration);
    m_bVibrationWasActive = false;
}

void CJoystickManager::SyncGtaVibrationPref()
{
    // CPad::StartShake (0x53F920) returns immediately unless this frontend pref is set.
    // PC SA never exposes it, so our checkbox has to drive it or rumble never starts.
    if (!g_pCore->GetGame())
        return;

    *reinterpret_cast<bool*>(0xBA6748 + 0x20) = m_bVibrationEnabled;
}

void CJoystickManager::ApplyVibration()
{
    // Get() is time since the last Reset, not a one-shot delta. Always tick it or a later
    // XInput switch inherits minutes of idle time and eats the whole ShakeDur in one frame.
    const int iElapsedMs = static_cast<int>(std::min<unsigned long long>(m_VibrationTimer.Get(), 50));
    m_VibrationTimer.Reset();

    if (!m_bUseXInput)
        return;

    XINPUT_VIBRATION vibration{};
    bool             bShouldRumble = false;

    CGame* pGame = g_pCore->GetGame();
    if (pGame && pGame->GetSystemState() == SystemState::GS_PLAYING_GAME)
        SyncGtaVibrationPref();

    if (m_bVibrationEnabled && m_bXInputDeviceAttached && IsJoypadConnected())
    {
        if (pGame && pGame->GetSystemState() == SystemState::GS_PLAYING_GAME)
        {
            if (CPad* pPad = pGame->GetPad())
            {
                short         iShakeDur = pPad->GetShakeDur();
                unsigned char ucShakeFreq = pPad->GetShakeFreq();
                if (iShakeDur > 0)
                {
                    short iRemaining = static_cast<short>(std::max(0, static_cast<int>(iShakeDur) - std::max(iElapsedMs, 1)));
                    pPad->SetShakeDur(iRemaining);
                    if (iRemaining > 0 && ucShakeFreq > 0)
                    {
                        WORD wSpeed = static_cast<WORD>(ucShakeFreq * 257);
                        vibration.wLeftMotorSpeed = wSpeed;
                        vibration.wRightMotorSpeed = wSpeed;
                        bShouldRumble = true;
                    }
                }
            }
        }
    }

    if (bShouldRumble || m_bVibrationWasActive)
        XInputSetState(m_iXInputUserIndex, &vibration);

    m_bVibrationWasActive = bShouldRumble;
}

void CJoystickManager::ReleaseDirectInputDevice()
{
    if (m_DevInfo.pDevice)
    {
        m_DevInfo.pDevice->Unacquire();
        m_DevInfo.pDevice->Release();
        m_DevInfo.pDevice = nullptr;
    }
    m_DevInfo.bDoneEnumAxes = false;
    m_DevInfo.iAxisCount = 0;
}

int CJoystickManager::FindFirstXInputIndex() const
{
    for (int i = 0; i < 4; i++)
    {
        XINPUT_CAPABILITIES Capabilities;
        if (XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &Capabilities) == ERROR_SUCCESS)
            return i;
    }
    return -1;
}

void CJoystickManager::ApplyControllerSelection(bool bReleaseCurrent)
{
    if (bReleaseCurrent)
        ReleaseDirectInputDevice();

    m_bXInputDeviceAttached = false;
    m_bUseXInput = false;
    m_bPreferredInstanceValid = false;
    m_iXInputUserIndex = 0;

    SParsedControllerId parsed = ParseControllerId(m_strSelectedControllerId);

    if (parsed.type == SParsedControllerId::XInput)
    {
        m_bUseXInput = true;
        m_iXInputUserIndex = parsed.iXInputIndex;
        return;
    }

    if (parsed.type == SParsedControllerId::DirectInput && parsed.bHasGuid)
    {
        m_bPreferredInstanceValid = true;
        m_PreferredInstanceGuid = parsed.guidInstance;
        if (bReleaseCurrent)
            StartDirectInputScan();
        return;
    }

    int iXInputIndex = FindFirstXInputIndex();
    if (iXInputIndex >= 0)
    {
        m_bUseXInput = true;
        m_iXInputUserIndex = iXInputIndex;
        return;
    }

    if (bReleaseCurrent)
        StartDirectInputScan();
}

void CJoystickManager::RefreshDirectInputDeviceList()
{
    // EnumDevices races the background scan's CreateDevice/EnumDevices on the same IDirectInput8.
    if (!g_pDirectInput8 || m_bScanRunning)
        return;

    m_ListedDInputDevices.clear();
    g_pDirectInput8->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksListCallback, &m_ListedDInputDevices, DIEDFL_ATTACHEDONLY);
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager Bindings
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
// ToString
//
// Text versions of some enums.
//
///////////////////////////////////////////////////////////////
static string ToString(eJoy value)
{
    if (value == eJoyX)
        return "X";
    if (value == eJoyY)
        return "Y";
    if (value == eJoyZ)
        return "Z";
    if (value == eJoyRx)
        return "RX";
    if (value == eJoyRy)
        return "RY";
    if (value == eJoyRz)
        return "RZ";
    if (value == eJoyS1)
        return "Sld";
    return "unknown";
}

static string ToString(eStick value)
{
    if (value == eLeftStickX)
        return "LeftStickX";
    if (value == eLeftStickY)
        return "LeftStickY";
    if (value == eRightStickX)
        return "RightStickX";
    if (value == eRightStickY)
        return "RightStickY";
    if (value == eAccelerate)
        return _("Accelerate Axis");
    if (value == eBrake)
        return _("Brake Axis");
    return "unknown";
}

static string ToString(eDir value)
{
    if (value == eDirNeg)
        return "-";
    if (value == eDirPos)
        return "+";
    if (value == eDirNegToPos)
        return " ";
    return "unknown";
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetOutputCount
//
// Get number of output(GTA Game) axes.
//
///////////////////////////////////////////////////////////////
int CJoystickManager::GetOutputCount()
{
    return NUMELMS(m_currentMapping);
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetOutputName
//
// Get text name of an output(GTA Game) axis.
//
///////////////////////////////////////////////////////////////
string CJoystickManager::GetOutputName(int iOutputIndex)
{
    if (!VALID_INDEX_FOR(m_currentMapping, iOutputIndex))
        return "";

    const SMappingLine& line = m_currentMapping[iOutputIndex];

    string strStickName = ToString(line.OutputAxisIndex);
    string strDirName = ToString(line.OutputAxisDir);

    if (line.OutputAxisIndex == eAccelerate || line.OutputAxisIndex == eBrake)
        return strStickName;

    return strStickName + string(" ") + strDirName;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetOutputInputName
//
// Get text name of the input(physical) axis mapped to this output(GTA Game) axis.
//
///////////////////////////////////////////////////////////////
string CJoystickManager::GetOutputInputName(int iOutputIndex)
{
    if (!VALID_INDEX_FOR(m_currentMapping, iOutputIndex))
        return "";

    const SMappingLine& line = m_currentMapping[iOutputIndex];

    if (!line.bEnabled)
        return "";

    string strJoyName = ToString(line.SourceAxisIndex);
    string strDirName = ToString(line.SourceAxisDir);

    return strJoyName + string(" ") + strDirName;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::BindNextUsedAxisToOutput
//
// Axis capture.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::BindNextUsedAxisToOutput(int iOutputIndex)
{
    if (!IsJoypadValid())
        return false;

    if (!VALID_INDEX_FOR(m_currentMapping, iOutputIndex))
        return false;

    // Cancel any previous
    m_bCaptureAxis = false;

    // Get current state to compare changes
    ReadCurrentState();
    m_PreBindJoystickState = m_JoystickState;

    // Flag capture start
    m_bCaptureAxis = true;
    m_iCaptureOutputIndex = iOutputIndex;

    return true;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::IsAxisBindComplete
//
// Axis capture.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::IsAxisBindComplete()
{
    return !m_bCaptureAxis;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::IsCapturingAxis
//
// Axis capture.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::IsCapturingAxis()
{
    return m_bCaptureAxis;
}

///////////////////////////////////////////////////////////////
//
// CJoystickManager::CancelCaptureAxis
//
// Axis capture.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::CancelCaptureAxis(bool bClear)
{
    m_bCaptureAxis = false;
    if (bClear)
    {
        // Clear the mapping
        m_currentMapping[m_iCaptureOutputIndex].bEnabled = false;
        m_SettingsRevision++;
    }
}
