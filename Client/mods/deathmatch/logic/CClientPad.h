/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPad.h
 *  PURPOSE:     Ped entity controller pad handler class
 *
 *****************************************************************************/

#pragma once

#define MAX_GTA_CONTROLS 44
#define MAX_GTA_ANALOG_CONTROLS 14

class CClientPed;
class CControllerState;

class CClientPad
{
public:
    static bool        GetControlIndex(const char* szName, unsigned int& uiIndex);
    static const char* GetControlName(unsigned int uiIndex);

    static bool GetAnalogControlIndex(const char* szName, unsigned int& uiIndex);
    static void InitAnalogControlStates();

    CClientPad();

    bool GetControlState(const char* szName, bool& bState);
    bool SetControlState(const char* szName, bool bState);

    bool GetControlState(const char* szName, float& fState);
    bool SetControlState(const char* szName, float fState);

    void DoPulse(CClientPed* pPed);

    static bool GetAnalogControlState(const char* szName, CControllerState& cs, bool bOnFoot, float& fState, bool bIgnoreOverrides);
    static bool SetAnalogControlState(const char* szName, float fState, bool bFrameForced);
    static void RemoveSetAnalogControlState(const char* szName);

    static void ProcessSetAnalogControlState(CControllerState& cs, bool bOnFoot);
    static void ProcessControl(short& usControlValue, unsigned int uiIndex);

    static void ProcessAllToggledControls(CControllerState& cs, bool bOnFoot);
    static bool ProcessToggledControl(const char* szName, CControllerState& cs, bool bOnFoot, bool bEnabled);
    static bool GetControlState(const char* szName, CControllerState& State, bool bOnFoot);

    static SFixedArray<short, MAX_GTA_CONTROLS>       m_sScriptedStates;
    static SFixedArray<bool, MAX_GTA_ANALOG_CONTROLS> m_bScriptedStatesNextFrameOverride;
    static bool                                       m_bFlyWithMouse;
    static bool                                       m_bSteerWithMouse;

protected:
    SFixedArray<float, MAX_GTA_CONTROLS> m_fStates;
};
