/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CControllerConfigManagerSA.cpp
 *  PURPOSE:     Controller configuration manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CControllerConfigManagerSA.h"

#define VAR_InputType                  ((BYTE*)(0xB6EC2E))
#define VAR_MouseInverted              ((BYTE*)(0xBA6745))
#define VAR_FlyWithMouse               ((BYTE*)(0xC1CC03))
#define VAR_SteerWithMouse             ((BYTE*)(0xC1CC02))
#define VAR_VerticalAimSensitivity     ((float*)(0xB6EC18))
#define VAR_HorizontalMouseSensitivity 0xB6EC1C

static const float VERTICAL_AIM_SENSITIVITY_MIN = 0.000312f;
static const float VERTICAL_AIM_SENSITIVITY_DEFAULT = 0.0015f;
static const float VERTICAL_AIM_SENSITIVITY_MAX = VERTICAL_AIM_SENSITIVITY_DEFAULT * 2 - VERTICAL_AIM_SENSITIVITY_MIN;

CControllerConfigManagerSA::CControllerConfigManagerSA()
{
    m_bSuspendSteerAndFlyWithMouse = false;
    // Get initial settings
    m_bSteerWithMouse = *VAR_FlyWithMouse != 0;
    m_bFlyWithMouse = *VAR_SteerWithMouse != 0;
    MemSet((void*)0x5BC7B4, 0x90, 10);  // Stop vertical aim sensitivity value reset
}

void CControllerConfigManagerSA::SetControllerKeyAssociatedWithAction(eControllerAction action, int iKey, eControllerType controllerType)
{
    DWORD dwFunc = FUNC_SetControllerKeyAssociatedWithAction;
    // clang-format off
    __asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    controllerType
        push    iKey
        push    action
        call    dwFunc
    }
    // clang-format on
}

int CControllerConfigManagerSA::GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType controllerType)
{
    int   iReturn = 0;
    DWORD dwFunc = FUNC_GetControllerKeyAssociatedWithAction;
    // clang-format off
    __asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    controllerType
        push    action
        call    dwFunc
        mov     iReturn, eax
    }
    // clang-format on
    return iReturn;
}

int CControllerConfigManagerSA::GetNumOfSettingsForAction(eControllerAction action)
{
    int   iReturn = 0;
    DWORD dwFunc = FUNC_GetNumOfSettingsForAction;
    // clang-format off
    __asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    action
        call    dwFunc
        mov     iReturn, eax
    }
    // clang-format on
    return iReturn;
}

void CControllerConfigManagerSA::ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType controllerType)
{
    DWORD dwFunc = FUNC_ClearSettingsAssociatedWithAction;
    // clang-format off
    __asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    controllerType
        push    action
        call    dwFunc
    }
    // clang-format on
}

void CControllerConfigManagerSA::SetClassicControls(bool bClassicControls)
{
    MemPutFast<unsigned char>(VAR_InputType, bClassicControls ? 0 : 1);
}

void CControllerConfigManagerSA::SetMouseInverted(bool bInverted)
{
    MemPutFast<BYTE>(VAR_MouseInverted, (bInverted) ? 0 : 1);
}

void CControllerConfigManagerSA::SetFlyWithMouse(bool bFlyWithMouse)
{
    m_bFlyWithMouse = bFlyWithMouse;
    ApplySteerAndFlyWithMouseSettings();
}

void CControllerConfigManagerSA::SetSteerWithMouse(bool bSteerWithMouse)
{
    m_bSteerWithMouse = bSteerWithMouse;
    ApplySteerAndFlyWithMouseSettings();
}

void CControllerConfigManagerSA::SuspendSteerAndFlyWithMouse(bool bSuspend)
{
    m_bSuspendSteerAndFlyWithMouse = bSuspend;
    ApplySteerAndFlyWithMouseSettings();
}

void CControllerConfigManagerSA::ApplySteerAndFlyWithMouseSettings()
{
    if (m_bSuspendSteerAndFlyWithMouse)
    {
        *VAR_FlyWithMouse = 0;
        *VAR_SteerWithMouse = 0;
    }
    else
    {
        *VAR_FlyWithMouse = m_bFlyWithMouse;
        *VAR_SteerWithMouse = m_bSteerWithMouse;
    }
}

float CControllerConfigManagerSA::GetVerticalAimSensitivity()
{
    float fRawValue = GetVerticalAimSensitivityRawValue();
    return UnlerpClamped(VERTICAL_AIM_SENSITIVITY_MIN, fRawValue, VERTICAL_AIM_SENSITIVITY_MAX);  // Remap to 0-1
}

void CControllerConfigManagerSA::SetVerticalAimSensitivity(float fSensitivity)
{
    float fRawValue = Lerp(VERTICAL_AIM_SENSITIVITY_MIN, fSensitivity, VERTICAL_AIM_SENSITIVITY_MAX);
    SetVerticalAimSensitivityRawValue(fRawValue);
}

// Raw value used for saving so range can be changed without affecting user setting
float CControllerConfigManagerSA::GetVerticalAimSensitivityRawValue()
{
    return *(float*)VAR_VerticalAimSensitivity;
}

void CControllerConfigManagerSA::SetVerticalAimSensitivityRawValue(float fRawValue)
{
    MemPutFast<float>(VAR_VerticalAimSensitivity, fRawValue);
}

void CControllerConfigManagerSA::SetVerticalAimSensitivitySameAsHorizontal(bool enabled)
{
    std::uintptr_t varToUse = enabled ? VAR_HorizontalMouseSensitivity : reinterpret_cast<std::uintptr_t>(VAR_VerticalAimSensitivity);

    MemPut<std::uintptr_t>(0x50F048, varToUse);  // CCam::Process_1rstPersonPedOnPC
    MemPut<std::uintptr_t>(0x50FB28, varToUse);  // CCam::Process_FollowPedWithMouse
    MemPut<std::uintptr_t>(0x510C28, varToUse);  // CCam::Process_M16_1stPerson
    MemPut<std::uintptr_t>(0x511E0A, varToUse);  // CCam::Process_Rocket
    MemPut<std::uintptr_t>(0x52228E, varToUse);  // CCam::Process_AimWeapon
}
