/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPad.cpp
*  PURPOSE:     Ped entity controller pad handler class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

/* This class is used for setting controller states on script-created peds,
   through scripting functions.
*/

#include <StdInc.h>

const char * g_GTAControls [ MAX_GTA_CONTROLS ] =
{
    "fire", "next_weapon", "previous_weapon", "forwards", "backwards",
    "left", "right", "zoom_in", "zoom_out", "enter_exit",
    "change_camera", "jump", "sprint", "look_behind", "crouch",
    "action", "walk", "vehicle_fire", "vehicle_secondary_fire",
    "vehicle_left", "vehicle_right", "steer_forward", "steer_back",
    "accelerate", "brake_reverse", "radio_next", "radio_previous",
    "radio_user_track_skip", "horn", "sub_mission", "handbrake",
    "vehicle_look_left", "vehicle_look_right", "vehicle_look_behind",
    "vehicle_mouse_look", "special_control_left", "special_control_right",
    "special_control_down", "special_control_up", "aim_weapon",
    "conversation_yes", "conversation_no", "group_control_forwards",
    "group_control_back",
};

const char * g_AnalogGTAControls [ MAX_GTA_ANALOG_CONTROLS ] =
{
    "left",
    "right",
    "forwards",
    "backwards",
    "vehicle_left",
    "vehicle_right",
    "steer_forward",
    "steer_back",
    "accelerate",
    "brake_reverse",
    "special_control_left",
    "special_control_right",
    "special_control_up",
    "special_control_down",
};


bool CClientPad::GetControlIndex ( const char * szName, unsigned int & uiIndex )
{ 
    for ( unsigned int i = 0 ; i < MAX_GTA_CONTROLS ; i++ )
    {
        if ( !stricmp ( g_GTAControls [ i ], szName ) )
        {
            uiIndex = i;
            return true;
        }
    }
    return false;
}


const char * CClientPad::GetControlName ( unsigned int uiIndex )
{
    if ( uiIndex < MAX_GTA_CONTROLS )
    {
        return g_GTAControls [ uiIndex ];
    }
    return NULL;
}


CClientPad::CClientPad ( void )
{
    memset ( m_bStates, 0, sizeof ( m_bStates ) );
}


bool CClientPad::GetControlState ( const char * szName, bool & bState )
{
    unsigned int uiIndex;
    if ( GetControlIndex ( szName, uiIndex ) )
    {
        bState = m_bStates [ uiIndex ];
        return true;
    }
    return false;
}


bool CClientPad::SetControlState ( const char * szName, bool bState )
{
    unsigned int uiIndex;
    if ( GetControlIndex ( szName, uiIndex ) )
    {
        m_bStates [ uiIndex ] = bState;
        return true;
    }
    return false;
}


void CClientPad::DoPulse ( CClientPed * pPed )
{
    CPlayerPed * pGamePlayer = pPed->GetGamePlayer ();
    if ( pGamePlayer )
    {
        bool bIsDead = false, bInVehicle = false;
        CTask * pTask = pPed->GetTaskManager ()->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_DEAD ) bIsDead = true;
        if ( pGamePlayer->GetVehicle () ) bInVehicle = true;

        CControllerState cs;
        memset ( &cs, 0, sizeof ( CControllerState ) );
        if ( !bIsDead )
        {
            if ( !bInVehicle )
            {
                cs.ButtonCircle = ( m_bStates [ 0 ] ) ? 255 : 0; // Fire
                cs.LeftStickY = ( ( m_bStates [ 3 ] && m_bStates [ 4 ] ) ||
                                ( !m_bStates [ 3 ] && !m_bStates [ 4 ] ) ) ? 0 :
                                ( m_bStates [ 3 ] ) ? -128 : 128;
                cs.LeftStickX = ( ( m_bStates [ 5 ] && m_bStates [ 6 ] ) ||
                                ( !m_bStates [ 5 ] && !m_bStates [ 6 ] ) ) ? 0 :
                                ( m_bStates [ 5 ] ) ? -128 : 128;
                cs.ButtonSquare = ( m_bStates [ 11 ] ) ? 255 : 0; // Jump
                cs.ButtonCross = ( m_bStates [ 12 ] ) ? 255 : 0; // Sprint
                cs.ShockButtonR = ( m_bStates [ 13 ] ) ? 255 : 0; // Look Behind
                cs.ShockButtonL = ( m_bStates [ 14 ] ) ? 255 : 0; // Crouch
                cs.LeftShoulder1 = ( m_bStates [ 15 ] ) ? 255 : 0; // Action
                cs.m_bPedWalk = ( m_bStates [ 16 ] ) ? 255 : 0; // Walk
                cs.RightShoulder1 = ( m_bStates [ 39 ] ) ? 255 : 0; // Aim Weapon
            }
            else
            {
                cs.ButtonCircle = ( m_bStates [ 17 ] ) ? 255 : 0; // Fire
                cs.LeftShoulder1 = ( m_bStates [ 18 ] ) ? 255 : 0; // Secondary Fire
                cs.LeftStickX = ( ( m_bStates [ 19 ] && m_bStates [ 20 ] ) ||
                                ( !m_bStates [ 19 ] && !m_bStates [ 20 ] ) ) ? 0 :
                                ( m_bStates [ 19 ] ) ? -128 : 128;
                cs.LeftStickY = ( ( m_bStates [ 21 ] && m_bStates [ 22 ] ) ||
                                ( !m_bStates [ 21 ] && !m_bStates [ 22 ] ) ) ? 0 :
                                ( m_bStates [ 21 ] ) ? -128 : 128;
                cs.ButtonCross = ( m_bStates [ 23 ] ) ? 255 : 0; // Accelerate
                cs.ButtonSquare = ( m_bStates [ 24 ] ) ? 255 : 0; // Reverse
                cs.ShockButtonL = ( m_bStates [ 28 ] ) ? 255 : 0; // Horn
                cs.RightShoulder1 = ( m_bStates [ 30 ] ) ? 255 : 0; // Handbrake
                cs.LeftShoulder2 = ( m_bStates [ 31 ] || m_bStates [ 33 ] ) ? 255 : 0; // Look Left
                cs.RightShoulder2 = ( m_bStates [ 32 ] || m_bStates [ 33 ] ) ? 255 : 0; // Look Right                
                cs.RightStickX = ( ( m_bStates [ 35 ] && m_bStates [ 36 ] ) ||
                                ( !m_bStates [ 35 ] && !m_bStates [ 36 ] ) ) ? 0 :
                                ( m_bStates [ 35 ] ) ? 128 : -128;
                cs.RightStickY = ( ( m_bStates [ 37 ] && m_bStates [ 38 ] ) ||
                                ( !m_bStates [ 37 ] && !m_bStates [ 38 ] ) ) ? 0 :
                                ( m_bStates [ 37 ] ) ? 128 : -128;
            }
        }
        pPed->SetControllerState ( cs );
    }
}



bool CClientPad::GetControlState ( const char * szName, CControllerState & State, bool bOnFoot )
{
    unsigned int uiIndex;
    if ( GetControlIndex ( szName, uiIndex ) )
    {
        if ( bOnFoot )
        {
            switch ( uiIndex )
            {
                case 0: return State.ButtonCircle == 255; break;        // fire
                case 1: return State.RightShoulder2 == 255; break;      // next wep
                case 2: return State.LeftShoulder2 == 255; break;       // previous wep
                case 3: return State.LeftStickY == -128; break;         // forwards
                case 4: return State.LeftStickY == 128; break;          // backwards
                case 5: return State.LeftStickX == -128; break;         // left
                case 6: return State.LeftStickX == 128; break;          // right
                case 7: return State.RightShoulder2 == 255; break;      // zoom in
                case 8: return State.LeftShoulder2 == 255; break;       // zoom out
                case 9: return false; break;                            // enter_exit
                case 10: return false; break;                           // change_cam
                case 11: return State.ButtonSquare == 255; break;       // jump
                case 12: return State.ButtonCross == 255; break;        // sprint
                case 13: return State.ShockButtonR == 255; break;       // look behind
                case 14: return State.ShockButtonL == 255; break;       // crouch
                case 15: return State.LeftShoulder1 == 255; break;      // action
                case 16: return State.m_bPedWalk > 0; break;            // walk
                // vehicle keys
                case 39: return State.RightShoulder1 == 255; break;     // aim
                case 40: return State.DPadRight == 255; break;          // conv yes
                case 41: return State.DPadLeft == 255; break;           // conv no
                case 42: return State.DPadUp == 255; break;             // group forward
                case 43: return State.DPadDown == 255; break;           // group backward
                default: break;
            }
        }
        else
        {
            switch ( uiIndex )
            {
                case 17: return State.ButtonCircle == 255; break;       // fire
                case 18: return State.LeftShoulder1 == 255; break;      // secondary fire
                case 19: return State.LeftStickX == -128; break;        // left
                case 20: return State.LeftStickX == 128; break;         // right
                case 21: return State.LeftStickY == -128; break;        // forward
                case 22: return State.LeftStickY == 128; break;         // backward
                case 23: return State.ButtonCross == 255; break;        // accel
                case 24: return State.ButtonSquare == 255; break;       // reverse
                case 25: return State.DPadUp == 255; break;             // radio next
                case 26: return State.DPadDown == 255; break;           // radio prev
                case 27: return State.m_bRadioTrackSkip == 255; break;  // track skip
                case 28: return State.ShockButtonL == 255; break;       // horn
                case 29: return State.ShockButtonR == 255; break;       // sub-mission
                case 30: return State.RightShoulder1 == 255; break;     // handbrake
                case 31: return State.LeftShoulder2 == 255; break;      // look left
                case 32: return State.RightShoulder2 == 255; break;     // look right
                case 33: return false; break;                           // look behind
                case 34: return false; break;                           // mouse look
                case 35: return State.RightStickX == 128; break;        // control left
                case 36: return State.RightStickX == -128; break;       // control right
                case 37: return State.RightStickY == 128; break;        // control down
                case 38: return State.RightStickY == -128; break;       // control up
                default: break;
            }
        }
    }
    return false;
}

bool CClientPad::GetAnalogControlIndex ( const char * szName, unsigned int & uiIndex )
{
    for ( unsigned int i = 0 ; i < MAX_GTA_ANALOG_CONTROLS ; i++ )
    {
        if ( !stricmp ( g_AnalogGTAControls [ i ], szName ) )
        {
            uiIndex = i;
            return true;
        }
    }
    return false;
}


bool CClientPad::GetAnalogControlState ( const char * szName, CControllerState & cs, bool bOnFoot, float & fState )
{
    unsigned int uiIndex;
    if ( GetAnalogControlIndex ( szName, uiIndex ) )
    {       
        if ( bOnFoot )
        {
            switch ( uiIndex )
            {
                case 0: fState = cs.LeftStickX < 0 ? cs.LeftStickX/-128.0f : 0 ; return true;  //Left
                case 1: fState = cs.LeftStickX > 0 ? cs.LeftStickX/128.0f : 0 ; return true;  //Right
                case 2: fState = cs.LeftStickY < 0 ? cs.LeftStickY/-128.0f : 0 ; return true;  //Up
                case 3: fState = cs.LeftStickY > 0 ? cs.LeftStickY/128.0f : 0 ; return true;  //Down
                default: fState = 0; return true;
            }
            
        }
        else
        {
            switch ( uiIndex )
            { 
                case 4: fState = cs.LeftStickX  < 0 ? cs.LeftStickX/-128.0f : 0 ; return true;  //Left
                case 5: fState = cs.LeftStickX  > 0 ? cs.LeftStickX/128.0f : 0 ; return true;  //Right 
                case 6: fState = cs.LeftStickY  < 0 ? cs.LeftStickY/-128.0f : 0 ; return true;  //Up
                case 7: fState = cs.LeftStickY  > 0 ? cs.LeftStickY/128.0f : 0 ; return true;  //Down
                case 8: fState = cs.ButtonCross > 0 ? cs.ButtonCross/255.0f : 0 ; return true;  //Accel
                case 9: fState = cs.ButtonSquare > 0 ? cs.ButtonSquare/255.0f : 0 ; return true;  //Reverse
                case 10: fState = cs.RightStickX < 0 ? cs.RightStickX/-128.0f : 0 ; return true;  //Special Left
                case 11: fState = cs.RightStickX > 0 ? cs.RightStickX/128.0f : 0 ; return true;  //Special Right
                case 12: fState = cs.RightStickY < 0 ? cs.RightStickY/-128.0f : 0 ; return true;  //Special Up
                case 13: fState = cs.RightStickY > 0 ? cs.RightStickY/128.0f : 0 ; return true;  //Special Down
                default: fState = 0; return true;
            }
        }
    }
    return false;
}
