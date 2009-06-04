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