/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPad.cpp
*  PURPOSE:     Game controller pad class
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static const SGTAControl g_gtaControls[] =
{
    { "fire",                   FIRE,                   CONTROL_FOOT },
    { "next_weapon",            NEXT_WEAPON,            CONTROL_FOOT },
    { "previous_weapon",        PREVIOUS_WEAPON,        CONTROL_FOOT },
    { "forwards",               FORWARDS,               CONTROL_FOOT },
    { "backwards",              BACKWARDS,              CONTROL_FOOT },
    { "left",                   LEFT,                   CONTROL_FOOT },
    { "right",                  RIGHT,                  CONTROL_FOOT },
    { "zoom_in",                ZOOM_IN,                CONTROL_FOOT },
    { "zoom_out",               ZOOM_OUT,               CONTROL_FOOT },
    { "enter_exit",             ENTER_EXIT,             CONTROL_BOTH },
    { "change_camera",          CHANGE_CAMERA,          CONTROL_BOTH },
    { "jump",                   JUMP,                   CONTROL_FOOT },
    { "sprint",                 SPRINT,                 CONTROL_FOOT },
    { "look_behind",            LOOK_BEHIND,            CONTROL_FOOT },
    { "crouch",                 CROUCH,                 CONTROL_FOOT },
    { "action",                 ACTION,                 CONTROL_FOOT },
    { "walk",                   WALK,                   CONTROL_FOOT },
    { "vehicle_fire",           VEHICLE_FIRE,           CONTROL_VEHICLE },
    { "vehicle_secondary_fire", VEHICLE_SECONDARY_FIRE, CONTROL_VEHICLE },
    { "vehicle_left",           VEHICLE_LEFT,           CONTROL_VEHICLE },
    { "vehicle_right",          VEHICLE_RIGHT,          CONTROL_VEHICLE },
    { "steer_forward",          STEER_FORWARDS_DOWN,    CONTROL_VEHICLE },
    { "steer_back",             STEER_BACK_UP,          CONTROL_VEHICLE },
    { "accelerate",             ACCELERATE,             CONTROL_VEHICLE },
    { "brake_reverse",          BRAKE_REVERSE,          CONTROL_VEHICLE },
    { "radio_next",             RADIO_NEXT,             CONTROL_VEHICLE },
    { "radio_previous",         RADIO_PREVIOUS,         CONTROL_VEHICLE },
    { "radio_user_track_skip",  RADIO_USER_TRACK_SKIP,  CONTROL_VEHICLE },
    { "horn",                   HORN,                   CONTROL_VEHICLE },
    { "sub_mission",            SUB_MISSION,            CONTROL_BOTH },
    { "handbrake",              HANDBRAKE,              CONTROL_VEHICLE },
    { "vehicle_look_left",      VEHICLE_LOOK_LEFT,      CONTROL_VEHICLE },
    { "vehicle_look_right",     VEHICLE_LOOK_RIGHT,     CONTROL_VEHICLE },
    { "vehicle_look_behind",    VEHICLE_LOOK_BEHIND,    CONTROL_VEHICLE },
    { "vehicle_mouse_look",     VEHICLE_MOUSE_LOOK,     CONTROL_VEHICLE },
    { "special_control_left",   SPECIAL_CONTROL_LEFT,   CONTROL_VEHICLE },
    { "special_control_right",  SPECIAL_CONTROL_RIGHT,  CONTROL_VEHICLE },
    { "special_control_down",   SPECIAL_CONTROL_DOWN,   CONTROL_VEHICLE },
    { "special_control_up",     SPECIAL_CONTROL_UP,     CONTROL_VEHICLE },
    { "aim_weapon",             AIM_WEAPON,             CONTROL_FOOT },
    { "conversation_yes",       CONVERSATION_YES,       CONTROL_FOOT },
    { "conversation_no",        CONVERSATION_NO,        CONTROL_FOOT },
    { "group_control_forwards", GROUP_CONTROL_FORWARDS, CONTROL_FOOT },
    { "group_control_back",     GROUP_CONTROL_BACK,     CONTROL_FOOT },

    { "", (eControllerAction)0, (eControlType)0 }
};


CPad::CPad ( CPlayer* pPlayer )
{
    m_pPlayer = pPlayer;

    for ( int i = 0 ; i < NUM_CONTROL_STATES ; i++ )
    {
        m_ControlStates [ i ].bState = false;
        m_ControlStates [ i ].bEnabled = true;
    }
    m_bUpdatedKeys = false;
}


void CPad::SetCurrentControllerState ( const CControllerState& State )
{
    m_csCurrentState.Copy ( State );

    m_bUpdatedKeys = false;
}


void CPad::NewControllerState ( const CControllerState& State )
{
    SetCurrentControllerState ( State );
}


void CPad::UpdateKeys ( void )
{
    if ( m_pPlayer )
    {
        bool bInVehicle = ( m_pPlayer->GetOccupiedVehicle () != NULL );

        if ( !bInVehicle )
        {
            m_ControlStates [ 0 ].bState = ( m_csCurrentState.ButtonCircle ) ? true : false;
            m_ControlStates [ 1 ].bState = ( m_csCurrentState.RightShoulder2 ) ? true : false;
            m_ControlStates [ 2 ].bState = ( m_csCurrentState.LeftShoulder2 ) ? true : false;
            m_ControlStates [ 3 ].bState = ( m_csCurrentState.LeftStickY < 0 ); // forwards
            m_ControlStates [ 4 ].bState = ( m_csCurrentState.LeftStickY > 0 ); // backwards
            m_ControlStates [ 5 ].bState = ( m_csCurrentState.LeftStickX < 0 ); // left 
            m_ControlStates [ 6 ].bState = ( m_csCurrentState.LeftStickX > 0 ); // right
            m_ControlStates [ 7 ].bState = ( m_csCurrentState.RightShoulder2 ) ? true : false;
            m_ControlStates [ 8 ].bState = ( m_csCurrentState.LeftShoulder2 ) ? true : false;
            m_ControlStates [ 9 ].bState = ( m_csCurrentState.LeftShoulder2 ) ? true : false;
            m_ControlStates [ 10 ].bState = ( m_csCurrentState.ButtonTriangle ) ? true : false;
            m_ControlStates [ 11 ].bState = ( m_csCurrentState.Select ) ? true : false;
            m_ControlStates [ 12 ].bState = ( m_csCurrentState.ButtonCross ) ? true : false;
            m_ControlStates [ 13 ].bState = ( m_csCurrentState.ShockButtonR ) ? true : false;
            m_ControlStates [ 14 ].bState = ( m_csCurrentState.ShockButtonL ) ? true : false;
            m_ControlStates [ 15 ].bState = ( m_csCurrentState.LeftShoulder1 ) ? true : false;
            m_ControlStates [ 16 ].bState = ( m_csCurrentState.m_bPedWalk ) ? true : false;
            // Vehicle Keys
            m_ControlStates [ 39 ].bState = ( m_csCurrentState.RightShoulder1 ) ? true : false;
            m_ControlStates [ 40 ].bState = ( m_csCurrentState.DPadRight ) ? true : false;
            m_ControlStates [ 41 ].bState = ( m_csCurrentState.DPadLeft ) ? true : false;
            m_ControlStates [ 42 ].bState = ( m_csCurrentState.DPadUp ) ? true : false;
            m_ControlStates [ 43 ].bState = ( m_csCurrentState.DPadDown ) ? true : false;
        }
        else
        {
            m_ControlStates [ 17 ].bState = ( m_csCurrentState.ButtonCircle ) ? true : false;
            m_ControlStates [ 18 ].bState = ( m_csCurrentState.LeftShoulder1 ) ? true : false;
            m_ControlStates [ 19 ].bState = ( m_csCurrentState.LeftStickX < 0 ); 
            m_ControlStates [ 20 ].bState = ( m_csCurrentState.LeftStickX > 0 );
            m_ControlStates [ 21 ].bState = ( m_csCurrentState.LeftStickY < 0 );
            m_ControlStates [ 22 ].bState = ( m_csCurrentState.LeftStickY > 0 );
            m_ControlStates [ 23 ].bState = ( m_csCurrentState.ButtonCross ) ? true : false;
            m_ControlStates [ 24 ].bState = ( m_csCurrentState.ButtonSquare ) ? true : false;
            m_ControlStates [ 25 ].bState = ( m_csCurrentState.DPadUp ) ? true : false;
            m_ControlStates [ 26 ].bState = ( m_csCurrentState.DPadDown ) ? true : false;
            m_ControlStates [ 27 ].bState = ( m_csCurrentState.m_bRadioTrackSkip ) ? true : false;
            m_ControlStates [ 28 ].bState = ( m_csCurrentState.ShockButtonL ) ? true : false;
            m_ControlStates [ 29 ].bState = ( m_csCurrentState.ShockButtonR ) ? true : false;
            m_ControlStates [ 30 ].bState = ( m_csCurrentState.RightShoulder1 ) ? true : false;
            m_ControlStates [ 31 ].bState = ( m_csCurrentState.LeftShoulder2 ) ? true : false;
            m_ControlStates [ 32 ].bState = ( m_csCurrentState.RightShoulder2 ) ? true : false;
            m_ControlStates [ 33 ].bState = ( m_csCurrentState.LeftShoulder2 ? true : false &&
                                              m_csCurrentState.RightShoulder2 ? true : false );
            // Mouse Look
            m_ControlStates [ 35 ].bState = ( m_csCurrentState.RightStickX > 0 );
            m_ControlStates [ 36 ].bState = ( m_csCurrentState.RightStickX < 0 );
            m_ControlStates [ 37 ].bState = ( m_csCurrentState.RightStickY > 0 );
            m_ControlStates [ 38 ].bState = ( m_csCurrentState.RightStickY < 0 );

        }
        m_ControlStates [ 9 ].bState = ( m_csCurrentState.ButtonTriangle ); // Enter Exit
        m_ControlStates [ 10 ].bState = ( m_csCurrentState.Select ) ? true : false;; // Change View
    }
}


bool CPad::GetControlState ( const char* szControl, bool& bState )
{
    for ( int i = 0 ; *g_gtaControls [ i ].szControl != '\0' ; i++ )
    {
        const SGTAControl* temp = &g_gtaControls [ i ];
        if ( stricmp ( temp->szControl, szControl ) == 0 )
        {
            if ( !m_bUpdatedKeys )
            {
                UpdateKeys ();
                m_bUpdatedKeys = true;
            }
            bState = m_ControlStates [ i ].bState;

            return true;
        }
    }

    return false;
}


bool CPad::SetControlState ( const char* szControl, bool bState )
{
    for ( int i = 0 ; *g_gtaControls [ i ].szControl != '\0' ; i++ )
    {
        const SGTAControl* temp = &g_gtaControls [ i ];
        if ( stricmp ( temp->szControl, szControl ) == 0 )
        {
            m_ControlStates [ i ].bState = bState;
            return true;
        }
    }

    return false;
}


bool CPad::IsControlEnabled ( const char* szControl, bool& bEnabled )
{
    for ( int i = 0 ; *g_gtaControls [ i ].szControl != '\0' ; i++ )
    {
        const SGTAControl* temp = &g_gtaControls [ i ];
        if ( stricmp ( temp->szControl, szControl ) == 0 )
        {
            bEnabled = m_ControlStates [ i ].bEnabled;
            return true;
        }
    }

    return false;
}


bool CPad::SetControlEnabled ( const char* szControl, bool bEnabled )
{
    for ( int i = 0 ; *g_gtaControls [ i ].szControl != '\0' ; i++ )
    {
        const SGTAControl* temp = &g_gtaControls [ i ];
        if ( stricmp ( temp->szControl, szControl ) == 0 )
        {
            m_ControlStates [ i ].bEnabled = bEnabled;
            return true;
        }
    }

    return false;
}


void CPad::SetAllControlsEnabled ( bool bEnabled )
{
    for ( int i = 0 ; *g_gtaControls [ i ].szControl != '\0' ; i++ )
    {
        m_ControlStates [ i ].bEnabled = bEnabled;
    }
}


const SGTAControl* CPad::GetControlFromString ( const char* szControl )
{
    for ( int i = 0 ; *g_gtaControls [ i ].szControl != '\0' ; i++ )
    {
        const SGTAControl* temp = &g_gtaControls [ i ];
        if ( stricmp ( temp->szControl, szControl ) == 0 )
            return temp;
    }

    return NULL;
}
