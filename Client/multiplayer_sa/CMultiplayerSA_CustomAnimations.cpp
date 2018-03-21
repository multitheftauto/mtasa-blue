#include "StdInc.h"

#include <../game_sa/CAnimBlendHierarchySA.h> 
#include <../game_sa/CAnimBlendStaticAssociationSA.h>
#include <../game_sa/CAnimBlendAssociationSA.h>
#include <../game_sa/CAnimBlendAssocGroupSA.h>

typedef std::map < CAnimBlendAssociationSAInterface *, SIFPAnimations * > AnimAssociations_type;
static AnimAssociations_type mapOfCustomAnimationAssociations;

DWORD FUNC_NEW_OPERATOR = 0x082119A;
DWORD FUNC_CAnimBlendAssociation_Constructor = 0x04CF080;

DWORD RETURN_CAnimBlendAssoc_Hierarchy_Constructor =        0x4CEFC5;
DWORD RETURN_CAnimBlendAssoc_destructor =                   0x4CECF6;
DWORD RETURN_CAnimBlendAssocGroup_CopyAnimation_NORMALFLOW =0x4CE151; 
DWORD RETURN_CAnimBlendAssocGroup_CopyAnimation =           0x4CE187; 
DWORD RETURN_CAnimBlendAssocGroup_CopyAnimation_ERROR =     0x4CE199;
DWORD RETURN_CAnimManager_AddAnimation =                    0x4D3AB1;
DWORD RETURN_CAnimManager_AddAnimationAndSync =             0x4D3B41;
DWORD RETURN_CAnimManager_BlendAnimation_Hierarchy =        0x4D4417; 

CAnimBlendAssocHierConstructorHandler * m_pCAnimBlendAssocHierConstructorHandler = nullptr;
CAnimBlendAssocDestructorHandler * m_pCAnimBlendAssocDestructorHandler = nullptr;
AddAnimationHandler * m_pAddAnimationHandler = nullptr;
AddAnimationAndSyncHandler * m_pAddAnimationAndSyncHandler = nullptr;
AssocGroupCopyAnimationHandler * m_pAssocGroupCopyAnimationHandler = nullptr;
BlendAnimationHierarchyHandler * m_pBlendAnimationHierarchyHandler = nullptr;

int _cdecl OnCAnimBlendAssocGroupCopyAnimation ( AssocGroupId animGroup, int iAnimId );
auto CAnimBlendStaticAssociation_FreeSequenceArray = (hCAnimBlendStaticAssociation_FreeSequenceArray)0x4ce9a0;

void CMultiplayerSA::SetCAnimBlendAssocHierConstructorHandler ( CAnimBlendAssocHierConstructorHandler * pHandler )
{
    m_pCAnimBlendAssocHierConstructorHandler = pHandler;
}

void CMultiplayerSA::SetCAnimBlendAssocDestructorHandler ( CAnimBlendAssocDestructorHandler * pHandler )
{
    m_pCAnimBlendAssocDestructorHandler = pHandler;
}

void CMultiplayerSA::SetAddAnimationHandler ( AddAnimationHandler * pHandler )
{
    m_pAddAnimationHandler = pHandler;
}

void CMultiplayerSA::SetAddAnimationAndSyncHandler ( AddAnimationAndSyncHandler * pHandler )
{
    m_pAddAnimationAndSyncHandler = pHandler;
}

void CMultiplayerSA::SetAssocGroupCopyAnimationHandler ( AssocGroupCopyAnimationHandler * pHandler )
{
    m_pAssocGroupCopyAnimationHandler = pHandler;
}

void CMultiplayerSA::SetBlendAnimationHierarchyHandler ( BlendAnimationHierarchyHandler * pHandler )
{
    m_pBlendAnimationHierarchyHandler = pHandler;
}

void DeleteIFPAnimations ( SIFPAnimations * pIFPAnimations )
{
    if ( pIFPAnimations->iReferences == 0 )
    { 
        printf("CClientGame::UnloadIFPAnimations (): iReferences are Zero\n");

        hCMemoryMgr_Free OLD_CMemoryMgr_Free = (hCMemoryMgr_Free)0x0072F430;
        auto OLD_CAnimBlendHierarchy_RemoveFromUncompressedCache = (hCAnimBlendHierarchy_RemoveFromUncompressedCache)0x004D42A0;

        for ( size_t i = 0; i < pIFPAnimations->vecAnimations.size(); i++ )
        {
            IFP_Animation * ifpAnimation = &pIFPAnimations->vecAnimations[i];
        
            OLD_CAnimBlendHierarchy_RemoveFromUncompressedCache ( (int)&ifpAnimation->Hierarchy );
            
            for (unsigned short SequenceIndex = 0; SequenceIndex < ifpAnimation->Hierarchy.m_nSeqCount; SequenceIndex++)
            {
                _CAnimBlendSequence * pSequence = (_CAnimBlendSequence*)((BYTE*)ifpAnimation->Hierarchy.m_pSequences + (sizeof(_CAnimBlendSequence) * SequenceIndex));

                if ( !( (pSequence->m_nFlags >> 3) & 1 ) ) // If ( !OneBigChunkForAllSequences )
                {
                    OLD_CMemoryMgr_Free ( pSequence->m_pFrames ); //*(void **)(pThis + 8)); //pSequence->m_pFrames );
                }
                else
                {
                    if ( SequenceIndex == 0 )
                    { 
                        // All frames of all sequences are allocated on one memory block, so free that one
                        // and break the loop 
                        OLD_CMemoryMgr_Free ( pSequence->m_pFrames );
                        break;
                    }
                }
  
            }
            delete ifpAnimation->pSequencesMemory;  
        }
        delete pIFPAnimations;
        printf("CClientGame::UnloadIFPAnimations (): IFP Animations have been unloaded successfully!\n");
    }
}

void __cdecl InsertAnimationAssociationToMap ( CAnimBlendAssociationSAInterface * pAnimAssociation, SIFPAnimations * pIFPAnimations )
{
    // We don't increment pIFPAnimations->iReferences here because it's done 
    // in custom animation handler functions in CClientGame.cpp
    mapOfCustomAnimationAssociations [ pAnimAssociation ] = pIFPAnimations;

    printf("InsertAnimationAssociationToMap: sAnimID: %d | iReferences: %d \n", pAnimAssociation->sAnimID, pIFPAnimations->iReferences);
}

void __cdecl RemoveAnimationAssociationFromMap ( CAnimBlendAssociationSAInterface * pAnimAssociation )
{
    AnimAssociations_type::iterator it;
    it = mapOfCustomAnimationAssociations.find ( pAnimAssociation );
    if ( it != mapOfCustomAnimationAssociations.end ( ) )
    {
        it->second->iReferences --;
        if ( ( it->second->bUnloadOnZeroReferences == true ) && ( it->second->iReferences == 0 ) )
        {
            // iReferences are zero, custom animation hierarchies are not being used anywhere.
            // It's safe to unload IFP animations here.
            DeleteIFPAnimations ( it->second );
        }
        mapOfCustomAnimationAssociations.erase ( pAnimAssociation );
    }
}

void _declspec(naked) HOOK_CAnimBlendAssoc_Hierarchy_Constructor ()
{
    _asm
    {
        pushad
    }
    
    if ( m_pCAnimBlendAssocHierConstructorHandler )
    {
        _asm
        {
            popad
            push    edx
            push    ecx
  
            push    ebp   
            mov     ebp, esp
            sub     esp, 4

            mov     eax, dword ptr [esp+18h] // pAnimHierarchy  [esp+0Ch]
            push    eax
            mov     eax, dword ptr [esp+18h] // pClump          [esp+0Ch]
            push    eax 
            push    ecx                      // this
            lea     edx, [ebp-4]             
            push    edx
            call    m_pCAnimBlendAssocHierConstructorHandler
            add     esp, 10h
            
            // It's a custom animation, store it in a map
            mov     edx, [ebp-4]

            add     esp, 4 // remove space for local var
            mov     esp, ebp
            pop     ebp
          
            // Check if it's a custom animation or not
            cmp     eax, 00
            je      NOT_CUSTOM_ANIMATION_CAnimBlendAssoc_Hierarchy_Constructor  

            push    edx // pIFPAnimations
            mov     ecx, [esp+4]
            push    ecx // pAnimAssociation
            call    InsertAnimationAssociationToMap
            add     esp, 8
            
            NOT_CUSTOM_ANIMATION_CAnimBlendAssoc_Hierarchy_Constructor:
            pop     ecx
            pop     edx
            pushad
            jmp     NORMAL_FLOW_CAnimBlendAssoc_Hierarchy_Constructor
        }
    }

    _asm
    {
        NORMAL_FLOW_CAnimBlendAssoc_Hierarchy_Constructor:
        popad
        xor     eax, eax
        push    esi
        mov     esi, ecx
        jmp     RETURN_CAnimBlendAssoc_Hierarchy_Constructor
    }
}

void CAnimBlendAssoc_destructor ( CAnimBlendAssociationSAInterface * pThis )
{
    RemoveAnimationAssociationFromMap ( pThis );

    if ( m_pCAnimBlendAssocDestructorHandler )
    {
        m_pCAnimBlendAssocDestructorHandler ( pThis );
    }
}

void _declspec(naked) HOOK_CAnimBlendAssoc_destructor ()
{
    _asm
    {
        push    ecx  // this
        call    CAnimBlendAssoc_destructor
        pop     ecx
        push    esi
        mov     esi, ecx
        mov     eax, [esi+10h]
        jmp     RETURN_CAnimBlendAssoc_destructor
    }
}

CAnimBlendStaticAssociationSAInterface * __cdecl AllocateStaticAssociationMemory ( void )
{
    return new CAnimBlendStaticAssociationSAInterface;
}

void __cdecl DeleteStaticAssociation ( CAnimBlendStaticAssociationSAInterface * pAnimStaticAssoc )
{
    CAnimBlendStaticAssociation_FreeSequenceArray ( pAnimStaticAssoc );
    delete pAnimStaticAssoc;
}

void _declspec(naked) HOOK_CAnimBlendAssocGroup_CopyAnimation ()
{
    _asm
    {
        pushad
    }

    if ( m_pAssocGroupCopyAnimationHandler )
    {
        _asm
        {
            popad                        
 
            push    ecx
            push    ebp   
            mov     ebp, esp
            sub esp, 4

            push    eax
            push    ecx
            push    edi
            
            lea     eax, [ebp-4]
            push    eax 

            // Allocate memory for our new static association
            call    AllocateStaticAssociationMemory
            mov     edi, eax

            // push the static association
            push    edi
            call    m_pAssocGroupCopyAnimationHandler 
            add     esp, 14h
            
            mov     ecx, [ebp-4] 

            add     esp, 4 // remove space for local var
            mov     esp, ebp
            pop     ebp

            // save eax and ecx for later to check whether current animation is custom or not 
            // after calling FUNC_CAnimBlendAssociation_Constructor function
            push    eax // isCustomAnimation ( bool )
            push    ecx // pIFPAnimations

            // get "this" from stack that we pushed first
            mov     ecx, [esp+8]
            mov     ecx, [ecx+4]
            sub     eax, edx
            push    esi

            // copy the static association to esi
            mov     esi, edi 
            test    esi, esi
            jz      ERROR_CopyAnimation
            mov     eax, [esi+10h]
            push    eax
            mov     eax, 04D41C0h
            call    eax
            push    3Ch   
            call    FUNC_NEW_OPERATOR
            add     esp, 8
            mov     [esp+20h], eax 
            test    eax, eax
            mov     [esp+18h], 0   
            jz      ERROR_CopyAnimation
            push    esi
            mov     ecx, eax
            call    FUNC_CAnimBlendAssociation_Constructor
            mov     edi, eax

            // Delete our static association, since we no longer need it 
            push    esi
            call    DeleteStaticAssociation
            add     esp, 4 
            
            mov     ecx, [esp+4] // pIFPAnimations
            mov     eax, [esp+8] // pAnimAssociation
            
            // Check wether this is a custom animation or not 
            cmp     eax, 00
            je      NOT_CUSTOM_ANIMATION_CopyAnimation  

            // It's a custom animation, store it in a map
            push    ecx // pIFPAnimations
            push    edi // pAnimAssociation
            call    InsertAnimationAssociationToMap
            add     esp, 8

            NOT_CUSTOM_ANIMATION_CopyAnimation: 
            // put CAnimBlendAssociation in eax
            mov     eax, edi
            add     esp, 0Ch
            jmp     RETURN_CAnimBlendAssocGroup_CopyAnimation

            ERROR_CopyAnimation:
            add     esp, 0Ch
            // Delete our static association
            push    edi
            call    DeleteStaticAssociation
            add     esp, 4
            jmp    RETURN_CAnimBlendAssocGroup_CopyAnimation_ERROR
        }
    }

    _asm
    {
        popad
        mov     ecx, [ecx+4]
        sub     eax, edx
        jmp RETURN_CAnimBlendAssocGroup_CopyAnimation_NORMALFLOW
    }
}

void _declspec(naked) HOOK_CAnimManager_AddAnimation ()
{
    _asm
    {
        pushad
    }

    if ( m_pAddAnimationHandler )
    {
        _asm
        {    
            popad
            mov     ecx, [esp+4]  // animationClump
            mov     edx, [esp+8]  // animationGroup
            mov     eax, [esp+12] // animationID
            push    eax
            push    edx
            call    OnCAnimBlendAssocGroupCopyAnimation
            add     esp, 8
            mov     [esp+12], eax // replace animationID
            
            // call our handler function
            push    eax 
            push    edx
            mov     ecx, [esp+12] // animationClump
            push    ecx
            call    m_pAddAnimationHandler
            add     esp, 0Ch
            pushad
            jmp     NORMAL_FLOW_AddAnimation
        }
    }

    _asm
    {
        NORMAL_FLOW_AddAnimation:
        popad
        mov     eax, dword ptr [esp+0Ch] 
        mov     edx, dword ptr ds:[0B4EA34h] 
        push    esi
        push    edi
        push    eax
        mov     eax, dword ptr [esp+14h] // animationGroup
        mov     edi, dword ptr [esp+10h] // animationClump
        jmp     RETURN_CAnimManager_AddAnimation
    } 
}

void _declspec(naked) HOOK_CAnimManager_AddAnimationAndSync ()
{
    _asm
    {
        pushad
    }

    if ( m_pAddAnimationAndSyncHandler )
    {
         _asm
        {    
            popad
            mov     ecx, [esp+4]  // animationClump
            mov     ebx, [esp+8]  // pAnimAssociationToSyncWith
            mov     edx, [esp+12] // animationGroup
            mov     eax, [esp+16] // animationID
            push    eax
            push    edx
            call    OnCAnimBlendAssocGroupCopyAnimation
            add     esp, 8
            mov     [esp+16], eax // replace animationID
            
            // call our handler function
            push    eax
            push    edx
            push    ebx
            mov     ecx, [esp+16] // animationClump
            push    ecx
            call    m_pAddAnimationAndSyncHandler
            add     esp, 10h
            pushad
            jmp     NORMAL_FLOW_AddAnimationAndSync
        }
    }

    _asm
    {
        NORMAL_FLOW_AddAnimationAndSync:
        popad
        mov     eax, dword ptr [esp+10h]
        mov     edx, dword ptr ds:[0B4EA34h] 
        push    esi
        push    edi
        push    eax
        mov     eax, dword ptr [esp+18h] // animationGroup
        mov     edi, dword ptr [esp+10h] // animationClump
        jmp     RETURN_CAnimManager_AddAnimationAndSync
    }
}

void _declspec(naked) HOOK_CAnimManager_BlendAnimation_Hierarchy () 
{
    _asm
    {
        pushad
    }

    if ( m_pBlendAnimationHierarchyHandler  )
    {
        _asm
        {        
            popad
            //mov     edx, [esp+4]  // animationClump
            mov     eax, [esp+8]  // pAnimHierarchy
            mov     ecx, [esp+12] // flags
            mov     edx, [esp+16] // animationBlendDelta

            // call our handler function
            push    edx
            mov     edx, [esp+8] // animationClump
            push    ecx
            push    eax
            push    edx
            call    m_pBlendAnimationHierarchyHandler
            add     esp, 10h
            mov    [esp+8], eax // replace pAnimHierarchy
            pushad
            jmp NORMAL_FLOW_BlendAnimation_Hierarchy
        }
    }

    _asm
    {
        NORMAL_FLOW_BlendAnimation_Hierarchy:
        popad
        push   0FFFFFFFFh
        push   04D4410h
        jmp    RETURN_CAnimManager_BlendAnimation_Hierarchy
    }

}
