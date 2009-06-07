/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAnimBlendAssociationSA.cpp
*  PURPOSE:		Animation blend association
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA * pGame;

CAnimBlendAssociationSA::CAnimBlendAssociationSA ( CAnimBlendAssociationSAInterface * pInterface )
{
    m_pInterface = pInterface;
    m_DeleteCallback = NULL;
    m_FinishCallback = NULL;
    m_pDeleteCallbackData = NULL;
    m_pFinishCallbackData = NULL;
}


CAnimBlendHierarchy * CAnimBlendAssociationSA::GetAnimHierarchy ( void )
{
    return pGame->GetAnimManager ()->GetAnimBlendHierarchy ( m_pInterface->pAnimHierarchy );
}


void CAnimBlendAssociationSA::StaticDeleteCallback ( CAnimBlendAssociationSAInterface * pInterface, void * pData )
{
    CAnimBlendAssociationSA * pAssoc = reinterpret_cast < CAnimBlendAssociationSA * > ( pData );
    if ( pAssoc->m_pInterface == pInterface )
    {
        if ( pAssoc->m_DeleteCallback ) pAssoc->m_DeleteCallback ( pAssoc, pAssoc->m_pDeleteCallbackData );
    }
}


void CAnimBlendAssociationSA::SetDeleteCallback ( CALLBACK_CAnimBlendAssoc Callback, void * pCallbackData )
{
    m_DeleteCallback = Callback;
    m_pDeleteCallbackData = pCallbackData;

    DWORD dwCallback = ( DWORD ) CAnimBlendAssociationSA::StaticDeleteCallback;
    void * pData = this;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_SetDeleteCallback;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        or      byte ptr [ecx+2Fh],20h
        push    pData
        push    dwCallback
        call    dwFunc
    }
}


void CAnimBlendAssociationSA::StaticFinishCallback ( CAnimBlendAssociationSAInterface * pInterface, void * pData )
{
    CAnimBlendAssociationSA * pAssoc = reinterpret_cast < CAnimBlendAssociationSA * > ( pData );
    if ( pAssoc->m_pInterface == pInterface )
    {
        if ( pAssoc->m_FinishCallback ) pAssoc->m_FinishCallback ( pAssoc, pAssoc->m_pFinishCallbackData );
    }
}


void CAnimBlendAssociationSA::SetFinishCallback ( CALLBACK_CAnimBlendAssoc Callback, void * pCallbackData )
{
    m_FinishCallback = Callback;
    m_pFinishCallbackData = pCallbackData;

    DWORD dwCallback = ( DWORD ) CAnimBlendAssociationSA::StaticFinishCallback;
    void * pData = this;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_SetFinishCallback;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    pData
        push    dwCallback
        call    dwFunc
    }
}


bool CAnimBlendAssociationSA::IsFlagSet ( unsigned int uiFlag )
{
    bool bReturn;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_IsFlagSet;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    uiFlag
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}



void CAnimBlendAssociationSA::SetFlag ( unsigned int uiFlag )
{
    DWORD dwFunc = FUNC_CAnimBlendAssociation_SetFlag;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    uiFlag
        call    dwFunc
    }
}


void CAnimBlendAssociationSA::ClearFlag ( unsigned int uiFlag )
{
    DWORD dwFunc = FUNC_CAnimBlendAssociation_ClearFlag;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    uiFlag
        call    dwFunc
    }
}


float CAnimBlendAssociationSA::GetSpeed ( void )
{
    float fSpeed;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_GetSpeed;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fSpeed
    }
    return fSpeed;
}


void CAnimBlendAssociationSA::SetSpeed ( float fSpeed )
{
    DWORD dwFunc = FUNC_CAnimBlendAssociation_SetSpeed;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    fSpeed
        call    dwFunc
    }
}


float CAnimBlendAssociationSA::GetTime ( void )
{
    float fCurrentTime;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_GetCurrentTime;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fCurrentTime
    }
    return fCurrentTime;
}


void CAnimBlendAssociationSA::SetTime ( float fCurrentTime )
{
    DWORD dwFunc = FUNC_CAnimBlendAssociation_SetCurrentTime;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    fCurrentTime
        call    dwFunc
    }
}


float CAnimBlendAssociationSA::GetTotalTime ( void )
{
    float fTotalTime;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_GetTotalTime;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fTotalTime
    }
    return fTotalTime;
}


float CAnimBlendAssociationSA::GetTimeStep ( void )
{
    float fTimeStep;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_GetTimeStep;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fTimeStep
    }
    return fTimeStep;
}


void CAnimBlendAssociationSA::UpdateTimeStep ( float fUnk, float fUnk2 )
{
    DWORD dwFunc = FUNC_CAnimBlendAssociation_UpdateTimeStep;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    fUnk
        push    fUnk2
        call    dwFunc
    }
}


bool CAnimBlendAssociationSA::UpdateBlend ( float fUnk )
{
    bool bReturn;
    DWORD dwFunc = FUNC_CAnimBlendAssociation_UpdateBlend;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    fUnk
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


void CAnimBlendAssociationSA::SyncAnimation ( CAnimBlendAssociation * pAssoc )
{
    DWORD dwFunc = FUNC_CAnimBlendAssociation_SyncAnimation;
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwAssoc = ( DWORD ) pAssoc->GetInterface ();
    _asm
    {
        mov     ecx, dwThis
        push    dwAssoc
        call    dwFunc
    }
}