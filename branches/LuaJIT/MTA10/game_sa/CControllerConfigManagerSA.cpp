/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CControllerConfigManagerSA.cpp
*  PURPOSE:     Controller configuration manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define VAR_InputType       ( ( BYTE * ) ( 0xB6EC2E ) )
#define VAR_MouseInverted   ( ( BYTE * ) ( 0xBA6745 ) )
#define VAR_FlyWithMouse    ( ( BYTE * ) ( 0xC1CC03 ) )
#define VAR_SteerWithMouse  ( ( BYTE * ) ( 0xC1CC02 ) )
#define VAR_VerticalAimSensitivity  ( ( FLOAT * ) ( 0xB6EC18 ) )

static const float VERTICAL_AIM_SENSITIVITY_MIN     = 0.000312f;
static const float VERTICAL_AIM_SENSITIVITY_DEFAULT = 0.0015f;
static const float VERTICAL_AIM_SENSITIVITY_MAX     = VERTICAL_AIM_SENSITIVITY_DEFAULT * 2 - VERTICAL_AIM_SENSITIVITY_MIN;

CControllerConfigManagerSA::CControllerConfigManagerSA()
{
    m_bSuspendSteerAndFlyWithMouse = false;
    // Get initial settings
    m_bSteerWithMouse = *VAR_FlyWithMouse != 0;
    m_bFlyWithMouse = *VAR_SteerWithMouse != 0;
    MemSet( (void*)0x5BC7B4, 0x90, 10 );   // Stop vertical aim sensitivity value reset
}

void CControllerConfigManagerSA::SetControllerKeyAssociatedWithAction ( eControllerAction action, int iKey, eControllerType controllerType )
{
    DWORD dwFunc = FUNC_SetControllerKeyAssociatedWithAction;
    _asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    controllerType
        push    iKey
        push    action
        call    dwFunc
    }
}

int CControllerConfigManagerSA::GetControllerKeyAssociatedWithAction ( eControllerAction action, eControllerType controllerType )
{
    int iReturn = 0;
    DWORD dwFunc = FUNC_GetControllerKeyAssociatedWithAction;
    _asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    controllerType
        push    action
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

int CControllerConfigManagerSA::GetNumOfSettingsForAction ( eControllerAction action )
{
    int iReturn = 0;
    DWORD dwFunc = FUNC_GetNumOfSettingsForAction;
    _asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    action
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

void CControllerConfigManagerSA::ClearSettingsAssociatedWithAction ( eControllerAction action, eControllerType controllerType )
{
    DWORD dwFunc = FUNC_ClearSettingsAssociatedWithAction;
    _asm
    {
        mov     ecx, CLASS_CControllerConfigManager
        push    controllerType
        push    action
        call    dwFunc
    }
}

void CControllerConfigManagerSA::SetClassicControls ( bool bClassicControls )
{
    MemPutFast < unsigned char > ( VAR_InputType, bClassicControls ? 0 : 1 );
}

void CControllerConfigManagerSA::SetMouseInverted ( bool bInverted )
{
    MemPutFast < BYTE > ( VAR_MouseInverted, ( bInverted ) ? 0 : 1 );
}

void CControllerConfigManagerSA::SetFlyWithMouse ( bool bFlyWithMouse )
{
    m_bFlyWithMouse = bFlyWithMouse;
    ApplySteerAndFlyWithMouseSettings ();
}

void CControllerConfigManagerSA::SetSteerWithMouse ( bool bSteerWithMouse )
{
    m_bSteerWithMouse = bSteerWithMouse;
    ApplySteerAndFlyWithMouseSettings ();
}

void CControllerConfigManagerSA::SuspendSteerAndFlyWithMouse ( bool bSuspend )
{
    m_bSuspendSteerAndFlyWithMouse = bSuspend;
    ApplySteerAndFlyWithMouseSettings ();
}

void CControllerConfigManagerSA::ApplySteerAndFlyWithMouseSettings ( void )
{
    if ( m_bSuspendSteerAndFlyWithMouse )
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

float CControllerConfigManagerSA::GetVerticalAimSensitivity ( )
{
    float fRawValue = GetVerticalAimSensitivityRawValue();
    return UnlerpClamped( VERTICAL_AIM_SENSITIVITY_MIN, fRawValue, VERTICAL_AIM_SENSITIVITY_MAX );    // Remap to 0-1
}

void CControllerConfigManagerSA::SetVerticalAimSensitivity ( float fSensitivity )
{
    float fRawValue = Lerp( VERTICAL_AIM_SENSITIVITY_MIN, fSensitivity, VERTICAL_AIM_SENSITIVITY_MAX );
    SetVerticalAimSensitivityRawValue( fRawValue );
}

// Raw value used for saving so range can be changed without affecting user setting
float CControllerConfigManagerSA::GetVerticalAimSensitivityRawValue ( )
{
    return *(FLOAT *)VAR_VerticalAimSensitivity;
}

void CControllerConfigManagerSA::SetVerticalAimSensitivityRawValue ( float fRawValue )
{
    MemPutFast < FLOAT > ( VAR_VerticalAimSensitivity, fRawValue );
}
