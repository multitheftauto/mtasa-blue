/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CControllerConfigManagerSA.h
 *  PURPOSE:     Header file for controller configuration manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CControllerConfigManager.h>

#define FUNC_SetControllerKeyAssociatedWithAction       0x530490
#define FUNC_GetControllerKeyAssociatedWithAction       0x52F4F0
#define FUNC_GetNumOfSettingsForAction                  0x52F4A0
#define FUNC_ClearSettingsAssociatedWithAction          0x52FD70

#define CLASS_CControllerConfigManager                  0xB70198

class CControllerConfigManagerSA : public CControllerConfigManager
{
public:
    CControllerConfigManagerSA();
    void  SetControllerKeyAssociatedWithAction(eControllerAction action, int iKey, eControllerType controllerType);
    int   GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType controllerType);
    int   GetNumOfSettingsForAction(eControllerAction action);
    void  ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType controllerType);
    void  SetClassicControls(bool bClassicControls);
    void  SetMouseInverted(bool bInverted);
    void  SetFlyWithMouse(bool bFlyWithMouse);
    void  SetSteerWithMouse(bool bSteerWithMouse);
    void  SuspendSteerAndFlyWithMouse(bool bSuspend);
    float GetVerticalAimSensitivity();
    void  SetVerticalAimSensitivity(float fSensitivity);
    float GetVerticalAimSensitivityRawValue();
    void  SetVerticalAimSensitivityRawValue(float fRawValue);

    // CControllerConfigManagerSA
    void ApplySteerAndFlyWithMouseSettings();

protected:
    bool m_bSteerWithMouse;
    bool m_bFlyWithMouse;
    bool m_bSuspendSteerAndFlyWithMouse;
};
