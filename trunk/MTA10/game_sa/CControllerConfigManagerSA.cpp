/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CControllerConfigManagerSA.cpp
*  PURPOSE:		Controller configuration manager
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CControllerConfigManagerSA::CControllerConfigManagerSA()
{
	

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

unsigned char CControllerConfigManagerSA::GetInputType ( void )
{
    return * ( unsigned char * ) ( 0xB6EC2E );
}

void CControllerConfigManagerSA::SetInputType ( unsigned char ucInputType )
{
    * ( unsigned char * ) ( 0xB6EC2E ) = ucInputType;
}

bool CControllerConfigManagerSA::IsMouseInverted ( void )
{
    return ( * ( unsigned char * ) ( 0xBA6745 ) == 0 );
}

void CControllerConfigManagerSA::SetMouseInverted ( bool bInverted )
{
    * ( unsigned char * ) ( 0xBA6745 ) = ( bInverted ) ? 0 : 1;
}

bool CControllerConfigManagerSA::GetFlyWithMouse ( void )
{
    return ( * ( unsigned char * ) ( 0xC1CC03 ) == 1 );
}

void CControllerConfigManagerSA::SetFlyWithMouse ( bool bFlyWithMouse )
{
    * ( unsigned char * ) ( 0xC1CC03 ) = ( bFlyWithMouse ) ? 1 : 0;
}

bool CControllerConfigManagerSA::GetSteerWithMouse ( void )
{
    return ( * ( unsigned char * ) ( 0xC1CC02 ) == 1 );
}

void CControllerConfigManagerSA::SetSteerWithMouse ( bool bSteerWithMouse )
{
    * ( unsigned char * ) ( 0xC1CC02 ) = ( bSteerWithMouse ) ? 1 : 0;
}