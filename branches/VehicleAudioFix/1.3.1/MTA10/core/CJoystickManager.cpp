/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CJoystickManager.cpp
*  PURPOSE:     Joystick related operations
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CPad.h>
#include "XInput.h"
#include <dinputd.h>

using std::string;

extern IDirectInput8* g_pDirectInput8;

//////////////////////////////////////////////////////////
//
// Helper stuff
//

#ifndef NUMELMS     // in DShow.h
    #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#endif

#define VALID_INDEX_FOR( array, index ) \
            ( index >= 0 && index < NUMELMS(array) )

#define ZERO_ON_NEW \
    void* operator new ( size_t size )              { void* ptr = ::operator new(size); memset(ptr,0,size); return ptr; } \
    void* operator new ( size_t size, void* where ) { memset(where,0,size); return where; }


SString GUIDToString ( const GUID& g )
{
    return SString (
                    "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                    g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2],
                    g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7] );
}

DEFINE_GUID(GUID_Xbox360Controller,   0x028E045E,0x0000,0x0000,0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44);


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
    eJoy    SourceAxisIndex;    // 0 - 7
    eDir    SourceAxisDir;      // 0 - 1
    eStick  OutputAxisIndex;    // 0/1 2/3 4 5
    eDir    OutputAxisDir;      // 0 - 1
    bool    bEnabled;
    int     MaxValue;
};


//////////////////////////////////////////////////////////
//
// DeviceInfo
//
struct SInputDeviceInfo
{
    IDirectInputDevice8A*   pDevice;
    bool                    bDoneEnumAxes;
    int                     iAxisCount;
    int                     iDeadZone;
    int                     iSaturation;
    GUID                    guidProduct;
    string                  strGuid;
    string                  strProductName;

    struct
    {
        bool    bEnabled;
        long    lMax;
        long    lMin;
        DWORD   dwType;
    } axis[7];
};


// Internal state
struct SJoystickState
{
    float   rgfAxis[7];             /* axis positions     -1.f to 1.f       */
    DWORD   rgdwPOV[4];             /* POV directions                       */
    BYTE    rgbButtons[32];         /* 32 buttons                           */
    BYTE    rgbButtonsWas[32];
    BYTE    povButtonsWas[4];
    BYTE    axisButtonsWas[14];     // Axis as buttons
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
                        CJoystickManager            ( void );
                        ~CJoystickManager           ( void );

    // CJoystickManagerInterface methods
    virtual void        OnSetDataFormat             ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a ) {}
    virtual void        RemoveDevice                ( IDirectInputDevice8A* pDevice );
    virtual void        DoPulse                     ( void );
    virtual void        ApplyAxes                   ( CControllerState& cs, bool bInVehicle );

    // Status
    virtual bool        IsJoypadConnected           ( void );

    // Settings
    virtual string      GetControllerName           ( void );
    virtual int         GetDeadZone                 ( void );
    virtual int         GetSaturation               ( void );
    virtual void        SetDeadZone                 ( int iDeadZone );
    virtual void        SetSaturation               ( int iSaturation );
    virtual int         GetSettingsRevision         ( void );
    virtual void        SetDefaults                 ( void );
    virtual bool        SaveToXML                   ( void );

    // Binding
    virtual int         GetOutputCount              ( void );
    virtual string      GetOutputName               ( int iOutputIndex );
    virtual string      GetOutputInputName          ( int iOutputIndex );
    virtual bool        BindNextUsedAxisToOutput    ( int iOutputIndex );
    virtual bool        IsAxisBindComplete          ( void );
    virtual bool        IsCapturingAxis             ( void );
    virtual void        CancelCaptureAxis           ( bool bClearBinding );

    // CJoystickManager methods
    BOOL                DoEnumJoysticksCallback     ( const DIDEVICEINSTANCE* pdidInstance );
    BOOL                DoEnumObjectsCallback       ( const DIDEVICEOBJECTINSTANCE* pdidoi );
private:
    bool                ReadInputSubsystem          ( DIJOYSTATE2& js );
    bool                HandleXInputGetState        ( XINPUT_STATE& XInputState );
    bool                IsXInputDeviceAttached      ( void );
    bool                IsJoypadValid               ( void );
    void                EnumAxes                    ( void );
    void                InitDirectInput             ( void );
    void                ReadCurrentState            ( void );
    CXMLNode*           GetConfigNode               ( bool bCreateIfRequired );
    bool                LoadFromXML                 ( void );

    bool                    m_bDoneInit;
    int                     m_SettingsRevision;
    SInputDeviceInfo        m_DevInfo;
    SJoystickState          m_JoystickState;
    SMappingLine            m_currentMapping[10];
    bool                    m_bUseXInput;
    bool                    m_bXInputDeviceAttached;
    uint                    m_uiXInputReattachDelay;
    CElapsedTime            m_XInputReattachTimer;

    // Used during axis binding
    bool                    m_bCaptureAxis;
    int                     m_iCaptureOutputIndex;
    SJoystickState          m_PreBindJoystickState;

    DIJOYCONFIG*            m_pPreferredJoyCfg;
    bool                    m_bPreferredJoyCfgValid;
};


///////////////////////////////////////////////////////////////
//
// CJoystickManager instantiation
//
///////////////////////////////////////////////////////////////
CJoystickManagerInterface* NewJoystickManager ( void )
{
    return new CJoystickManager ();
}

// This is nice so there
CJoystickManagerInterface* g_pJoystickManager = NULL;

CJoystickManagerInterface* GetJoystickManager ( void )
{
    if ( !g_pJoystickManager )
        g_pJoystickManager = NewJoystickManager ();
    return g_pJoystickManager;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager implementation
//
///////////////////////////////////////////////////////////////
CJoystickManager::CJoystickManager ( void )
{
    // See if we have a XInput compatible device
    XINPUT_CAPABILITIES Capabilities;
    DWORD dwStatus = XInputGetCapabilities( 0, XINPUT_FLAG_GAMEPAD, &Capabilities );
    if ( dwStatus == ERROR_SUCCESS )
    {
        WriteDebugEvent( SString( "XInput device detected. Type:%d SubType:%d Flags:0x%04x"
                                    ,Capabilities.Type
                                    ,Capabilities.SubType
                                    ,Capabilities.Flags
                               ));
        WriteDebugEvent( SString( "XInput - wButtons:0x%04x  bLeftTrigger:0x%02x  bRightTrigger:0x%02x"
                                    ,Capabilities.Gamepad.wButtons
                                    ,Capabilities.Gamepad.bLeftTrigger
                                    ,Capabilities.Gamepad.bRightTrigger
                               ));
        WriteDebugEvent( SString( "XInput - sThumbLX:0x%04x  sThumbLY:0x%04x  sThumbRX:0x%04x  sThumbRY:0x%04x"
                                    ,Capabilities.Gamepad.sThumbLX
                                    ,Capabilities.Gamepad.sThumbLY
                                    ,Capabilities.Gamepad.sThumbRX
                                    ,Capabilities.Gamepad.sThumbRY
                               ));
        WriteDebugEvent( SString( "XInput - wLeftMotorSpeed:0x%04x  wRightMotorSpeed:0x%04x"
                                    ,Capabilities.Vibration.wLeftMotorSpeed
                                    ,Capabilities.Vibration.wRightMotorSpeed
                               ));
        m_bUseXInput = true;
    }

    SetDefaults();
}


CJoystickManager::~CJoystickManager ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager EnumJoysticksCallback
//
// Called once for each enumerated Joystick. If we find one, create a
//       device interface on it so we can play with it.
//
///////////////////////////////////////////////////////////////
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
    // Redir to instance
    return (( CJoystickManager* ) pContext )->DoEnumJoysticksCallback ( pdidInstance );
}

BOOL CJoystickManager::DoEnumJoysticksCallback ( const DIDEVICEINSTANCE* pdidInstance )
{

    WriteDebugEvent( SString( "DInput EnumJoysticksCallback - guidProduct:%s  ProductName:%s"
                                ,*GUIDToString( pdidInstance->guidProduct )
                                ,pdidInstance->tszProductName
                           ));

    // Skip anything other than the perferred Joystick device as defined by the control panel.  
    // Instead you could store all the enumerated Joysticks and let the user pick.
    if( m_bPreferredJoyCfgValid &&
        !IsEqualGUID( pdidInstance->guidInstance, m_pPreferredJoyCfg->guidInstance ) )
        return DIENUM_CONTINUE;

    // Obtain an interface to the enumerated Joystick.
    HRESULT hr = g_pDirectInput8->CreateDevice( pdidInstance->guidInstance, &m_DevInfo.pDevice, NULL );

    // If it failed, then we can't use this Joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED( hr ) )
        return DIENUM_CONTINUE;

    // Stop enumeration. Note: we're just taking the first Joystick we get. You
    // could store all the enumerated Joysticks and let the user pick.
    return DIENUM_STOP;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::RemoveDevice
//
// When notifed that a device is being removed, remove it from our list here.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::RemoveDevice ( IDirectInputDevice8A* pDevice )
{
    if ( m_DevInfo.pDevice == pDevice )
        m_DevInfo.pDevice = NULL;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager EnumObjectsCallback
//
// Enumeration callback used by CJoystickManager::EnumAxes.
//
///////////////////////////////////////////////////////////////
BOOL CALLBACK EnumObjectsCallback ( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
    // Redir to instance
    return (( CJoystickManager* ) pContext )->DoEnumObjectsCallback ( pdidoi );
}


BOOL CJoystickManager::DoEnumObjectsCallback ( const DIDEVICEOBJECTINSTANCE* pdidoi )
{
    SString strGuid = GUIDToString( pdidoi->guidType );
    SString strName = pdidoi->tszName;
    WriteDebugEvent( SString( "DInput - EnumObjectsCallback. dwSize:%d  strGuid:%s  dwOfs:%d  dwType:0x%08x  dwFlags:0x%08x strName:%s"
                                ,pdidoi->dwSize
                                ,*strGuid
                                ,pdidoi->dwOfs
                                ,pdidoi->dwType
                                ,pdidoi->dwFlags
                                ,*strName
                           ));

    // For axes that are found, do things
    if ( pdidoi->dwType & DIDFT_AXIS )
    {
        // Set the range for the axis
        DIPROPRANGE range;
        range.diph.dwSize = sizeof ( DIPROPRANGE );
        range.diph.dwHeaderSize = sizeof ( DIPROPHEADER );
        range.diph.dwHow = DIPH_BYID;
        range.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
        range.lMin = -1000;
        range.lMax = +1000;

        if ( FAILED ( m_DevInfo.pDevice->SetProperty ( DIPROP_RANGE, &range.diph ) ) )
        {
            WriteDebugEvent( SStringX( "                    Failed to set DIPROP_RANGE" ));
            return DIENUM_CONTINUE;
        }

        if ( FAILED ( m_DevInfo.pDevice->GetProperty ( DIPROP_RANGE, &range.diph ) ) )
        {
            WriteDebugEvent( SStringX( "                    Failed to get DIPROP_RANGE" ));
            return DIENUM_CONTINUE;
        }

        // Remove Deadzone and Saturation
        DIPROPDWORD dead,
                    sat;

        dead.diph.dwSize = sizeof dead;
        dead.diph.dwHeaderSize = sizeof dead.diph;
        dead.diph.dwHow = DIPH_BYID;
        dead.diph.dwObj = pdidoi->dwType;
        dead.dwData = 0;        // No Deadzone

        sat = dead;
        sat.dwData = 10000;    // No Saturation

        m_DevInfo.pDevice->SetProperty ( DIPROP_DEADZONE, &dead.diph );
        m_DevInfo.pDevice->SetProperty ( DIPROP_SATURATION, &sat.diph );


        // Figure out the axis index
        int axisIndex = -1;

        if ( pdidoi->guidType == GUID_XAxis )    axisIndex = 0;
        if ( pdidoi->guidType == GUID_YAxis )    axisIndex = 1;
        if ( pdidoi->guidType == GUID_ZAxis )    axisIndex = 2;
        if ( pdidoi->guidType == GUID_RxAxis )   axisIndex = 3;
        if ( pdidoi->guidType == GUID_RyAxis )   axisIndex = 4;
        if ( pdidoi->guidType == GUID_RzAxis )   axisIndex = 5;
        if ( pdidoi->guidType == GUID_Slider )   axisIndex = 6;

        // Save the range and the axis index
        if ( axisIndex >= 0  &&  axisIndex < NUMELMS ( m_DevInfo.axis )  &&  range.lMin < range.lMax )
        {
            m_DevInfo.axis[axisIndex].lMin      = range.lMin;
            m_DevInfo.axis[axisIndex].lMax      = range.lMax;
            m_DevInfo.axis[axisIndex].bEnabled  = true;
            m_DevInfo.axis[axisIndex].dwType    = pdidoi->dwType;

            m_DevInfo.iAxisCount++;
            WriteDebugEvent( SString( "                    Added axis index %d. lMin:%d lMax:%d (iAxisCount:%d)", axisIndex, range.lMin, range.lMax, m_DevInfo.iAxisCount ));
        }
        else
        {
            WriteDebugEvent( SStringX( "                 Failed to recognise axis" ));
        }

#ifdef MTA_DEBUG
#if 0
        if ( CCore::GetSingleton ().GetConsole () )
            CCore::GetSingleton ().GetConsole ()->Printf(
                            "%p  dwHow:%d  dwObj:%d  guid:%x  index:%d  lMin:%d  lMax:%d"
                            ,m_DevInfo.pDevice
                            ,range.diph.dwHow            
                            ,range.diph.dwObj            
                            ,pdidoi->guidType.Data1            
                            ,axisIndex            
                            ,range.lMin            
                            ,range.lMax            
                            );

#endif
#endif
    }

    return DIENUM_CONTINUE;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::EnumAxes
//
// Starts the enumeration of the joystick axes.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::EnumAxes ( void )
{
    if ( !m_DevInfo.pDevice )
        return;

    // Enumerate the joystick objects. The callback function ..blah..blah..
    // values property ..blah..blah.. discovered axes ..blah..blah..
    if ( FAILED( m_DevInfo.pDevice->EnumObjects ( EnumObjectsCallback, ( VOID* )this, DIDFT_ALL ) ) )
    {
        WriteDebugEvent ( "CJoystickManager EnumObjects failed" );
    }

    // Get device id and load config for it
    DIDEVICEINSTANCE didi;
    didi.dwSize = sizeof didi;

    if ( SUCCEEDED( m_DevInfo.pDevice->GetDeviceInfo ( &didi ) ) )
    {
        m_DevInfo.guidProduct = didi.guidProduct;
        m_DevInfo.strProductName = didi.tszProductName;
        m_DevInfo.strGuid = GUIDToString ( m_DevInfo.guidProduct );
        if ( !LoadFromXML () )
        {
            SetDefaults();
        }
    }

    m_DevInfo.bDoneEnumAxes = true;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::InitDirectInput
//
// Create a joystick device if possible
//
///////////////////////////////////////////////////////////////
void CJoystickManager::InitDirectInput( void )
{
    if ( m_bUseXInput )
        return;

    DIJOYCONFIG PreferredJoyCfg = {0};
    m_pPreferredJoyCfg = &PreferredJoyCfg;
    m_bPreferredJoyCfgValid = false;

    IDirectInputJoyConfig8* pJoyConfig = NULL;
    if( FAILED( g_pDirectInput8->QueryInterface( IID_IDirectInputJoyConfig8, ( void** )&pJoyConfig ) ) )
    {
        WriteDebugEvent ( "InitDirectInput - QueryInterface IDirectInputJoyConfig8 failed" );
        return;
    }

    PreferredJoyCfg.dwSize = sizeof( PreferredJoyCfg );
    if( SUCCEEDED( pJoyConfig->GetConfig( 0, &PreferredJoyCfg, DIJC_GUIDINSTANCE ) ) ) // This function is expected to fail if no Joystick is attached
        m_bPreferredJoyCfgValid = true;
    SAFE_RELEASE( pJoyConfig );

    // Look for a simple Joystick we can use for this sample program.
    if( FAILED( g_pDirectInput8->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY ) ) )
    {
        WriteDebugEvent ( "InitDirectInput - EnumDevices failed" );
    }

    // Make sure we got a Joystick
    if( NULL == m_DevInfo.pDevice )
    {
        WriteDebugEvent ( "InitDirectInput - No Joystick found" );
        return;
    }

    // Set the data format to "simple Joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
    if( FAILED( m_DevInfo.pDevice->SetDataFormat( &c_dfDIJoystick2 ) ) )
    {
        WriteDebugEvent ( "InitDirectInput - SetDataFormat failed" );
    }

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
    if( FAILED( m_DevInfo.pDevice->SetCooperativeLevel( g_pCore->GetHookedWindow(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) ) )
    {
        WriteDebugEvent ( "InitDirectInput - SetCooperativeLevel failed" );
    }
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::DoPulse
//
// Updates the joystick state and sends keydown/up messages for any
// buttons state changes.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::DoPulse ( void )
{
    if ( !m_bDoneInit )
    {
        if ( !g_pDirectInput8 )
            return;

        // Init DInput if not done yet
        InitDirectInput();
        m_bDoneInit = true;
    }


    // Stop if no joystick
    if ( !IsJoypadConnected () )
        return;

    //
    // Try to read current state
    //
    ReadCurrentState();


    //
    // Process current state
    //
    HWND hWnd = CMessageLoopHook::GetSingleton ().GetHookedWindowHandle ();

    // Turn the button presses into messages
    for ( int i = 0 ; i < 32 ; i++ )
    {
        const BYTE& NowPress = m_JoystickState.rgbButtons[i];
        BYTE& WasPress       = m_JoystickState.rgbButtonsWas[i];

        // Edge detection
        if ( NowPress != WasPress )
        {
            WasPress = NowPress;

            if ( NowPress )
                SendMessage ( hWnd, WM_KEYDOWN, VK_JOY(i+1), 0x00000001 );
            else
                SendMessage ( hWnd, WM_KEYUP, VK_JOY(i+1), 0xC0000001 );

        }
    }

    // Turn the pov into messages
    {
        BYTE povButtons[4] = { 0, 0, 0, 0 };

        int pov = m_JoystickState.rgdwPOV[0];

        if ( pov != -1 )
        {
            if ( pov > 0-6000 && pov < 0+6000 )
                povButtons[0] = 1;
            if ( pov > 9000-6000 && pov < 9000+6000 )
                povButtons[1] = 1;
            if ( pov > 18000-6000 && pov < 18000+6000 )
                povButtons[2] = 1;
            if ( pov > 27000-6000 && pov < 27000+6000 )
                povButtons[3] = 1;
            if ( pov > 36000-6000 && pov < 36000+6000 )
                povButtons[0] = 1;
        }

        for ( int i = 0 ; i < 4 ; i++ )
        {
            const BYTE& NowPress = povButtons[i];
            BYTE& WasPress       = m_JoystickState.povButtonsWas[i];

            // Edge detection
            if ( NowPress != WasPress )
            {
                WasPress = NowPress;

                if ( NowPress )
                    SendMessage ( hWnd, WM_KEYDOWN, VK_POV(i+1), 0x00000001 );
                else
                    SendMessage ( hWnd, WM_KEYUP, VK_POV(i+1), 0xC0000001 );
            }
        }
    }

    // Turn axis movement into button style messages
    {
        for ( uint i = 0; i < NUMELMS ( m_JoystickState.axisButtonsWas ) ; i++ )
        {
            uint uiAxisIndex = i >> 1;
            uint uiAxisDir = i & 1;

            if ( uiAxisIndex >= NUMELMS( m_JoystickState.rgfAxis ) )
                break;

            BYTE NowPress;
            if ( uiAxisDir )
                NowPress = m_JoystickState.rgfAxis[uiAxisIndex] > 0.75f;
            else
                NowPress = m_JoystickState.rgfAxis[uiAxisIndex] < -0.75f;

            BYTE& WasPress = m_JoystickState.axisButtonsWas[i];

            // Edge detection
            if ( NowPress != WasPress )
            {
                WasPress = NowPress;

                if ( NowPress )
                    SendMessage ( hWnd, WM_KEYDOWN, VK_AXIS(i+1), 0x00000001 );
                else
                    SendMessage ( hWnd, WM_KEYUP, VK_AXIS(i+1), 0xC0000001 );
            }
        }
    }


    // Handle capture and binding
    if ( m_bCaptureAxis )
    {
        // See if any axes have changed to over 0.75
        for ( int i = 0; i < NUMELMS ( m_JoystickState.rgfAxis ) ; i++ )
        {
            if ( fabs ( m_JoystickState.rgfAxis[i] ) > 0.75f )
                if ( fabs ( m_PreBindJoystickState.rgfAxis[i] ) < 0.75f )
                {
                    m_bCaptureAxis = false;
                    // Save the mapping
                    m_currentMapping[m_iCaptureOutputIndex].bEnabled = true;
                    m_currentMapping[m_iCaptureOutputIndex].SourceAxisIndex = (eJoy)i;
                    m_currentMapping[m_iCaptureOutputIndex].SourceAxisDir = m_JoystickState.rgfAxis[i] < 0.f ? eDirNeg : eDirPos;
                    m_SettingsRevision++;
                }
        }        
    }

}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::ReadCurrentState
//
// Puts current state of the joystick into m_JoystickState.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::ReadCurrentState ( void )
{
    // Clear current state
    for ( int i = 0; i < NUMELMS ( m_JoystickState.rgfAxis ) ; i++ )
        m_JoystickState.rgfAxis[i] = 0;
    for ( int i = 0; i < 4 ; i++ )
        m_JoystickState.rgdwPOV[i] = -1;
    for ( int i = 0; i < 32 ; i++ )
        m_JoystickState.rgbButtons[i] = 0;

    DIJOYSTATE2 js;           // DInput joystick state 

    if ( ReadInputSubsystem ( js ) )
    {
        SString strStatus;
        bool bOutputStatus = ( g_pCore->GetDiagnosticDebug () == EDiagnosticDebug::JOYSTICK_0000 ) && !g_pCore->IsConnected();
        if ( bOutputStatus )
        {
            strStatus += SString( "iSaturation:%d iDeadZone:%d\n"
                                ,m_DevInfo.iSaturation
                                ,m_DevInfo.iDeadZone
                            );
        }

        // Read axes
        for ( int a = 0 ; a < NUMELMS ( m_DevInfo.axis )  &&  a < NUMELMS ( m_JoystickState.rgfAxis ); a++ )
        {

            if ( m_DevInfo.axis[a].bEnabled )
            {
                LONG lMin = m_DevInfo.axis[a].lMin;
                LONG lMax = m_DevInfo.axis[a].lMax;
                LONG lSize = lMax - lMin;

                // Re-range: -1.f to 1.f
                // (-min - half(size)) * 2.f / size
                float fResult = ( (&js.lX)[a] - lMin - lSize / 2 ) * 2.f / lSize;

                // Remap test
                //fResult = powf ( fabs( fResult ), 1.5f ) * ( fResult < 0 ? -1.f : 1.f );

                // Apply saturation
                float Saturation = m_DevInfo.iSaturation * (1/100.f);
                fResult += fResult * ( 1 - Saturation );

                // Apply dead zone
                float DeadZone = m_DevInfo.iDeadZone * (1/100.f);

                if ( fResult >= 0.f  )
                    fResult = Max ( 0.f, fResult - DeadZone );
                else
                    fResult = Min ( 0.f, fResult + DeadZone );

                fResult = fResult * ( 1 / ( 1 - DeadZone ) );

                // Clamp range: -1.f to 1.f
                m_JoystickState.rgfAxis[a] = Clamp ( -1.f, fResult, 1.f );

                if ( bOutputStatus )
                {

                    DIPROPRANGE range;
                    range.diph.dwSize = sizeof ( DIPROPRANGE );
                    range.diph.dwHeaderSize = sizeof ( DIPROPHEADER );
                    range.diph.dwHow = DIPH_BYID;
                    range.diph.dwObj = m_DevInfo.axis[a].dwType; // Specify the enumerated axis
                    range.lMin = -2001;
                    range.lMax = +2001;

                    if ( m_DevInfo.pDevice )
                        m_DevInfo.pDevice->GetProperty ( DIPROP_RANGE, &range.diph );

                    // Remove Deadzone and Saturation
                    DIPROPDWORD dead,
                                sat;

                    dead.diph.dwSize = sizeof dead;
                    dead.diph.dwHeaderSize = sizeof dead.diph;
                    dead.diph.dwHow = DIPH_BYID;
                    dead.diph.dwObj = m_DevInfo.axis[a].dwType;
                    dead.dwData = 1;

                    sat = dead;
                    sat.dwData = 9999;

                    if ( m_DevInfo.pDevice )
                    {
                        m_DevInfo.pDevice->GetProperty ( DIPROP_DEADZONE, &dead.diph );
                        m_DevInfo.pDevice->GetProperty ( DIPROP_SATURATION, &sat.diph );
                    }

                    strStatus += SString( "Axis:%d lMin:%d lMax:%d dead:%d sat:%d raw:%d result:%1.4f\n"
                                        ,a
                                        ,range.lMin
                                        ,range.lMax
                                        ,dead.dwData
                                        ,sat.dwData
                                        ,(&js.lX)[a]
                                        ,fResult
                                    );
                }
            }
            else
            {
                if ( bOutputStatus )
                {
                    strStatus += SString( "Axis:%d raw:%d\n"
                                        ,a
                                        ,(&js.lX)[a]
                                    );
                }
            }
        }

        // Read POV
        for ( int i = 0; i < 4 ; i++ )
            m_JoystickState.rgdwPOV[i] = js.rgdwPOV[i];

        // Read buttons
        for ( int i = 0; i < 32 ; i++ )
            m_JoystickState.rgbButtons[i] = js.rgbButtons[i];


        if ( bOutputStatus )
        {
            CGraphicsInterface* pGraphics = CCore::GetSingleton().GetGraphics();
            int x = 20;
            int y = 20;//pGraphics->GetViewportHeight() / 2;
            pGraphics->DrawRectQueued( x, y, 350, 150, 0xaf000000, true );
            pGraphics->DrawTextQueued( x+10, y+10, 0, 0, 0xFFFFFFFF, strStatus, 1, 1, DT_NOCLIP, NULL, true );
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
bool CJoystickManager::ReadInputSubsystem ( DIJOYSTATE2& js )
{
    if ( !m_bUseXInput )
    {
        //
        // DirectInput
        //

        if ( !m_DevInfo.pDevice )
            return false;

        // Enumerate axes if not done yet
        if ( !m_DevInfo.bDoneEnumAxes )
        {
            EnumAxes ();
        }

        // Try to poll
        if ( FAILED ( m_DevInfo.pDevice->Poll() ) )
        {
            m_DevInfo.pDevice->Acquire ();
            return false;
        }

        if ( FAILED ( m_DevInfo.pDevice->GetDeviceState ( sizeof( DIJOYSTATE2 ), &js ) ) )
            return false;
    }
    else
    {
        //
        // XInput
        //

        XINPUT_STATE XInputState;
        if ( !HandleXInputGetState( XInputState ) )
            return false;

        memset( &js, 0, sizeof( DIJOYSTATE2 ) );
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

        bool bPovUp    = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1 : 0;
        bool bPovRight = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1 : 0;
        bool bPovDown  = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1 : 0;
        bool bPovLeft  = XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1 : 0;

        if ( bPovUp )
        {
            if ( bPovRight )        js.rgdwPOV[0] = 0     + 4500; 
            else if ( bPovLeft )    js.rgdwPOV[0] = 36000 - 4500; 
            else                    js.rgdwPOV[0] = 0; 
        }
        else
        if ( bPovDown )
        {
            if ( bPovRight )        js.rgdwPOV[0] = 18000 - 4500; 
            else if ( bPovLeft )    js.rgdwPOV[0] = 18000 + 4500; 
            else                    js.rgdwPOV[0] = 18000; 
        }
        else
        {
            if ( bPovRight )        js.rgdwPOV[0] = 9000; 
            else if ( bPovLeft )    js.rgdwPOV[0] = 27000; 
            else                    js.rgdwPOV[0] = -1; 
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
bool CJoystickManager::HandleXInputGetState ( XINPUT_STATE& XInputState )
{
    if ( !IsXInputDeviceAttached() )
        return false;

    DWORD dwStatus = XInputGetState( 0, &XInputState );

    if ( dwStatus == ERROR_DEVICE_NOT_CONNECTED )
    {
        m_bXInputDeviceAttached = false;
        return false;
    }

    if ( dwStatus != ERROR_SUCCESS )
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
bool CJoystickManager::IsXInputDeviceAttached ( void )
{
    if ( !m_bXInputDeviceAttached )
    {
        // Delay before reattached for performance
        if ( m_XInputReattachTimer.Get() < m_uiXInputReattachDelay )
            return false;
        m_XInputReattachTimer.Reset();
        m_uiXInputReattachDelay = 3000;

        XINPUT_CAPABILITIES Capabilities;
        DWORD dwStatus = XInputGetCapabilities( 0, XINPUT_FLAG_GAMEPAD, &Capabilities );
        if ( dwStatus != ERROR_SUCCESS )
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
        m_DevInfo.axis[7].bEnabled = 0;

        // Compose a guid for saving config
        m_DevInfo.guidProduct.Data1 = 0x12345678;
        m_DevInfo.guidProduct.Data2 = Capabilities.Type;
        m_DevInfo.guidProduct.Data3 = Capabilities.SubType;

        // Compose a product name
        const char* subTypeNames[] = { "Unknown", "Gamepad", "Wheel", "Arcade stick", "Flight stick", "Dance pad", "Guitar", "Drum kit" };
        if ( Capabilities.SubType < NUMELMS( subTypeNames ) )
            m_DevInfo.strProductName = subTypeNames[ Capabilities.SubType ];
        else
            m_DevInfo.strProductName = SString ( "Subtype %d", Capabilities.SubType );

        m_DevInfo.strGuid = GUIDToString ( m_DevInfo.guidProduct );

        // Load config for this guid, or set defaults
        if ( !LoadFromXML () )
        {
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
void CJoystickManager::ApplyAxes ( CControllerState& cs, bool bInVehicle )
{
    if ( !IsJoypadConnected () )
        return;

    // Map each half axis
    for ( int i = 0 ; i < NUMELMS(m_currentMapping) ; i++ )
    {
        const SMappingLine& line = m_currentMapping[i];

        if ( !line.bEnabled )
            continue;

        float value = m_JoystickState.rgfAxis[ line.SourceAxisIndex ];

        if ( line.SourceAxisDir == eDirPos )
            value = Max ( 0.f, value );
        else
            value = -Min ( 0.f, value );

        if ( line.OutputAxisDir == eDirNeg )
            value = -value;

        int iValue = Round ( value * line.MaxValue );

             if ( line.OutputAxisIndex == eLeftStickX )   cs.LeftStickX += iValue;
        else if ( line.OutputAxisIndex == eLeftStickY )   cs.LeftStickY += iValue;
        else if ( line.OutputAxisIndex == eRightStickX )  cs.RightStickX += iValue;
        else if ( line.OutputAxisIndex == eRightStickY )  cs.RightStickY += iValue;
        else if ( line.OutputAxisIndex == eAccelerate && bInVehicle )   cs.ButtonCross += iValue;
        else if ( line.OutputAxisIndex == eBrake && bInVehicle )        cs.ButtonSquare += iValue;
    }


    // Keep everything in range
    cs.LeftStickX  = Clamp < const short > ( -128, cs.LeftStickX, 128 );
    cs.LeftStickY  = Clamp < const short > ( -128, cs.LeftStickY, 128 );
    cs.RightStickX = Clamp < const short > ( -128, cs.RightStickX, 128 );
    cs.RightStickY = Clamp < const short > ( -128, cs.RightStickY, 128 );
    
    cs.ButtonCross  = Clamp < const short > ( 0, cs.ButtonCross, 255 );
    cs.ButtonSquare = Clamp < const short > ( 0, cs.ButtonSquare, 255 );


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

    CCore::GetSingleton ().GetGraphics ()->DrawText ( 20, 150, 0xFFFFFFFF, 1, strBuffer );

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

    CCore::GetSingleton ().GetGraphics ()->DrawText ( 20, 450, 0xFFFFFFFF, 1, strBuffer );


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

    CCore::GetSingleton ().GetGraphics ()->DrawText ( 20, 550, 0xFFFFFFFF, 1, strBuffer );

#endif
#endif

}


///////////////////////////////////////////////////////////////
//
// CJoystickManager Status
//
///////////////////////////////////////////////////////////////

bool CJoystickManager::IsJoypadConnected ( void )
{
    if ( m_bUseXInput )
        return IsXInputDeviceAttached();
    return m_DevInfo.pDevice != NULL;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager Settings
//
///////////////////////////////////////////////////////////////

string CJoystickManager::GetControllerName ( void )
{
    return m_DevInfo.strProductName;
}

int CJoystickManager::GetDeadZone ( void )
{
    return m_DevInfo.iDeadZone;
}

int CJoystickManager::GetSaturation ( void )
{
    return m_DevInfo.iSaturation;
}

void CJoystickManager::SetDeadZone ( int iDeadZone )
{
    m_SettingsRevision++;
    m_DevInfo.iDeadZone = Clamp( 0, iDeadZone, 49 );
}

void CJoystickManager::SetSaturation ( int iSaturation )
{
    m_SettingsRevision++;
    m_DevInfo.iSaturation = Clamp( 51, iSaturation, 100 );
}

int CJoystickManager::GetSettingsRevision ( void )
{
    return m_SettingsRevision;
}


bool CJoystickManager::IsJoypadValid ( void )
{
    if ( m_bUseXInput )
        return IsXInputDeviceAttached();
    return m_DevInfo.pDevice != NULL  &&  m_DevInfo.bDoneEnumAxes  &&  m_DevInfo.strGuid.size () > 0;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetConfigNode
//
// Get the main node for load/saving data for the current joypad.
//
///////////////////////////////////////////////////////////////
CXMLNode* CJoystickManager::GetConfigNode ( bool bCreateIfRequired )
{
    // Get the root node
    CXMLNode *pRoot = CCore::GetSingleton ().GetConfig ();
    if ( !pRoot )
        return NULL;

    // Get the top joypad config node
    CXMLNode* pSectionNode = pRoot->FindSubNode ( CONFIG_NODE_JOYPAD );
    if ( !pSectionNode )
    {
        if ( !bCreateIfRequired )
            return NULL;

        // Non-existant, create a new node
        pSectionNode = pRoot->CreateSubNode ( CONFIG_NODE_JOYPAD );
    }

    // Get the node for this joystick's GUID

    CXMLNode* pItemNode = NULL;
    // Find existing node
    for( int i=0; true; i++ )
    {
        CXMLNode* pNode = pSectionNode->FindSubNode ( "product", i );

        if ( !pNode )
            break;

        CXMLAttributes* pAttributes = &pNode->GetAttributes ();

        if ( CXMLAttribute* pA = pAttributes->Find ( "guid" ) )
        {
            string value = pA->GetValue ();
            if ( value == m_DevInfo.strGuid )
            {
                pItemNode = pNode;
                break;
            }
        }   
    }

    if ( !pItemNode )
    {
        if ( !bCreateIfRequired )
            return NULL;

        // Non-existant, create a new node
        pItemNode = pSectionNode->CreateSubNode ( "product" );

        if ( pItemNode )
        {
            CXMLAttributes* pAttributes = &pItemNode->GetAttributes ();

            CXMLAttribute* pA = NULL;
            pA = pAttributes->Create ( "guid" );
            pA->SetValue ( m_DevInfo.strGuid.c_str () );
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
void CJoystickManager::SetDefaults ( void )
{
    m_SettingsRevision++;

    const SMappingLine defaultMappingStd[] = {
                                eJoyX,  eDirNeg,    eLeftStickX,    eDirNeg,  true,  128,
                                eJoyX,  eDirPos,    eLeftStickX,    eDirPos,  true,  128, 
                                eJoyY,  eDirNeg,    eLeftStickY,    eDirNeg,  true,  128, 
                                eJoyY,  eDirPos,    eLeftStickY,    eDirPos,  true,  128, 
                                eJoyZ,  eDirNeg,    eRightStickX,   eDirNeg,  true,  128, 
                                eJoyZ,  eDirPos,    eRightStickX,   eDirPos,  true,  128, 
                                eJoyRz, eDirNeg,    eRightStickY,   eDirNeg,  true,  128, 
                                eJoyRz, eDirPos,    eRightStickY,   eDirPos,  true,  128, 
                                eJoyZ,  eDirNeg,    eAccelerate,    eDirPos,  false,  255, 
                                eJoyZ,  eDirPos,    eBrake,         eDirPos,  false,  255, 
                            };

    const SMappingLine defaultMapping360[] = {
                                eJoyX,  eDirNeg,    eLeftStickX,    eDirNeg,  true,  128,
                                eJoyX,  eDirPos,    eLeftStickX,    eDirPos,  true,  128, 
                                eJoyY,  eDirNeg,    eLeftStickY,    eDirNeg,  true,  128, 
                                eJoyY,  eDirPos,    eLeftStickY,    eDirPos,  true,  128, 
                                eJoyRx, eDirNeg,    eRightStickX,   eDirNeg,  true,  128, 
                                eJoyRx, eDirPos,    eRightStickX,   eDirPos,  true,  128, 
                                eJoyRy, eDirNeg,    eRightStickY,   eDirNeg,  true,  128, 
                                eJoyRy, eDirPos,    eRightStickY,   eDirPos,  true,  128, 
                                eJoyZ,  eDirNeg,    eAccelerate,    eDirPos,  true,  255, 
                                eJoyZ,  eDirPos,    eBrake,         eDirPos,  true,  255, 
                            };

    const SMappingLine defaultMappingXInput[] = {
                                eJoyX,  eDirNeg,    eLeftStickX,    eDirNeg,  true,  128,
                                eJoyX,  eDirPos,    eLeftStickX,    eDirPos,  true,  128, 
                                eJoyY,  eDirPos,    eLeftStickY,    eDirNeg,  true,  128, 
                                eJoyY,  eDirNeg,    eLeftStickY,    eDirPos,  true,  128, 
                                eJoyRx, eDirNeg,    eRightStickX,   eDirNeg,  true,  128, 
                                eJoyRx, eDirPos,    eRightStickX,   eDirPos,  true,  128, 
                                eJoyRy, eDirPos,    eRightStickY,   eDirNeg,  true,  128, 
                                eJoyRy, eDirNeg,    eRightStickY,   eDirPos,  true,  128, 
                                eJoyRz, eDirPos,    eAccelerate,    eDirPos,  true,  255, 
                                eJoyZ,  eDirPos,    eBrake,         eDirPos,  true,  255, 
                            };

    memset( m_currentMapping, 0, sizeof(m_currentMapping) );

    // Select defaultMapping to use

    if ( m_bUseXInput )
    {
        // If XInput device, then use default XInput mapping
        for ( int i = 0 ; i < NUMELMS(m_currentMapping) ; i++ )
            m_currentMapping[i] = defaultMappingXInput[i];

        m_DevInfo.iDeadZone = 20;
        m_DevInfo.iSaturation = 99;
    }
    else
    if ( m_DevInfo.pDevice && m_DevInfo.iAxisCount == 5 && m_DevInfo.guidProduct == GUID_Xbox360Controller )
    {
        // If GUID matches published 360 controller GUID and device has 5 axes, then use 360 mapping
        for ( int i = 0 ; i < NUMELMS(m_currentMapping) ; i++ )
            m_currentMapping[i] = defaultMapping360[i];

        m_DevInfo.iDeadZone = 18;
        m_DevInfo.iSaturation = 99;
    }
    else
    {
        // Otherwise use generic dual axis mapping
        for ( int i = 0 ; i < NUMELMS(m_currentMapping) ; i++ )
            m_currentMapping[i] = defaultMappingStd[i];

        m_DevInfo.iDeadZone = 12;
        m_DevInfo.iSaturation = 99;
    }
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::LoadFromXML
//
// Load axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::LoadFromXML ( void )
{
    m_SettingsRevision++;

    // Try load
    CXMLNode* pMainNode = GetConfigNode ( false );

    if ( !pMainNode )
        return false;

    int iErrors = 0;

    {
        // Find the 'info' node
        CXMLNode* pNode = pMainNode->FindSubNode ( "info" );

        // If it was found
        if ( pNode )
        {
            CXMLAttributes* pAttributes = &pNode->GetAttributes ();

            CXMLAttribute* pA = NULL;
            if ( pA = pAttributes->Find( "deadzone" ) )
                m_DevInfo.iDeadZone = Clamp ( 0, atoi ( pA->GetValue ().c_str () ), 49 );
            else
                iErrors++;

            if ( pA = pAttributes->Find( "saturation" ) )
                m_DevInfo.iSaturation = Clamp ( 51, atoi ( pA->GetValue ().c_str () ), 100 );
            else
                iErrors++;
        }
        else
            iErrors++;

    }

    // Iterate the binds reading them from the XML tree
    for ( int i = 0 ; i < NUMELMS(m_currentMapping) ; i++ )
    {
        SMappingLine& line = m_currentMapping[i];

        // Find the 'axis' node
        CXMLNode* pNode = pMainNode->FindSubNode( "axis", i );

        // If it was found
        if ( pNode )
        {
            CXMLAttributes* pAttributes = &pNode->GetAttributes ();

            CXMLAttribute* pA = NULL;
            if ( pA = pAttributes->Find( "source_index" ) )
                line.SourceAxisIndex = (eJoy)Clamp < int > ( 0, atoi ( pA->GetValue ().c_str () ), eJoyMax );
            else
                iErrors++;

            if ( pA = pAttributes->Find( "source_dir" ) )
                line.SourceAxisDir = (eDir)Clamp < int > ( 0, atoi ( pA->GetValue ().c_str () ), eDirMax );
            else
                iErrors++;

            if ( pA = pAttributes->Find( "output_index" ) )
                line.OutputAxisIndex = (eStick)Clamp < int > ( 0, atoi ( pA->GetValue ().c_str () ), eStickMax );
            else
                iErrors++;

            if ( pA = pAttributes->Find( "output_dir" ) )
                line.OutputAxisDir = (eDir)Clamp < int > ( 0, atoi ( pA->GetValue ().c_str () ), eDirMax );
            else
                iErrors++;

            if ( pA = pAttributes->Find( "enabled" ) )
                line.bEnabled = atoi ( pA->GetValue ().c_str () ) ? true : false;
            else
                iErrors++;

            if ( pA = pAttributes->Find( "max_value" ) )
                line.MaxValue = atoi ( pA->GetValue ().c_str () );
            else
                iErrors++;

        }
        else
            iErrors++;
    }

    if ( iErrors )
        if ( CCore::GetSingleton ().GetConsole () )
            CCore::GetSingleton ().GetConsole ()->Printf( "Warning: %d errors reading joypad configuration.", iErrors );
        

    return true;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::SaveToXML
//
// Save axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::SaveToXML ( void )
{
    if ( !IsJoypadValid () )
        return false;

    m_SettingsRevision++;

    CXMLNode* pMainNode = GetConfigNode ( true );

    // Add the current settings
    if ( pMainNode )
    {
        // Clear our current bind nodes
        pMainNode->DeleteAllSubNodes ();

        {
            // Create a new 'info' node
            CXMLNode* pNode = pMainNode->CreateSubNode ( "info" );

            // If it was created
            if ( pNode )
            {
                CXMLAttributes* pAttributes = &pNode->GetAttributes ();

                CXMLAttribute* pA = NULL;
                pA = pAttributes->Create ( "deadzone" );
                pA->SetValue ( m_DevInfo.iDeadZone );

                pA = pAttributes->Create ( "saturation" );
                pA->SetValue ( m_DevInfo.iSaturation );

                pA = pAttributes->Create ( "product_name" );
                pA->SetValue ( m_DevInfo.strProductName.c_str () );
            }
        }

        // Iterate the binds adding them to the XML tree
        for ( int i = 0 ; i < NUMELMS(m_currentMapping) ; i++ )
        {
            const SMappingLine& line = m_currentMapping[i];

            // Create the new 'axis' node
            CXMLNode* pNode = pMainNode->CreateSubNode ( "axis" );

            // If it was created
            if ( pNode )
            {
                CXMLAttributes* pAttributes = &pNode->GetAttributes ();

                CXMLAttribute* pA = NULL;
                pA = pAttributes->Create ( "source_index" );
                pA->SetValue ( line.SourceAxisIndex );

                pA = pAttributes->Create ( "source_dir" );
                pA->SetValue ( line.SourceAxisDir );

                pA = pAttributes->Create ( "output_index" );
                pA->SetValue ( line.OutputAxisIndex );

                pA = pAttributes->Create ( "output_dir" );
                pA->SetValue ( line.OutputAxisDir );

                pA = pAttributes->Create ( "enabled" );
                pA->SetValue ( line.bEnabled );

                pA = pAttributes->Create ( "max_value" );
                pA->SetValue ( line.MaxValue );

            }
        }
        return true;
    }
    return false;
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
static string ToString( eJoy value )
{
    if ( value == eJoyX )     return "X";
    if ( value == eJoyY )     return "Y";
    if ( value == eJoyZ )     return "Z";
    if ( value == eJoyRx )    return "RX";
    if ( value == eJoyRy )    return "RY";
    if ( value == eJoyRz )    return "RZ";
    if ( value == eJoyS1 )    return "Sld";
    return "unknown";
}

static string ToString( eStick value )
{
    if ( value == eLeftStickX )   return "LeftStickX";
    if ( value == eLeftStickY )   return "LeftStickY";
    if ( value == eRightStickX )  return "RightStickX";
    if ( value == eRightStickY )  return "RightStickY";
    if ( value == eAccelerate )   return "Accelerate Axis";
    if ( value == eBrake )        return "Brake Axis";
    return "unknown";
}

static string ToString( eDir value )
{
    if ( value == eDirNeg )   return "-";
    if ( value == eDirPos )   return "+";
    return "unknown";
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetOutputCount
//
// Get number of output(GTA Game) axes.
//
///////////////////////////////////////////////////////////////
int CJoystickManager::GetOutputCount ( )
{
    return NUMELMS ( m_currentMapping );
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetOutputName
//
// Get text name of an output(GTA Game) axis.
//
///////////////////////////////////////////////////////////////
string CJoystickManager::GetOutputName ( int iOutputIndex )
{
    if ( !VALID_INDEX_FOR ( m_currentMapping, iOutputIndex ) )
        return "";

    const SMappingLine& line = m_currentMapping[iOutputIndex];

    string strStickName  = ToString ( line.OutputAxisIndex );
    string strDirName  = ToString ( line.OutputAxisDir );

    if ( line.OutputAxisIndex == eAccelerate || line.OutputAxisIndex == eBrake )
        return strStickName;

    return strStickName + string ( " " ) + strDirName;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::GetOutputInputName
//
// Get text name of the input(physical) axis mapped to this output(GTA Game) axis.
//
///////////////////////////////////////////////////////////////
string CJoystickManager::GetOutputInputName ( int iOutputIndex )
{
    if ( !VALID_INDEX_FOR ( m_currentMapping, iOutputIndex ) )
        return "";

    const SMappingLine& line = m_currentMapping[iOutputIndex];

    if ( !line.bEnabled )
        return "";

    string strJoyName  = ToString ( line.SourceAxisIndex );
    string strDirName  = ToString ( line.SourceAxisDir );

    return strJoyName + string ( " " ) + strDirName;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::BindNextUsedAxisToOutput
//
// Axis capture.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::BindNextUsedAxisToOutput ( int iOutputIndex )
{
    if( !IsJoypadValid() )
        return false;

    if ( !VALID_INDEX_FOR ( m_currentMapping, iOutputIndex ) )
        return false;

    // Cancel any previous
    m_bCaptureAxis          = false;

    // Get current state to compare changes
    ReadCurrentState();
    m_PreBindJoystickState  = m_JoystickState;

    // Flag capture start
    m_bCaptureAxis          = true;
    m_iCaptureOutputIndex   = iOutputIndex;

    return true;
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::IsAxisBindComplete
//
// Axis capture.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::IsAxisBindComplete ( void )
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
bool CJoystickManager::IsCapturingAxis ( void )
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
void CJoystickManager::CancelCaptureAxis ( bool bClear )
{
    m_bCaptureAxis = false;
    if ( bClear )
    {
        // Clear the mapping
        m_currentMapping[m_iCaptureOutputIndex].bEnabled = false;
        m_SettingsRevision++;
    }
}

