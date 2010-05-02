/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CJoystickManager.h
*  PURPOSE:     Header file for the joystick manager class
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CJOYSTICKMANAGER_H
#define __CJOYSTICKMANAGER_H


class CJoystickManagerInterface
{
public:
    virtual             ~CJoystickManagerInterface  ( void ) {}

    virtual void        OnSetDataFormat             ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a ) = 0;
    virtual void        RemoveDevice                ( IDirectInputDevice8A* pDevice ) = 0;
    virtual void        DoPulse                     ( void ) = 0;
    virtual void        ApplyAxes                   ( class CControllerState& cs, bool bInVehicle ) = 0;

    // Status
    virtual bool        IsJoypadConnected           ( void ) = 0;

    // Settings
    virtual std::string GetControllerName           ( void ) = 0;
    virtual int         GetDeadZone                 ( void ) = 0;
    virtual int         GetSaturation               ( void ) = 0;
    virtual void        SetDeadZone                 ( int iDeadZone ) = 0;
    virtual void        SetSaturation               ( int iSaturation ) = 0;
    virtual int         GetSettingsRevision         ( void ) = 0;
    virtual void        SetDefaults                 ( void ) = 0;
    virtual bool        SaveToXML                   ( void ) = 0;

    // Binding
    virtual int         GetOutputCount              ( void ) = 0;
    virtual std::string GetOutputName               ( int iOutputIndex ) = 0;
    virtual std::string GetOutputInputName          ( int iOutputIndex ) = 0;
    virtual bool        BindNextUsedAxisToOutput    ( int iOutputIndex ) = 0;
    virtual bool        IsAxisBindComplete          ( void ) = 0;
    virtual bool        IsCapturingAxis             ( void ) = 0;
    virtual void        CancelCaptureAxis           ( bool bClear ) = 0;

};

CJoystickManagerInterface* NewJoystickManager ( void );
CJoystickManagerInterface* GetJoystickManager ( void );

// Custom VK_ codes for the joystick buttons - Starts at VK_JOY(1)
#define VK_JOY(x)       ( 0x100+(x) )
// Custom VK_ codes for the joystick pov hat - Starts at VK_POV(1)
#define VK_POV(x)       ( 0x150+(x) )

// Unique numbers for the joystick buttons - Used to identify buttons in the GTA settings
#define GTA_KEY_JOY(x)  ( 500+(x) )

#endif

