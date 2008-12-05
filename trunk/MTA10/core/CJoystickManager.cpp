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
//
//////////////////////////////////////////////////////////

#include "StdInc.h"
#include <game/CPad.h>
#include <list>
using namespace std;


//////////////////////////////////////////////////////////
//
// Helper stuff
//

#ifndef NUMELMS     // in DShow.h
    #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#endif


template<class T>
T Min ( const T& a, const T& b )
{
    return a < b ? a : b;
}

template<class T>
T Max ( const T& a, const T& b )
{
    return a > b ? a : b;
}

template<class T>
T Clamp ( const T& min, const T& value, const T& max )
{
    return value < min ? min : value > max ? max : value;
}

int Round ( float value )
{
	return (int)floor(value + 0.5f);
}

//////////////////////////////////////////////////////////


enum eInputDeviceType
{
    INPUT_DEVICE_UNKNOWN,
    INPUT_DEVICE_MOUSE,
    INPUT_DEVICE_MOUSE2,
    INPUT_DEVICE_KEYBOARD,
    INPUT_DEVICE_JOYSTICK,
    INPUT_DEVICE_JOYSTICK2,
};


struct SInputDeviceInfo
{
    IDirectInputDevice8A*   pDevice;
    eInputDeviceType        eType;
    bool                    bDoneEnumAxes;
    int                     iAxisCount;
    GUID                    guidProduct;

    struct
    {
        bool    bEnabled;
        long    lMax;
        long    lMin;
    } axis[8];
};


struct SJoystickState
{
    float   rgfAxis[8];             /* axis positions     -1.f to 1.f       */
    DWORD   rgdwPOV[4];             /* POV directions                       */
    BYTE    rgbButtons[32];         /* 32 buttons                           */
    BYTE    rgbButtonsWas[32];
};


class CJoystickManager : public CJoystickManagerInterface
{
public:
                        CJoystickManager            ( void );
                        ~CJoystickManager           ( void );

    // CJoystickManagerInterface methods
    void                AddDevice                   ( IDirectInputDevice8A* pDevice );
    void                RemoveDevice                ( IDirectInputDevice8A* pDevice );
    void                SetDeviceDataFormat         ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a );
    void                DoPulse                     ( void );
    void                ApplyAxes                   ( CControllerState& cs );

private:
    // CJoystickManager methods
    void                SetDeviceType               ( IDirectInputDevice8A* pDevice, eInputDeviceType eType );
    SInputDeviceInfo*   FindInfo                    ( IDirectInputDevice8A* pDevice );
    void                EnumAxes                    ( SInputDeviceInfo* info );
    void                ReadCurrentState            ( void );

    list < SInputDeviceInfo >   m_pList;
    SJoystickState              m_JoystickState;
};


CJoystickManagerInterface* NewJoystickManager ( void )
{
    return new CJoystickManager ();
}


// Well, this is not nice - Should be moved to CCore if the joystick manager is kept
CJoystickManagerInterface* g_pJoystickManager = NULL;

CJoystickManagerInterface* GetJoystickManager ( void )
{
    if( !g_pJoystickManager )
        g_pJoystickManager = NewJoystickManager ();
    return g_pJoystickManager;
}


//
// CJoystickManager implementation
//
CJoystickManager::CJoystickManager ( void )
{
    memset( &m_JoystickState, 0, sizeof( m_JoystickState ) );
}


CJoystickManager::~CJoystickManager ( void )
{
}


//
// List management
//
void CJoystickManager::AddDevice ( IDirectInputDevice8A* pDevice )
{
    if( FindInfo( pDevice ) )
        return;

    SInputDeviceInfo info;
    memset( &info, 0, sizeof( info ) );    // Another triumph!
    info.pDevice    = pDevice;
    info.eType      = INPUT_DEVICE_UNKNOWN;
    m_pList.push_back ( info );
}


void CJoystickManager::RemoveDevice ( IDirectInputDevice8A* pDevice )
{
    list < SInputDeviceInfo > ::iterator iter = m_pList.begin ();
    for ( ; iter != m_pList.end (); iter++ )
        if ( (*iter).pDevice == pDevice )
            iter = m_pList.erase ( iter );
}


//
// Info getting
//
SInputDeviceInfo* CJoystickManager::FindInfo ( IDirectInputDevice8A* pDevice )
{
    list < SInputDeviceInfo > ::iterator iter = m_pList.begin ();
    for ( ; iter != m_pList.end (); iter++ )
        if ( (*iter).pDevice == pDevice )
            return &(*iter);
    return NULL;
}


//
// Info changing
//
void CJoystickManager::SetDeviceType ( IDirectInputDevice8A* pDevice, eInputDeviceType eType )
{
    if( SInputDeviceInfo* info = FindInfo ( pDevice ) )
        info->eType = eType;
}


// Figure out device type by looking at the SetDataFormat parameter.
// Comparing dwNumObjs seems to work. Could do with a more bullet proof way of determining the device type.
void CJoystickManager::SetDeviceDataFormat ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a )
{
    struct
    {
        const DIDATAFORMAT*    dataFormat;
        eInputDeviceType    type;

    } list[] = {    &c_dfDIMouse,       INPUT_DEVICE_MOUSE,
                    &c_dfDIMouse2,      INPUT_DEVICE_MOUSE2,
                    &c_dfDIKeyboard,    INPUT_DEVICE_KEYBOARD,
                    &c_dfDIJoystick,    INPUT_DEVICE_JOYSTICK,
                    &c_dfDIJoystick2,   INPUT_DEVICE_JOYSTICK2  };

    for ( int i = 0 ; i < NUMELMS ( list ) ; i++ )
    {
        if( list[i].dataFormat->dwNumObjs == a->dwNumObjs )
        {
            SetDeviceType ( pDevice, list[i].type );
        }
    }
}


//
// Enumeration
//
BOOL CALLBACK EnumObjectsCallback ( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
    SInputDeviceInfo* info = ( SInputDeviceInfo* ) pContext;

    // For axes that are returned, do things
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        // Set the range for the axis
        DIPROPRANGE range;
        range.diph.dwSize = sizeof ( DIPROPRANGE );
        range.diph.dwHeaderSize = sizeof ( DIPROPHEADER );
        range.diph.dwHow = DIPH_BYID;
        range.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
        range.lMin = -2000;
        range.lMax = +2000;

        if( FAILED ( info->pDevice->SetProperty( DIPROP_RANGE, &range.diph ) ) )
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

        info->pDevice->SetProperty ( DIPROP_DEADZONE, &dead.diph );
        info->pDevice->SetProperty ( DIPROP_SATURATION, &sat.diph );


        // Figure out the axis index
        int axisIndex = -1;

        if( pdidoi->guidType == GUID_XAxis )    axisIndex = 0;
        if( pdidoi->guidType == GUID_YAxis )    axisIndex = 1;
        if( pdidoi->guidType == GUID_ZAxis )    axisIndex = 2;
        if( pdidoi->guidType == GUID_RxAxis )   axisIndex = 3;
        if( pdidoi->guidType == GUID_RyAxis )   axisIndex = 4;
        if( pdidoi->guidType == GUID_RzAxis )   axisIndex = 5;

        // Save the range and the axis index
        if( axisIndex >= 0  &&  axisIndex < NUMELMS ( info->axis )  &&  range.lMin < range.lMax )
        {
            info->axis[axisIndex].lMin      = range.lMin;
            info->axis[axisIndex].lMax      = range.lMax;
            info->axis[axisIndex].bEnabled  = true;

            info->iAxisCount++;
       }

#ifdef MTA_DEBUG
        CCore::GetSingleton ().GetConsole ()->Printf(
                        "%p  dwHow:%d  dwObj:%d  guid:%x  index:%d  lMin:%d  lMax:%d"
                        ,info->pDevice
                        ,range.diph.dwHow            
                        ,range.diph.dwObj            
                        ,pdidoi->guidType.Data1            
                        ,axisIndex            
                        ,range.lMin            
                        ,range.lMax            
                        );

#endif
    }

    return DIENUM_CONTINUE;
}



void CJoystickManager::EnumAxes ( SInputDeviceInfo* info )
{
    // Enumerate the joystick objects. The callback function ..blah..blah..
    // values property ..blah..blah.. discovered axes ..blah..blah..
    if( FAILED( info->pDevice->EnumObjects ( EnumObjectsCallback, ( VOID* )info, DIDFT_ALL ) ) )
    {
        WriteDebugEvent ( "CJoystickManager EnumObjects failed" );
    }

    // Save device GUID
    DIDEVICEINSTANCE didi;
    didi.dwSize = sizeof didi;

    if( SUCCEEDED( info->pDevice->GetDeviceInfo ( &didi ) ) )
        info->guidProduct = didi.guidProduct;
}


//
// The stuff
//
void CJoystickManager::DoPulse ( void )
{

    //
    // Clear current state
    //
    for ( int i = 0; i < NUMELMS ( m_JoystickState.rgfAxis ) ; i++ )
        m_JoystickState.rgfAxis[i] = 0;
    for ( int i = 0; i < 4 ; i++ )
        m_JoystickState.rgdwPOV[i] = 0;
    for ( int i = 0; i < 32 ; i++ )
        m_JoystickState.rgbButtons[i] = 0;


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
        if( NowPress != WasPress )
        {
            WasPress = NowPress;

            if( NowPress )
                SendMessage ( hWnd, WM_KEYDOWN, VK_JOY(i+1), 0x00000001 );
            else
                SendMessage ( hWnd, WM_KEYUP, VK_JOY(i+1), 0xC0000001 );

        }
    }
}


void CJoystickManager::ReadCurrentState ( void )
{
    // Find a joystick
    list < SInputDeviceInfo > ::iterator iter = m_pList.begin ();
    for ( ; iter != m_pList.end (); iter++ )
    {
        SInputDeviceInfo& info = (*iter);

        if ( info.eType == INPUT_DEVICE_JOYSTICK2 )
        {

            // Enumerate axes if not done yet
            if( !info.bDoneEnumAxes )
            {
                EnumAxes ( &info );
                info.bDoneEnumAxes = true;
            }

            // Try to poll
            if( FAILED ( info.pDevice->Poll() ) )
            {
                HRESULT hr = info.pDevice->Acquire ();
                //CCore::GetSingleton ().GetConsole ()->Printf( "Joystick Poll failed, Acquire result:%d", hr );
                break;
            }


            DIJOYSTATE2 js;           // DInput joystick state 

            HRESULT hr;

            // Read the state
            if( info.eType == INPUT_DEVICE_JOYSTICK )
                hr = info.pDevice->GetDeviceState ( sizeof( DIJOYSTATE ), &js );
            else
                hr = info.pDevice->GetDeviceState ( sizeof( DIJOYSTATE2 ), &js );


            if( SUCCEEDED ( hr ) )
            {
                // Read axes
                for ( int a = 0 ; a < 8  &&  a < NUMELMS ( m_JoystickState.rgfAxis ); a++ )
                {

                    if( info.axis[a].bEnabled )
                    {
                        LONG lMin = info.axis[a].lMin;
                        LONG lMax = info.axis[a].lMax;
                        LONG lSize = lMax - lMin;

                        // Re-range: -1.f to 1.f
                        // (-min - half(size)) * 2.f / size
                        float fResult = ( (&js.lX)[a] - lMin - lSize / 2 ) * 2.f / lSize;

                        // Apply saturation
                        float Saturation = 0.995f;
                        fResult += fResult * (1-Saturation);

                        // Apply dead zone
                        float DeadZone = 0.25f;

                        if( fResult >= 0.f  )
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

                // Stop after first successful joystick read
                return;
            }
        }
    }
}



DEFINE_GUID(GUID_Xbox360Controller,   0x028E045E,0x0000,0x0000,0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44);

// Work In Progress!
// Needs some sort of user access to the axis mapping
void CJoystickManager::ApplyAxes ( CControllerState& cs )
{

    enum eJoy
    {
        eJoyX,
        eJoyY,
        eJoyZ,
        eJoyRX,
        eJoyRY,
        eJoyRZ,
    };

    enum eDir
    {
        eDirNeg,
        eDirPos,
    };

    enum eStick
    {
        eLeftStickX,
        eLeftStickY,
        eRightStickX,
        eRightStickY,
        eAccelerate,
        eBrake,
    };


    struct SMappingLine
    {
        eJoy    FromAxisIndex;  // 0 - 7
        eDir    FromAxisDir;    // 0 - 1
        eStick  ToAxisIndex;    // 0/1 2/3 4
        eDir    ToAxisDir;      // 0 - 1
        int     MaxValue;       // 127 or 255
    };


    // Default layout - maps most two axis joysticks - some axes might need inverting
    SMappingLine mappingStd[] = {
                                eJoyX,  eDirNeg, eLeftStickX,  eDirNeg, 128,
                                eJoyX,  eDirPos, eLeftStickX,  eDirPos, 128,
                                eJoyY,  eDirNeg, eLeftStickY,  eDirNeg, 128,
                                eJoyY,  eDirPos, eLeftStickY,  eDirPos, 128,
                                eJoyRX, eDirNeg, eRightStickX, eDirNeg, 128,
                                eJoyRX, eDirPos, eRightStickX, eDirPos, 128,
                                eJoyRY, eDirNeg, eRightStickY, eDirNeg, 128,
                                eJoyRY, eDirPos, eRightStickY, eDirPos, 128,
                                eJoyZ,  eDirNeg, eRightStickX, eDirNeg, 128,
                                eJoyZ,  eDirPos, eRightStickX, eDirPos, 128,
                                eJoyRZ, eDirNeg, eRightStickY, eDirNeg, 128,
                                eJoyRZ, eDirPos, eRightStickY, eDirPos, 128,
                            };


    // Custom layout for 360 controller - RightStickY might need inverting
    SMappingLine mapping360[] = {
                                eJoyX,  eDirNeg, eLeftStickX,  eDirNeg, 128,
                                eJoyX,  eDirPos, eLeftStickX,  eDirPos, 128,
                                eJoyY,  eDirNeg, eLeftStickY,  eDirNeg, 128,
                                eJoyY,  eDirPos, eLeftStickY,  eDirPos, 128,
                                eJoyRX, eDirNeg, eRightStickX, eDirNeg, 128,
                                eJoyRX, eDirPos, eRightStickX, eDirPos, 128,
                                eJoyRY, eDirNeg, eRightStickY, eDirNeg, 128,
                                eJoyRY, eDirPos, eRightStickY, eDirPos, 128,
                                eJoyZ,  eDirNeg, eAccelerate,  eDirPos, 255,
                                eJoyZ,  eDirPos, eBrake,       eDirPos, 255,
                            };


    SMappingLine* mappingLines  = mappingStd;
    int mappingLinesNum         = NUMELMS ( mappingStd );

    // Temp hack to see if using 360 controller
    list < SInputDeviceInfo > ::iterator iter = m_pList.begin ();
    for ( ; iter != m_pList.end (); iter++ )
        if ( (*iter).iAxisCount == 5 && (*iter).guidProduct == GUID_Xbox360Controller )
        {
            // If GUID matches published 360 controller GUID and device has 5 axes, then use 360 mapping
            mappingLines     = mapping360;
            mappingLinesNum  = NUMELMS ( mapping360 );
        }


    // Map each half axis
    for ( int i = 0 ; i < mappingLinesNum ; i++ )
    {
        SMappingLine& line = mappingLines[i];

        float value = m_JoystickState.rgfAxis[ line.FromAxisIndex ];

        if( line.FromAxisDir == eDirPos )
            value = Max ( 0.f, value );
        else
            value = -Min ( 0.f, value );

        if( line.ToAxisDir == eDirNeg )
            value = -value;

        int iValue = Round ( value * line.MaxValue );

             if( line.ToAxisIndex == eLeftStickX )   cs.LeftStickX += iValue;
        else if( line.ToAxisIndex == eLeftStickY )   cs.LeftStickY += iValue;
        else if( line.ToAxisIndex == eRightStickX )  cs.RightStickX += iValue;
        else if( line.ToAxisIndex == eRightStickY )  cs.RightStickY += iValue;
        else if( line.ToAxisIndex == eAccelerate )   cs.ButtonCross += iValue;
        else if( line.ToAxisIndex == eBrake )        cs.ButtonSquare += iValue;
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

    char szBuffer [256];
    _snprintf ( szBuffer, 256, "LeftShoulder1: %u\n"
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

    CCore::GetSingleton ().GetGraphics ()->DrawText ( 20, 150, 0xFFFFFFFF, 1, szBuffer );

    _snprintf ( szBuffer, 256, "VehicleMouseLook: %u\n"
                                "LeftStickX: %u\n"
                                "LeftStickY: %u\n"
                                "RightStickX: %u\n"
                                "RightStickY: %u",
                                cs.m_bVehicleMouseLook,
                                cs.LeftStickX,
                                cs.LeftStickY,
                                cs.RightStickX,
                                cs.RightStickY );

    CCore::GetSingleton ().GetGraphics ()->DrawText ( 20, 450, 0xFFFFFFFF, 1, szBuffer );


    _snprintf ( szBuffer, 256,
                                "Axis1X: %5.8f\n"
                                "Axis1Y: %5.8f\n"
                                "Axis1Z: %5.8f\n"
                                "Axis2Rx: %5.8f\n"
                                "Axis2Ry: %5.8f\n"
                                "Axis2Rz: %5.8f\n",
                                m_JoystickState.rgfAxis[0],
                                m_JoystickState.rgfAxis[1],
                                m_JoystickState.rgfAxis[2],
                                m_JoystickState.rgfAxis[3],
                                m_JoystickState.rgfAxis[4],
                                m_JoystickState.rgfAxis[5],
                                m_JoystickState.rgfAxis[6]
                            );

    CCore::GetSingleton ().GetGraphics ()->DrawText ( 20, 550, 0xFFFFFFFF, 1, szBuffer );

#endif
#endif

}




