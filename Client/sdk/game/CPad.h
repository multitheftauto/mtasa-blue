/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPad.h
 *  PURPOSE:     Controller pad interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Set values to 128 unless otherwise specified
class CControllerState
{
public:
    signed short LeftStickX;             // move/steer left (-128?)/right (+128)
    signed short LeftStickY;             // move back(+128)/forwards(-128?)
    signed short RightStickX;            // numpad 6(+128)/numpad 4(-128?)
    signed short RightStickY;

    signed short LeftShoulder1;
    signed short LeftShoulder2;
    signed short RightShoulder1;            // target / hand brake
    signed short RightShoulder2;

    signed short DPadUp;              // radio change up
    signed short DPadDown;            // radio change down
    signed short DPadLeft;
    signed short DPadRight;

    signed short Start;
    signed short Select;

    signed short ButtonSquare;              // jump / reverse
    signed short ButtonTriangle;            // get in/out
    signed short ButtonCross;               // sprint / accelerate
    signed short ButtonCircle;              // fire

    signed short ShockButtonL;
    signed short ShockButtonR;            // look behind

    signed short m_bChatIndicated;
    signed short m_bPedWalk;
    signed short m_bVehicleMouseLook;
    signed short m_bRadioTrackSkip;

    CControllerState() { memset(this, 0, sizeof(CControllerState)); }
};

class CPad
{
public:
    virtual CControllerState* GetCurrentControllerState(CControllerState* ControllerState) = 0;
    virtual CControllerState* GetLastControllerState(CControllerState* ControllerState) = 0;
    virtual void              SetCurrentControllerState(CControllerState* ControllerState) = 0;
    virtual void              SetLastControllerState(CControllerState* ControllerState) = 0;
    virtual void              Restore() = 0;
    virtual void              Store() = 0;
    virtual bool              IsEnabled() = 0;
    virtual void              Disable(bool bDisable) = 0;
    virtual void              Clear() = 0;
    virtual void              SetHornHistoryValue(bool value) = 0;
    virtual void              SetLastTimeTouched(DWORD dwTime) = 0;
};
