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
    virtual void        AddDevice                   ( IDirectInputDevice8A* pDevice ) = 0;
    virtual void        RemoveDevice                ( IDirectInputDevice8A* pDevice ) = 0;
    virtual void        SetDeviceDataFormat         ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a ) = 0;

    virtual void        DoPulse                     ( void ) = 0;
    virtual void        ApplyAxes                   ( class CControllerState& cs ) = 0;
};

CJoystickManagerInterface* NewJoystickManager ( void );
CJoystickManagerInterface* GetJoystickManager ( void );

// Custom VK_ codes for the joystick buttons - Starts at VK_JOY(1)
#define VK_JOY(x)       ( 0x100+(x) )

// Unique numbers for the joystick buttons 
#define GTA_KEY_JOY(x)  ( 500+(x) )

#endif

