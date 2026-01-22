/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CJoystickManager.h
 *  PURPOSE:     Header file for the joystick manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CJoystickManagerInterface
{
public:
    virtual ~CJoystickManagerInterface() {}

    virtual void OnSetDataFormat(IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a) = 0;
    virtual void RemoveDevice(IDirectInputDevice8A* pDevice) = 0;
    virtual void DoPulse() = 0;
    virtual void ApplyAxes(class CControllerState& cs, bool bInVehicle) = 0;

    // Status
    virtual bool IsJoypadConnected() = 0;

    // Settings
    virtual std::string GetControllerName() = 0;
    virtual int         GetDeadZone() = 0;
    virtual int         GetSaturation() = 0;
    virtual void        SetDeadZone(int iDeadZone) = 0;
    virtual void        SetSaturation(int iSaturation) = 0;
    virtual int         GetSettingsRevision() = 0;
    virtual void        SetDefaults() = 0;
    virtual bool        SaveToXML() = 0;

    // Binding
    virtual int         GetOutputCount() = 0;
    virtual std::string GetOutputName(int iOutputIndex) = 0;
    virtual std::string GetOutputInputName(int iOutputIndex) = 0;
    virtual bool        BindNextUsedAxisToOutput(int iOutputIndex) = 0;
    virtual bool        IsAxisBindComplete() = 0;
    virtual bool        IsCapturingAxis() = 0;
    virtual void        CancelCaptureAxis(bool bClear) = 0;
};

CJoystickManagerInterface* NewJoystickManager();
CJoystickManagerInterface* GetJoystickManager();

// Custom VK_ codes for the joystick buttons - Starts at VK_JOY(1)
#define VK_JOY(x)       ( 0x100+(x) )
// Custom VK_ codes for the joystick pov hat - Starts at VK_POV(1)
#define VK_POV(x)       ( 0x150+(x) )
// Custom VK_ codes for the joystick axes - Starts at VK_AXIS(1)
#define VK_AXIS(x)      ( 0x160+(x) )

// Unique numbers for the joystick buttons - Used to identify buttons in the GTA settings
#define GTA_KEY_JOY(x)  ( 500+(x) )
