/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFxManagerSA.cpp
*  PURPOSE:     Game effects handling
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


CFxSystem* CFxManagerSA::CreateFxSystem( const char * szBlueprint, const CVector & vecPosition, RwMatrix * pRwMatrixTag, unsigned char bSkipCameraFrustumCheck )
{
    const CVector * pvecPosition = &vecPosition;
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_FxManager_c__CreateFxSystem;
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        push    bSkipCameraFrustumCheck
        push    pRwMatrixTag
        push    pvecPosition
        push    szBlueprint
        call    dwFunc
        mov     dwReturn, eax
    }
    if ( dwReturn != 0 )
    {
        CFxSystemSA* pFxSystemSA = new CFxSystemSA((CFxSystemSAInterface*)dwReturn);
        return pFxSystemSA;
    }
    else
        return NULL;
}

void CFxManagerSA::DestroyFxSystem(CFxSystem* pFxSystem)
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_FxManager_c__DestroyFxSystem;

    void * pFxSA = pFxSystem->GetInterface();

    _asm
    {
        mov     ecx, dwThis
        push    pFxSA
        call    dwFunc
    }
}

//
// Called when GTA deletes an FxSystem
//
void CFxManagerSA::OnFxSystemSAInterfaceDestroyed ( CFxSystemSAInterface* pFxSystemSAInterface )
{
    // Delete our wrapper object if we have one
    CFxSystemSA* pFxSystemSA = GetFxSystem( pFxSystemSAInterface );
    if ( pFxSystemSA )
        delete pFxSystemSA;
}

//
// AddToList/RemoveFromList called from CFxSystemSA constructor/destructor
//
void CFxManagerSA::AddToList( CFxSystemSAInterface* pFxSystemSAInterface, CFxSystemSA* pFxSystemSA )
{
    MapSet( m_FxInterfaceMap, pFxSystemSAInterface, pFxSystemSA );
}

void CFxManagerSA::RemoveFromList( CFxSystemSA* pFxSystemSA )
{
    MapRemoveByValue( m_FxInterfaceMap, pFxSystemSA );
}

//
// Find our wrapper object for the GTA object
//
CFxSystemSA* CFxManagerSA::GetFxSystem( CFxSystemSAInterface* pFxSystemSAInterface )
{
    return MapFindRef( m_FxInterfaceMap, pFxSystemSAInterface );
}
