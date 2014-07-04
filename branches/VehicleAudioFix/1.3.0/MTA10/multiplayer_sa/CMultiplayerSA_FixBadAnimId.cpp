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

////////////////////////////////////////////////////////////////////////////////////////////////
// Check for anims that will crash and change to one that wont. (The new anim will be wrong and look crap though)
int _cdecl OnCAnimBlendAssocGroupCopyAnimation ( CAnimBlendAssocGroupSAInterface* pGroup, int iAnimId )
{
    // Apply offset
    int iUseAnimId = iAnimId - pGroup->iIDOffset;

    CAnimBlendStaticAssociationSAInterface* pAssociation = pGroup->pAssociationsArray + iUseAnimId;

    if ( pAssociation )
    {
        if ( pAssociation->pAnimHeirarchy == NULL )
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


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hook
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_FixBadAnimId ( void )
{
   EZHookInstall ( CAnimBlendAssocGroupCopyAnimation );
}
