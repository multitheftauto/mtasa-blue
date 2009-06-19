/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFxSystemSA.cpp
*  PURPOSE:		Game effects system
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CFxSystemSA::Play ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_Play;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CFxSystemSA::Pause ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_Pause;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CFxSystemSA::Stop ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_Stop;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CFxSystemSA::PlayAndKill ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_PlayAndKill;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CFxSystemSA::Kill ( void )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_Kill;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CFxSystemSA::AttachToBone ( CEntity * pEntity, int iBone )
{
    DWORD dwEntityInterface = ( DWORD ) pEntity->GetInterface ();
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_AttachToBone;
    _asm
    {
        mov     ecx, dwThis
        push    iBone
        push    dwEntityInterface
        call    dwFunc
    }
}


void CFxSystemSA::SetMatrix ( RwMatrix * pMatrix )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_SetMatrix;
    _asm
    {
        mov     ecx, dwThis
        push    pMatrix
        call    dwFunc
    }
}


void CFxSystemSA::SetLocalParticles ( unsigned char uc_1 )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_SetLocalParticles;
    _asm
    {
        mov     ecx, dwThis
        push    uc_1
        call    dwFunc
    }
}


void CFxSystemSA::SetConstTime ( unsigned char uc_1, float fTime )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_SetConstTime;
    _asm
    {
        mov     ecx, dwThis
        push    fTime
        push    uc_1
        call    dwFunc
    }
}


int CFxSystemSA::GetPlayStatus ( void )
{
    int iReturn;
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CFxSystem_GetPlayStatus;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}