/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPad.h
*  PURPOSE:     Game controller pad class
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPad;

#ifndef __CPAD_H
#define __CPAD_H

#include <stdio.h>
#include <string.h>
#include "CPlayer.h"

class CControllerState
{
public:
    signed short        LeftStickX; // move/steer left (-128?)/right (+128)
    signed short        LeftStickY; // move back(+128)/forwards(-128?)
    signed short        RightStickX; // numpad 6(+128)/numpad 4(-128?)
    signed short        RightStickY;
    
    //signed short        LeftShoulder1;
    signed short        LeftShoulder2;
    //signed short        RightShoulder1; // target / hand brake
    signed short        RightShoulder2; 
    
    signed short        DPadUp; // radio change up
    signed short        DPadDown; // radio change down
    signed short        DPadLeft;
    signed short        DPadRight;

    signed short        Start;
    signed short        Select;

    //signed short        ButtonSquare; // jump / reverse
    //signed short        ButtonTriangle; // get in/out
    //signed short        ButtonCross; // sprint / accelerate
    //signed short        ButtonCircle; // fire

    //signed short        ShockButtonL;
    signed short        ShockButtonR; // look behind

    signed short        m_bChatIndicated;
    //signed short        m_bPedWalk;
    //signed short        m_bVehicleMouseLook;
    signed short        m_bRadioTrackSkip;

    // Sync'd keys
    bool LeftShoulder1;
    bool RightShoulder1;
    bool ButtonSquare;
    bool ButtonCross;
    bool ButtonCircle;
    bool ButtonTriangle;
    bool ShockButtonL;
    bool m_bPedWalk;

    inline              CControllerState    ( void )
    { 
        memset ( this, 0, sizeof ( CControllerState ) );
    }
    inline void         Copy    ( const CControllerState& State )
    {
        memcpy ( this, &State, sizeof ( CControllerState ) );
    }
};

enum eControllerAction
{
    FIRE = 0,
    
    NEXT_WEAPON = 2,
    PREVIOUS_WEAPON,
    FORWARDS,
    BACKWARDS,
    LEFT,
    RIGHT,
    ZOOM_IN,
    ZOOM_OUT,
    ENTER_EXIT,
    CHANGE_CAMERA,
    JUMP,
    SPRINT,
    LOOK_BEHIND,
    CROUCH,
    ACTION,
    WALK,
    VEHICLE_FIRE,
    VEHICLE_SECONDARY_FIRE,
    VEHICLE_LEFT,
    VEHICLE_RIGHT,
    STEER_FORWARDS_DOWN,
    STEER_BACK_UP,
    ACCELERATE,
    BRAKE_REVERSE,
    RADIO_NEXT,
    RADIO_PREVIOUS,
    RADIO_USER_TRACK_SKIP,
    HORN,
    SUB_MISSION,
    HANDBRAKE,
    
    VEHICLE_LOOK_LEFT = 0x22,
    VEHICLE_LOOK_RIGHT,
    VEHICLE_LOOK_BEHIND,
    VEHICLE_MOUSE_LOOK,
    SPECIAL_CONTROL_LEFT,
    SPECIAL_CONTROL_RIGHT,
    SPECIAL_CONTROL_DOWN,
    SPECIAL_CONTROL_UP,

    AIM_WEAPON = 0x2D,

    CONVERSATION_YES = 0x2F,
    CONVERSATION_NO,
    GROUP_CONTROL_FORWARDS,
    GROUP_CONTROL_BACK,
};

enum eControlType
{
    CONTROL_FOOT,
    CONTROL_VEHICLE,
    CONTROL_BOTH
};

struct SGTAControl
{
    const char* szControl;
    eControllerAction action;
    eControlType controlType;
};

struct SGTAControlState
{
    bool bState;
    bool bEnabled;
};

#define NUM_CONTROL_STATES 45
#define NUM_MTA_CONTROL_STATES 19

class CPad
{
public:
                                CPad                            ( CPlayer* pPlayer );

    const CControllerState&     GetCurrentControllerState       ( void )                            { return m_csCurrentState; }
    void                        SetCurrentControllerState       ( const CControllerState& State );

    void                        NewControllerState              ( const CControllerState& State );

    bool                        GetControlState                 ( const char* szControl, bool& bState );
    bool                        SetControlState                 ( const char* szControl, bool bState );
    static const SGTAControl*   GetControlFromString            ( const char* szControl );

    bool                        IsControlEnabled                ( const char* szControl, bool& bEnabled );
    bool                        SetControlEnabled               ( const char* szControl, bool bEnabled );

    void                        SetAllGTAControlsEnabled        ( bool bEnabled );
    void                        SetAllMTAControlsEnabled        ( bool bEnabled );

protected:
    void                        UpdateKeys                      ( void );

    CPlayer*                    m_pPlayer;

    CControllerState            m_csCurrentState;

    SFixedArray < SGTAControlState, NUM_CONTROL_STATES >    m_ControlStates;
    SFixedArray < bool, NUM_MTA_CONTROL_STATES >            m_MTAEnabledControls;
    bool                        m_bUpdatedKeys;
};

#endif
