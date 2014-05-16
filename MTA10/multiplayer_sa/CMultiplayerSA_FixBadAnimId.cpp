/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_HookDestructors.cpp
*  PORPOISE:    
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../game_sa/CAnimBlendAssocGroupSA.h"
#include "../game_sa/CAnimBlendAssociationSA.h"

std::set < CAnimBlendAssociationSAInterface* > ms_ValidAnimBlendAssociationMap;

////////////////////////////////////////////////////////////////////////////////////////////////
// Check for anims that will crash and change to one that wont. (The new anim will be wrong and look crap though)
int _cdecl OnCAnimBlendAssocGroupCopyAnimation ( CAnimBlendAssocGroupSAInterface* pGroup, int iAnimId )
{
    // Apply offset
    int iUseAnimId = iAnimId - pGroup->iIDOffset;

    if ( pGroup->pAssociationsArray )
    {
        CAnimBlendStaticAssociationSAInterface* pAssociation = pGroup->pAssociationsArray + iUseAnimId;
        if ( pAssociation && pAssociation->pAnimHeirarchy == NULL )
        {
            // Choose another animId
            int iNewAnimId = iUseAnimId;
            for ( int i = 0 ; i < pGroup->iNumAnimations ; i++ )
            {
                pAssociation = pGroup->pAssociationsArray + i;
                if ( pAssociation->pAnimHeirarchy )
                {
                    // Find closest valid anim id
                    if ( abs( iUseAnimId - i ) < abs( iUseAnimId - iNewAnimId ) || iNewAnimId == iUseAnimId )
                        iNewAnimId = i;
                }
            }

            iUseAnimId = iNewAnimId;
            LogEvent ( 534, "CopyAnimation", "", SString ( "Group:%d replaced id:%d with id:%d", pGroup->groupID, iAnimId, iUseAnimId + pGroup->iIDOffset ) );
        }
    }

    // Unapply offset
    iAnimId = iUseAnimId + pGroup->iIDOffset;
    return iAnimId;
}


// Hook info
#define HOOKPOS_CAnimBlendAssocGroupCopyAnimation        0x4CE130
#define HOOKSIZE_CAnimBlendAssocGroupCopyAnimation       6
DWORD RETURN_CAnimBlendAssocGroupCopyAnimation =         0x4CE136;
void _declspec(naked) HOOK_CAnimBlendAssocGroupCopyAnimation()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        push    ecx
        call    OnCAnimBlendAssocGroupCopyAnimation
        add     esp, 4*2
        mov     [esp+32+4*1],eax
        popad

        mov     eax,  fs:0
        jmp     RETURN_CAnimBlendAssocGroupCopyAnimation
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// CAnimBlendAssociation::Init
//
// Remember valid CAnimBlendAssociation
//
////////////////////////////////////////////////////////////////////////////////////////////////
void _cdecl OnCAnimBlendAssociation_Init( CAnimBlendAssociationSAInterface* pAnimBlendAssociation )
{
    MapInsert( ms_ValidAnimBlendAssociationMap, pAnimBlendAssociation );
}

// Hook info
#define HOOKPOS_CAnimBlendAssociation_Init        0x4CEEC0
#define HOOKSIZE_CAnimBlendAssociation_Init       8
DWORD RETURN_CAnimBlendAssociation_Init =         0x4CEEC8;
void _declspec(naked) HOOK_CAnimBlendAssociation_Init()
{
    _asm
    {
        pushad
        push    ecx
        call    OnCAnimBlendAssociation_Init
        add     esp, 4*1
        popad

        push    esi
        mov     esi, ecx
        push    edi
        mov     edi, [esp+4+8]
        jmp     RETURN_CAnimBlendAssociation_Init
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// CAnimBlendAssociation::CAnimBlendAssociation
//
// Forget valid CAnimBlendAssociation
//
////////////////////////////////////////////////////////////////////////////////////////////////
void _cdecl OnCAnimBlendAssociation_Destructor( CAnimBlendAssociationSAInterface* pAnimBlendAssociation )
{
    MapRemove( ms_ValidAnimBlendAssociationMap, pAnimBlendAssociation );
}

// Hook info
#define HOOKPOS_CAnimBlendAssociation_Destructor        0x4CECF0
#define HOOKSIZE_CAnimBlendAssociation_Destructor       6
DWORD RETURN_CAnimBlendAssociation_Destructor =         0x4CECF6;
void _declspec(naked) HOOK_CAnimBlendAssociation_Destructor()
{
    _asm
    {
        pushad
        push    ecx
        call    OnCAnimBlendAssociation_Destructor
        add     esp, 4*1
        popad

        push    esi
        mov     esi, ecx
        mov     eax, [esi+10h]
        jmp     RETURN_CAnimBlendAssociation_Destructor
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Check CAnimBlendAssociation is valid for use in CTaskSimpleCarFallOut::FinishAnimFallOutCB
//
//
//////////////////////////////////////////////////////////////////////////////////////////
bool IsValidAnimBlendAssociation( CAnimBlendAssociationSAInterface* pAnimBlendAssociation )
{
    if ( !MapContains( ms_ValidAnimBlendAssociationMap, pAnimBlendAssociation ) )
        return false;

    __try
    {
        DWORD* pAnimBlendNodeArray = pAnimBlendAssociation->pAnimBlendNodeArray;
        if ( pAnimBlendNodeArray == NULL )
            return true;

        DWORD dwSomething = pAnimBlendNodeArray[ 0x590 / sizeof( DWORD ) ];
        if ( dwSomething != NULL )
            return true;

        DWORD pDamageManager = pAnimBlendNodeArray[ 0x5A0 / sizeof( DWORD ) ];
        if ( pDamageManager == NULL )
            return false;
        return true;
    }
    __except( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION )
    {
        MapRemove( ms_ValidAnimBlendAssociationMap, pAnimBlendAssociation );
        return false;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_FixBadAnimId ( void )
{
   EZHookInstall ( CAnimBlendAssocGroupCopyAnimation );
   EZHookInstall ( CAnimBlendAssociation_Init );
   EZHookInstall ( CAnimBlendAssociation_Destructor );
}
