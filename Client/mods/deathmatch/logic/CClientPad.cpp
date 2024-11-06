/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPad.cpp
 *  PURPOSE:     Ped entity controller pad handler class
 *
 *****************************************************************************/

/* This class is used for setting controller states on script-created peds,
   through scripting functions.
*/
#include <StdInc.h>
#include <game/CTaskManager.h>
#include <game/Task.h>

#define CS_NAN -32768

SFixedArray<short, MAX_GTA_CONTROLS>       CClientPad::m_sScriptedStates;
SFixedArray<bool, MAX_GTA_ANALOG_CONTROLS> CClientPad::m_bScriptedStatesNextFrameOverride;
bool                                       CClientPad::m_bFlyWithMouse;
bool                                       CClientPad::m_bSteerWithMouse;

static const SFixedArray<const char*, MAX_GTA_CONTROLS> g_GTAControls = {{
    "fire",
    "next_weapon",
    "previous_weapon",
    "forwards",
    "backwards",
    "left",
    "right",
    "zoom_in",
    "zoom_out",
    "enter_exit",
    "change_camera",
    "jump",
    "sprint",
    "look_behind",
    "crouch",
    "action",
    "walk",
    "vehicle_fire",
    "vehicle_secondary_fire",
    "vehicle_left",
    "vehicle_right",
    "steer_forward",
    "steer_back",
    "accelerate",
    "brake_reverse",
    "radio_next",
    "radio_previous",
    "radio_user_track_skip",
    "horn",
    "sub_mission",
    "handbrake",
    "vehicle_look_left",
    "vehicle_look_right",
    "vehicle_look_behind",
    "vehicle_mouse_look",
    "special_control_left",
    "special_control_right",
    "special_control_down",
    "special_control_up",
    "aim_weapon",
    "conversation_yes",
    "conversation_no",
    "group_control_forwards",
    "group_control_back",
}};

static const SFixedArray<const char*, MAX_GTA_ANALOG_CONTROLS> g_AnalogGTAControls = {{
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
}};

bool CClientPad::GetControlIndex(const char* szName, unsigned int& uiIndex)
{
    for (unsigned int i = 0; i < MAX_GTA_CONTROLS; i++)
    {
        if (!stricmp(g_GTAControls[i], szName))
        {
            uiIndex = i;
            return true;
        }
    }
    return false;
}

const char* CClientPad::GetControlName(unsigned int uiIndex)
{
    if (uiIndex < MAX_GTA_CONTROLS)
    {
        return g_GTAControls[uiIndex];
    }
    return NULL;
}

CClientPad::CClientPad()
{
    memset(&m_fStates, 0, sizeof(m_fStates));

    for (unsigned int i = 0; i < MAX_GTA_CONTROLS; i++)
    {
        m_fStates[i] = 0.0f;
    }

    // Initialise our analog control states
    for (unsigned int i = 0; i < MAX_GTA_ANALOG_CONTROLS; i++)
    {
        m_sScriptedStates[i] = CS_NAN;
        m_bScriptedStatesNextFrameOverride[i] = false;
    }
}

bool CClientPad::GetControlState(const char* szName, bool& bState)
{
    unsigned int uiIndex;
    if (GetControlIndex(szName, uiIndex))
    {
        bState = m_fStates[uiIndex] ? true : false;
        return true;
    }
    return false;
}

// Sets the control state according to the custom ped state.  Use for peds.
bool CClientPad::SetControlState(const char* szName, bool bState)
{
    unsigned int uiIndex;
    if (GetControlIndex(szName, uiIndex))
    {
        m_fStates[uiIndex] = bState ? 1.0f : 0.0f;
        return true;
    }
    return false;
}

// Gets the analog control state according to custom pad state.  Use for peds.
bool CClientPad::GetControlState(const char* szName, float& fState)
{
    unsigned int uiIndex;
    if (GetControlIndex(szName, uiIndex))
    {
        fState = m_fStates[uiIndex];
        return true;
    }
    return false;
}

// Sets the control state according to the custom ped state.  Use for peds.
bool CClientPad::SetControlState(const char* szName, float fState)
{
    fState = Clamp<float>(0, fState, 1);
    unsigned int uiIndex;
    if (GetAnalogControlIndex(szName, uiIndex) && GetControlIndex(szName, uiIndex))
    {
        m_fStates[uiIndex] = fState;
        // Clear out any opposing states
        switch (uiIndex)
        {
            case 3:
                m_fStates[4] = 0;
                break;
            case 4:
                m_fStates[3] = 0;
                break;
            case 5:
                m_fStates[6] = 0;
                break;
            case 6:
                m_fStates[5] = 0;
                break;
            case 19:
                m_fStates[20] = 0;
                break;
            case 20:
                m_fStates[19] = 0;
                break;
            case 21:
                m_fStates[22] = 0;
                break;
            case 22:
                m_fStates[21] = 0;
                break;
            case 35:
                m_fStates[36] = 0;
                break;
            case 36:
                m_fStates[35] = 0;
                break;
            case 37:
                m_fStates[38] = 0;
                break;
            case 38:
                m_fStates[37] = 0;
                break;
            default:
                break;
        }
        return true;
    }
    return false;
}

// Process Ped control states.
void CClientPad::DoPulse(CClientPed* pPed)
{
    CPlayerPed* pGamePlayer = pPed->GetGamePlayer();
    if (pGamePlayer)
    {
        bool   bIsDead = false, bInVehicle = false;
        CTask* pTask = pPed->GetTaskManager()->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP);
        if (pTask && pTask->GetTaskType() == TASK_SIMPLE_DEAD)
            bIsDead = true;
        if (pGamePlayer->GetVehicle())
            bInVehicle = true;

        CControllerState cs;
        memset(&cs, 0, sizeof(CControllerState));
        if (!bIsDead)
        {
            if (!bInVehicle)
            {
                cs.ButtonCircle = (m_fStates[0]) ? 255 : 0;            // Fire

                cs.LeftStickY = (short)(((m_fStates[3] && m_fStates[4]) || (!m_fStates[3] && !m_fStates[4])) ? 0
                                        : (m_fStates[3])                                                     ? m_fStates[3] * -128
                                                                                                             : m_fStates[4] * 128);

                cs.LeftStickX = (short)(((m_fStates[5] && m_fStates[6]) || (!m_fStates[5] && !m_fStates[6])) ? 0
                                        : (m_fStates[5])                                                     ? m_fStates[5] * -128
                                                                                                             : m_fStates[6] * 128);

                cs.ButtonTriangle = (m_fStates[9]) ? 255 : 0;            // Get in/out and alternative fighting styles

                cs.ButtonSquare = (m_fStates[11]) ? 255 : 0;            // Jump

                cs.ButtonCross = (m_fStates[12]) ? 255 : 0;            // Sprint

                cs.ShockButtonR = (m_fStates[13]) ? 255 : 0;            // Look Behind

                cs.ShockButtonL = (m_fStates[14]) ? 255 : 0;            // Crouch

                cs.LeftShoulder1 = (m_fStates[15]) ? 255 : 0;            // Action

                cs.m_bPedWalk = (m_fStates[16]) ? 255 : 0;            // Walk

                cs.RightShoulder1 = (m_fStates[39]) ? 255 : 0;            // Aim Weapon
            }
            else
            {
                cs.ButtonCircle = (m_fStates[17]) ? 255 : 0;            // Fire

                cs.LeftShoulder1 = (m_fStates[18]) ? 255 : 0;            // Secondary Fire
                cs.LeftStickX = (short)(((m_fStates[19] && m_fStates[20]) || (!m_fStates[19] && !m_fStates[20])) ? 0
                                        : (m_fStates[19])                                                        ? m_fStates[19] * -128
                                                                                                                 : m_fStates[20] * 128);

                cs.LeftStickY = (short)(((m_fStates[21] && m_fStates[22]) || (!m_fStates[21] && !m_fStates[22])) ? 0
                                        : (m_fStates[21])                                                        ? m_fStates[21] * -128
                                                                                                                 : m_fStates[22] * 128);

                cs.ButtonCross = (short)((m_fStates[23] * 255));            // Accelerate

                cs.ButtonSquare = (short)((m_fStates[24] * 255));            // Reverse

                cs.ShockButtonL = (m_fStates[28]) ? 255 : 0;            // Horn

                cs.RightShoulder1 = (m_fStates[30]) ? 255 : 0;            // Handbrake

                cs.LeftShoulder2 = (m_fStates[31] || m_fStates[33]) ? 255 : 0;            // Look Left

                cs.RightShoulder2 = (m_fStates[32] || m_fStates[33]) ? 255 : 0;            // Look Right

                cs.RightStickX = (short)(((m_fStates[35] && m_fStates[36]) || (!m_fStates[35] && !m_fStates[36])) ? 0
                                         : (m_fStates[35])                                                        ? m_fStates[35] * 128
                                                                                                                  : m_fStates[36] * -128);

                cs.RightStickY = (short)(((m_fStates[37] && m_fStates[38]) || (!m_fStates[37] && !m_fStates[38])) ? 0
                                         : (m_fStates[37])                                                        ? m_fStates[37] * 128
                                                                                                                  : m_fStates[38] * -128);
            }
        }
        pPed->SetControllerState(cs);
    }
}

// Get the control state directly from a pad state.  Use for players.
bool CClientPad::GetControlState(const char* szName, CControllerState& State, bool bOnFoot)
{
    unsigned int uiIndex;
    if (GetControlIndex(szName, uiIndex))
    {
        if (bOnFoot)
        {
            switch (uiIndex)
            {
                case 0:
                    return State.ButtonCircle == 255;
                    break;            // fire
                case 1:
                    return State.RightShoulder2 == 255;
                    break;            // next wep
                case 2:
                    return State.LeftShoulder2 == 255;
                    break;            // previous wep
                case 3:
                    return State.LeftStickY == -128;
                    break;            // forwards
                case 4:
                    return State.LeftStickY == 128;
                    break;            // backwards
                case 5:
                    return State.LeftStickX == -128;
                    break;            // left
                case 6:
                    return State.LeftStickX == 128;
                    break;            // right
                case 7:
                    return State.RightShoulder2 == 255;
                    break;            // zoom in
                case 8:
                    return State.LeftShoulder2 == 255;
                    break;            // zoom out
                case 9:
                    return State.ButtonTriangle == 255; // enter_exit
                case 10:
                    return State.Select == 255; // change_cam
                case 11:
                    return State.ButtonSquare == 255;
                    break;            // jump
                case 12:
                    return State.ButtonCross == 255;
                    break;            // sprint
                case 13:
                    return State.ShockButtonR == 255;
                    break;            // look behind
                case 14:
                    return State.ShockButtonL == 255;
                    break;            // crouch
                case 15:
                    return State.LeftShoulder1 == 255;
                    break;            // action
                case 16:
                    return State.m_bPedWalk > 0;
                    break;            // walk
                // vehicle keys
                case 39:
                    return State.RightShoulder1 == 255;
                    break;            // aim
                case 40:
                    return State.DPadRight == 255;
                    break;            // conv yes
                case 41:
                    return State.DPadLeft == 255;
                    break;            // conv no
                case 42:
                    return State.DPadUp == 255;
                    break;            // group forward
                case 43:
                    return State.DPadDown == 255;
                    break;            // group backward
                default:
                    break;
            }
        }
        else
        {
            switch (uiIndex)
            {
                case 17:
                    return State.ButtonCircle == 255;
                    break;            // fire
                case 18:
                    return State.LeftShoulder1 == 255;
                    break;            // secondary fire
                case 19:
                    return State.LeftStickX == -128;
                    break;            // left
                case 20:
                    return State.LeftStickX == 128;
                    break;            // right
                case 21:
                    return State.LeftStickY == -128;
                    break;            // forward
                case 22:
                    return State.LeftStickY == 128;
                    break;            // backward
                case 23:
                    return State.ButtonCross == 255;
                    break;            // accel
                case 24:
                    return State.ButtonSquare == 255;
                    break;            // reverse
                case 25:
                    return State.DPadUp == 255;
                    break;            // radio next
                case 26:
                    return State.DPadDown == 255;
                    break;            // radio prev
                case 27:
                    return State.m_bRadioTrackSkip == 255;
                    break;            // track skip
                case 28:
                    return State.ShockButtonL == 255;
                    break;            // horn
                case 29:
                    return State.ShockButtonR == 255;
                    break;            // sub-mission
                case 30:
                    return State.RightShoulder1 == 255;
                    break;            // handbrake
                case 31:
                    return State.LeftShoulder2 == 255;
                    break;            // look left
                case 32:
                    return State.RightShoulder2 == 255;
                    break;            // look right
                case 33:
                    return State.LeftShoulder2 == 255 && State.RightShoulder2 == 255; // look behind
                case 34:
                    return false;
                    break;            // mouse look
                case 35:
                    return State.RightStickX == 128;
                    break;            // control left
                case 36:
                    return State.RightStickX == -128;
                    break;            // control right
                case 37:
                    return State.RightStickY == 128;
                    break;            // control down
                case 38:
                    return State.RightStickY == -128;
                    break;            // control up
                default:
                    break;
            }
        }
    }

    return false;
}

bool CClientPad::GetAnalogControlIndex(const char* szName, unsigned int& uiIndex)
{
    for (unsigned int i = 0; i < MAX_GTA_ANALOG_CONTROLS; i++)
    {
        if (!stricmp(g_AnalogGTAControls[i], szName))
        {
            uiIndex = i;
            return true;
        }
    }
    return false;
}

// Get the analog control state directly from a pad state.  Use for players.
bool CClientPad::GetAnalogControlState(const char* szName, CControllerState& cs, bool bOnFoot, float& fState, bool bIgnoreOverrides)
{
    unsigned int uiIndex;
    if (GetAnalogControlIndex(szName, uiIndex))
    {
        // If we are not ignoring overrides and have a script override
        if (!bIgnoreOverrides && m_sScriptedStates[uiIndex] != CS_NAN)
            switch (uiIndex)
            {
                case 0:
                    fState = m_sScriptedStates[uiIndex] / -128.0f;
                    return true;            // Left
                case 1:
                    fState = m_sScriptedStates[uiIndex] / 128.0f;
                    return true;            // Right
                case 2:
                    fState = m_sScriptedStates[uiIndex] / -128.0f;
                    return true;            // Up
                case 3:
                    fState = m_sScriptedStates[uiIndex] / 128.0f;
                    return true;            // Down
                case 4:
                    fState = m_sScriptedStates[uiIndex] / -128.0f;
                    return true;            // Left
                case 5:
                    fState = m_sScriptedStates[uiIndex] / 128.0f;
                    return true;            // Right
                case 6:
                    fState = m_sScriptedStates[uiIndex] / -128.0f;
                    return true;            // Up
                case 7:
                    fState = m_sScriptedStates[uiIndex] / 128.0f;
                    return true;            // Down
                case 8:
                    fState = m_sScriptedStates[uiIndex] / 255.0f;
                    return true;            // Accel
                case 9:
                    fState = m_sScriptedStates[uiIndex] / 255.0f;
                    return true;            // Reverse
                case 10:
                    fState = m_sScriptedStates[uiIndex] / -128.0f;
                    return true;            // Special Left
                case 11:
                    fState = m_sScriptedStates[uiIndex] / 128.0f;
                    return true;            // Special Right
                case 12:
                    fState = m_sScriptedStates[uiIndex] / -128.0f;
                    return true;            // Special Up
                case 13:
                    fState = m_sScriptedStates[uiIndex] / 128.0f;
                    return true;            // Special Down
                default:
                    return false;
            }

        if (bOnFoot)
        {
            switch (uiIndex)
            {
                case 0:
                    fState = cs.LeftStickX < 0 ? cs.LeftStickX / -128.0f : 0;
                    return true;            // Left
                case 1:
                    fState = cs.LeftStickX > 0 ? cs.LeftStickX / 128.0f : 0;
                    return true;            // Right
                case 2:
                    fState = cs.LeftStickY < 0 ? cs.LeftStickY / -128.0f : 0;
                    return true;            // Up
                case 3:
                    fState = cs.LeftStickY > 0 ? cs.LeftStickY / 128.0f : 0;
                    return true;            // Down
                default:
                    fState = 0;
                    return true;
            }
        }
        else
        {
            switch (uiIndex)
            {
                case 4:
                    fState = cs.LeftStickX < 0 ? cs.LeftStickX / -128.0f : 0;
                    return true;            // Left
                case 5:
                    fState = cs.LeftStickX > 0 ? cs.LeftStickX / 128.0f : 0;
                    return true;            // Right
                case 6:
                    fState = cs.LeftStickY < 0 ? cs.LeftStickY / -128.0f : 0;
                    return true;            // Up
                case 7:
                    fState = cs.LeftStickY > 0 ? cs.LeftStickY / 128.0f : 0;
                    return true;            // Down
                case 8:
                    fState = cs.ButtonCross > 0 ? cs.ButtonCross / 255.0f : 0;
                    return true;            // Accel
                case 9:
                    fState = cs.ButtonSquare > 0 ? cs.ButtonSquare / 255.0f : 0;
                    return true;            // Reverse
                case 10:
                    fState = cs.RightStickX < 0 ? cs.RightStickX / -128.0f : 0;
                    return true;            // Special Left
                case 11:
                    fState = cs.RightStickX > 0 ? cs.RightStickX / 128.0f : 0;
                    return true;            // Special Right
                case 12:
                    fState = cs.RightStickY < 0 ? cs.RightStickY / -128.0f : 0;
                    return true;            // Special Up
                case 13:
                    fState = cs.RightStickY > 0 ? cs.RightStickY / 128.0f : 0;
                    return true;            // Special Down
                default:
                    fState = 0;
                    return true;
            }
        }
    }

    return false;
}
// Set the analog control state and store them temporarilly before they are actually applied.  Used for players.
bool CClientPad::SetAnalogControlState(const char* szName, float fState, bool bFrameForced)
{
    // Ensure values are between 0 and 1
    fState = Clamp<float>(0, fState, 1);
    unsigned int uiIndex;
    if (GetAnalogControlIndex(szName, uiIndex))
    {
        switch (uiIndex)
        {
            case 0:
                m_sScriptedStates[uiIndex] = (short)(fState * -128.0f);
                m_sScriptedStates[1] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[1] = false;
                return true;            // Left
            case 1:
                m_sScriptedStates[uiIndex] = (short)(fState * 128.0f);
                m_sScriptedStates[0] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[0] = false;
                return true;            // Right
            case 2:
                m_sScriptedStates[uiIndex] = (short)(fState * -128.0f);
                m_sScriptedStates[3] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[3] = false;
                return true;            // Up
            case 3:
                m_sScriptedStates[uiIndex] = (short)(fState * 128.0f);
                m_sScriptedStates[2] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[2] = false;
                return true;            // Down
            case 4:
                m_sScriptedStates[uiIndex] = (short)(fState * -128.0f);
                m_sScriptedStates[5] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[5] = false;
                return true;            // Vehicle Left
            case 5:
                m_sScriptedStates[uiIndex] = (short)(fState * 128.0f);
                m_sScriptedStates[4] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[4] = false;
                return true;            // Vehicle Right
            case 6:
                m_sScriptedStates[uiIndex] = (short)(fState * -128.0f);
                m_sScriptedStates[7] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[7] = false;
                return true;            // Up
            case 7:
                m_sScriptedStates[uiIndex] = (short)(fState * 128.0f);
                m_sScriptedStates[6] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[6] = false;
                return true;            // Down
            case 8:
                m_sScriptedStates[uiIndex] = (short)(fState * 255.0f);
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                return true;            // Accel
            case 9:
                m_sScriptedStates[uiIndex] = (short)(fState * 255.0f);
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                return true;            // Reverse
            case 10:
                m_sScriptedStates[uiIndex] = (short)(fState * -128.0f);
                m_sScriptedStates[11] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[11] = false;
                return true;            // Special Left
            case 11:
                m_sScriptedStates[uiIndex] = (short)(fState * 128.0f);
                m_sScriptedStates[10] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[10] = false;
                return true;            // Special Right
            case 12:
                m_sScriptedStates[uiIndex] = (short)(fState * -128.0f);
                m_sScriptedStates[13] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[13] = false;
                return true;            // Special Up
            case 13:
                m_sScriptedStates[uiIndex] = (short)(fState * 128.0f);
                m_sScriptedStates[12] = 0;
                m_bScriptedStatesNextFrameOverride[uiIndex] = bFrameForced;
                m_bScriptedStatesNextFrameOverride[12] = false;
                return true;            // Special Down
            default:
                return false;
        }
    }

    return false;
}

void CClientPad::RemoveSetAnalogControlState(const char* szName)
{
    unsigned int uiIndex;
    if (GetAnalogControlIndex(szName, uiIndex))
        m_sScriptedStates[uiIndex] = CS_NAN;
}

void CClientPad::ProcessSetAnalogControlState(CControllerState& cs, bool bOnFoot)
{
    // We forcefully apply the control state until we find that the user isnt pressing that button anymore.
    // When that happens, we wait for new input and then stop setting the control state and remove it.
    if (bOnFoot)
    {
        unsigned int uiIndex = 0;

        ProcessControl(cs.LeftStickX, uiIndex);            // Left
        uiIndex++;
        ProcessControl(cs.LeftStickX, uiIndex);            // Right
        uiIndex++;
        ProcessControl(cs.LeftStickY, uiIndex);            // Up
        uiIndex++;
        ProcessControl(cs.LeftStickY, uiIndex);            // Down
    }
    else
    {
        unsigned int uiIndex = 4;

        ProcessControl(cs.LeftStickX, uiIndex);            // Left
        uiIndex++;
        ProcessControl(cs.LeftStickX, uiIndex);            // Right
        uiIndex++;
        ProcessControl(cs.LeftStickY, uiIndex);            // Up
        uiIndex++;
        ProcessControl(cs.LeftStickY, uiIndex);            // Down
        uiIndex++;
        ProcessControl(cs.ButtonCross, uiIndex);            // Accel
        uiIndex++;
        ProcessControl(cs.ButtonSquare, uiIndex);            // Brake
        uiIndex++;
        ProcessControl(cs.RightStickX, uiIndex);            // Special Left
        uiIndex++;
        ProcessControl(cs.RightStickX, uiIndex);            // Special Right
        uiIndex++;
        ProcessControl(cs.RightStickY, uiIndex);            // Special Up
        uiIndex++;
        ProcessControl(cs.RightStickY, uiIndex);            // Special Down
    }
}

void CClientPad::ProcessControl(short& usControlValue, unsigned int uiIndex)
{
    // Note:    control values can be 0, negative or positive
    //          that's why we check unequals != 0
    //          otherwise the values are already in their expected value boundaries
    //
    // usControlValue                       is the updated input value we get from the player
    // m_sScriptedStates                    contains our script value
    // m_bScriptedStatesNextFrameOverride   if the player input should be forcefully overriden for the next frame
    //
    //
    // old behavior or (override == false)
    //      - player input will not be overwitten if it's unequals to 0* and script input is set 0
    //      - otherwise it will use the last set value after player input went 0*
    //        and will keep this behavior for comming frames
    //
    // behavior with (override == true)
    //      - will overwrite the player input even if not 0*
    //        only for the next frame
    //
    // 0* = no key pressed or analog hardware controll touched
    //
    //

    if (m_bScriptedStatesNextFrameOverride[uiIndex])
    {
        m_bScriptedStatesNextFrameOverride[uiIndex] = false;
        if (m_sScriptedStates[uiIndex] != CS_NAN)
            std::swap(usControlValue, m_sScriptedStates[uiIndex]);
    }
    else
    {
        if (usControlValue != 0)
            m_sScriptedStates[uiIndex] = CS_NAN;
        else if (m_sScriptedStates[uiIndex] != CS_NAN && m_sScriptedStates[uiIndex] != 0)
            usControlValue = m_sScriptedStates[uiIndex];
    }
}

// Process toggled controls and apply them directly to the pad state.  Used for players when keyboard input blocking is insufficient.
void CClientPad::ProcessAllToggledControls(CControllerState& cs, bool bOnFoot)
{
    bool bDisableControllerLeftRight = false;
    bDisableControllerLeftRight |= ProcessToggledControl("left", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("left"));
    bDisableControllerLeftRight |= ProcessToggledControl("right", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("right"));
    bDisableControllerLeftRight |= ProcessToggledControl("forwards", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("forwards"));
    bDisableControllerLeftRight |= ProcessToggledControl("backwards", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("backwards"));
    bDisableControllerLeftRight |= ProcessToggledControl("vehicle_left", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("vehicle_left"));
    bDisableControllerLeftRight |= ProcessToggledControl("vehicle_right", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("vehicle_right"));
    bDisableControllerLeftRight |= ProcessToggledControl("steer_forward", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("steer_forward"));
    bDisableControllerLeftRight |= ProcessToggledControl("steer_back", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("steer_back"));
    ProcessToggledControl("accelerate", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("accelerate"));
    ProcessToggledControl("brake_reverse", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("brake_reverse"));
    ProcessToggledControl("special_control_left", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("special_control_left"));
    ProcessToggledControl("special_control_right", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("special_control_right"));
    ProcessToggledControl("special_control_up", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("special_control_up"));
    ProcessToggledControl("special_control_down", cs, bOnFoot, g_pCore->GetKeyBinds()->IsControlEnabled("special_control_down"));

    g_pGame->GetControllerConfigManager()->SuspendSteerAndFlyWithMouse(bDisableControllerLeftRight);
}

// Clear pad data for a disabled control. Returns true if control was actually disabled.
bool CClientPad::ProcessToggledControl(const char* szName, CControllerState& cs, bool bOnFoot, bool bEnabled)
{
    if (bEnabled)            // We don't need to disable anything if it the control is enabled.
        return false;

    unsigned int uiIndex;
    if (GetAnalogControlIndex(szName, uiIndex))
    {
        if (bOnFoot)
        {
            switch (uiIndex)
            {
                case 0:
                    cs.LeftStickX = (cs.LeftStickX < 0) ? 0 : cs.LeftStickX;
                    return true;            // Left
                case 1:
                    cs.LeftStickX = (cs.LeftStickX > 0) ? 0 : cs.LeftStickX;
                    return true;            // Right
                case 2:
                    cs.LeftStickY = (cs.LeftStickY < 0) ? 0 : cs.LeftStickY;
                    return true;            // Up
                case 3:
                    cs.LeftStickY = (cs.LeftStickY > 0) ? 0 : cs.LeftStickY;
                    return true;            // Down
                default:
                    return false;
            }
        }
        else
        {
            switch (uiIndex)
            {
                case 4:
                    cs.LeftStickX = (cs.LeftStickX < 0) ? 0 : cs.LeftStickX;
                    return true;            // Left
                case 5:
                    cs.LeftStickX = (cs.LeftStickX > 0) ? 0 : cs.LeftStickX;
                    return true;            // Right
                case 6:
                    cs.LeftStickY = (cs.LeftStickY < 0) ? 0 : cs.LeftStickY;
                    return true;            // Up
                case 7:
                    cs.LeftStickY = (cs.LeftStickY > 0) ? 0 : cs.LeftStickY;
                    return true;            // Down
                case 8:
                    cs.ButtonCross = 0;
                    return true;            // Accel
                case 9:
                    cs.ButtonSquare = 0;
                    return true;            // Reverse
                case 10:
                    cs.RightStickX = (cs.RightStickX < 0) ? 0 : cs.RightStickX;
                    return true;            // Special Left
                case 11:
                    cs.RightStickX = (cs.RightStickX > 0) ? 0 : cs.RightStickX;
                    return true;            // Special Right
                case 12:
                    cs.RightStickY = (cs.RightStickY < 0) ? 0 : cs.RightStickY;
                    return true;            // Special Up
                case 13:
                    cs.RightStickY = (cs.RightStickY > 0) ? 0 : cs.RightStickY;
                    return true;            // Special Down
                default:
                    return false;
            }
        }
    }
    return false;
}
