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

//////////////////////////////////////////////////////////
//
// NOTE:
//
//    GetBindableFromKey in
//    MTA10\mods\deathmatch\logic\CScriptKeyBinds.cpp and
//    MTA10_Server\mods\deathmatch\logic\CKeyBinds.cpp
//    does not know about the joystick buttons. The implications of which
//    I'm not sure of. If it just means the script can't bind the joystick
//    buttons, then that maybe a good thing, or not
//
//   TO DO.
//          1. Axis config              - Done
//          2. Them thar script binds
//          3. POV hat as buttons       - Done
//          4. Axis switch off
//          5. Axis as buttons
//
//
//////////////////////////////////////////////////////////

#include "StdInc.h"
#include <game/CPad.h>

using std::string;

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


static SString GUIDToString ( const GUID& g )
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
    virtual void        OnSetDataFormat             ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a );
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
    virtual void        LoadDefaultConfig           ( void );
    virtual bool        SaveConfig                  ( void );

    // Binding
    virtual int         GetOutputCount              ( void );
    virtual string      GetOutputName               ( int iOutputIndex );
    virtual string      GetOutputInputName          ( int iOutputIndex );
    virtual bool        BindNextUsedAxisToOutput    ( int iOutputIndex );
    virtual bool        IsAxisBindComplete          ( void );
    virtual bool        IsCapturingAxis             ( void );
    virtual void        CancelCaptureAxis           ( bool bClearBinding );

    // CJoystickManager methods
    BOOL                DoEnumObjectsCallback       ( const DIDEVICEOBJECTINSTANCE* pdidoi );
private:
    bool                IsJoypadValid               ( void );
    void                EnumAxes                    ( void );
    void                ReadCurrentState            ( void );
    CXMLNode*           GetConfigNode               ( bool bCreateIfRequired );
    bool                LoadConfig                  ( void );

    int                     m_SettingsRevision;
    SInputDeviceInfo        m_DevInfo;
    SJoystickState          m_JoystickState;
    SMappingLine            m_currentMapping[10];

    // Used during axis binding
    bool                    m_bCaptureAxis;
    int                     m_iCaptureOutputIndex;
    SJoystickState          m_PreBindJoystickState;
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
    LoadDefaultConfig();
}


CJoystickManager::~CJoystickManager ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CJoystickManager::OnSetDataFormat
//
// Figure out device type by intercepting and looking at the SetDataFormat parameter.
// Comparing dwNumObjs seems to work. Could do with a more bullet proof way of determining the device type.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::OnSetDataFormat ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a )
{
    if ( a->dwNumObjs == c_dfDIJoystick2.dwNumObjs )
    {
        if ( !m_DevInfo.pDevice )
        {
            // This is the first Joystick2 device
            m_DevInfo.pDevice = pDevice;
        }
    }
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
    // For axes that are found, do things
    if ( pdidoi->dwType & DIDFT_AXIS )
    {
        // Set the range for the axis
        DIPROPRANGE range;
        range.diph.dwSize = sizeof ( DIPROPRANGE );
        range.diph.dwHeaderSize = sizeof ( DIPROPHEADER );
        range.diph.dwHow = DIPH_BYID;
        range.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
        range.lMin = -2000;
        range.lMax = +2000;

        if ( FAILED ( m_DevInfo.pDevice->SetProperty ( DIPROP_RANGE, &range.diph ) ) )
            return DIENUM_STOP;


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

            m_DevInfo.iAxisCount++;
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
        if ( !LoadConfig () )
        {
            LoadDefaultConfig();
        }
    }

    m_DevInfo.bDoneEnumAxes = true;
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

    if ( !m_DevInfo.pDevice )
        return;

    // Enumerate axes if not done yet
    if ( !m_DevInfo.bDoneEnumAxes )
    {
        EnumAxes ();
    }

    // Try to poll
    if ( FAILED ( m_DevInfo.pDevice->Poll() ) )
    {
        //HRESULT hr =
        m_DevInfo.pDevice->Acquire ();
        //CCore::GetSingleton ().GetConsole ()->Printf( "Joystick Poll failed, Acquire result:%d", hr );
        return;
    }


    DIJOYSTATE2 js;           // DInput joystick state 

    if ( SUCCEEDED ( m_DevInfo.pDevice->GetDeviceState ( sizeof( DIJOYSTATE2 ), &js ) ) )
    {
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
            }
        }

        // Read POV
        for ( int i = 0; i < 4 ; i++ )
            m_JoystickState.rgdwPOV[i] = js.rgdwPOV[i];

        // Read buttons
        for ( int i = 0; i < 32 ; i++ )
            m_JoystickState.rgbButtons[i] = js.rgbButtons[i];

    }
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
// CJoystickManager::LoadDefaultConfig
//
// Set the default axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
void CJoystickManager::LoadDefaultConfig ( void )
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

    memset( m_currentMapping, 0, sizeof(m_currentMapping) );


    // Select defaultMapping to use

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
// CJoystickManager::LoadConfig
//
// Load axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::LoadConfig ( void )
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
// CJoystickManager::SaveConfig
//
// Save axes mapping for the current joypad.
//
///////////////////////////////////////////////////////////////
bool CJoystickManager::SaveConfig ( void )
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

