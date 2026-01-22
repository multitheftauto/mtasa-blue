/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/multiplayer_sa/CMultiplayerSA_CustomAnimations.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include <../game_sa/CAnimBlendAssocGroupSA.h>
#include <../game_sa/CAnimManagerSA.h>

DWORD FUNC_CAnimBlendAssociation__ReferenceAnimBlock = 0x4CEA50;
DWORD FUNC_UncompressAnimation = 0x4D41C0;
DWORD FUNC_CAnimBlendAssociation__CAnimBlendAssociation_hierarchy = 0x4CEFC0;

DWORD RETURN_CAnimBlendAssociation_SetCurrentTime_NORMALFLOW = 0x4CEA88;
DWORD RETURN_RpAnimBlendClumpUpdateAnimations_NORMALFLOW = 0x4D34F8;
DWORD RETURN_CAnimManager_AddAnimation_NORMAL_FLOW = 0x4D3AAA;
DWORD RETURN_CAnimManager_AddAnimation = 0x4D3ABC;
DWORD RETURN_CAnimManager_AddAnimationAndSync_NORMAL_FLOW = 0x4D3B3A;
DWORD RETURN_CAnimManager_AddAnimationAndSync = 0x4D3B4C;
DWORD RETURN_CAnimManager_BlendAnimation_Hierarchy = 0x4D4577;

auto CAnimBlendAssociation_NewOperator = (hCAnimBlendAssociation_NewOperator)0x82119A;

AddAnimationHandler*            m_pAddAnimationHandler = nullptr;
AddAnimationAndSyncHandler*     m_pAddAnimationAndSyncHandler = nullptr;
AssocGroupCopyAnimationHandler* m_pAssocGroupCopyAnimationHandler = nullptr;
BlendAnimationHierarchyHandler* m_pBlendAnimationHierarchyHandler = nullptr;

static bool bDisableCallsToCAnimBlendNode = true;

eAnimID _cdecl OnCAnimBlendAssocGroupCopyAnimation_FixBadAnim(eAnimGroup* pAnimGroup, eAnimID* pAnimId);

void CMultiplayerSA::SetAddAnimationHandler(AddAnimationHandler* pHandler)
{
    m_pAddAnimationHandler = pHandler;
}

void CMultiplayerSA::SetAddAnimationAndSyncHandler(AddAnimationAndSyncHandler* pHandler)
{
    m_pAddAnimationAndSyncHandler = pHandler;
}

void CMultiplayerSA::SetAssocGroupCopyAnimationHandler(AssocGroupCopyAnimationHandler* pHandler)
{
    m_pAssocGroupCopyAnimationHandler = pHandler;
}

void CMultiplayerSA::SetBlendAnimationHierarchyHandler(BlendAnimationHierarchyHandler* pHandler)
{
    m_pBlendAnimationHierarchyHandler = pHandler;
}

void CMultiplayerSA::DisableCallsToCAnimBlendNode(bool bDisableCalls)
{
    bDisableCallsToCAnimBlendNode = bDisableCalls;
}

CAnimationStyleDescriptorSAInterface* getAnimStyleDescriptorInterface(eAnimGroup animGroup)
{
    auto pAnimAssocDefinitionsArray = (CAnimationStyleDescriptorSAInterface*)0x8AA5A8;
    return &pAnimAssocDefinitionsArray[(int)animGroup];
}

CAnimBlendAssocGroupSAInterface* getAnimAssocGroupInterface(eAnimGroup animGroup)
{
    auto pAnimGroupArray = reinterpret_cast<CAnimBlendAssocGroupSAInterface*>(*(DWORD*)0xb4ea34);
    return &pAnimGroupArray[(int)animGroup];
}

void _declspec(naked) HOOK_CAnimBlendAssociation_SetCurrentTime()
{
    _asm
    {
        pushad
    }

    if (bDisableCallsToCAnimBlendNode)
    {
        _asm
        {
            popad
            retn 4
        }
    }

    _asm
    {
        popad
        mov     eax, [esp+4]
        fld     [esp+4]
        jmp     RETURN_CAnimBlendAssociation_SetCurrentTime_NORMALFLOW
    }
}

void _declspec(naked) HOOK_RpAnimBlendClumpUpdateAnimations()
{
    _asm
    {
        pushad
    }

    if (bDisableCallsToCAnimBlendNode)
    {
        _asm
        {
            popad
            retn
        }
    }

    _asm
    {
        popad
        sub     esp, 3Ch
        mov     eax, ds:[0B5F878h]
        jmp     RETURN_RpAnimBlendClumpUpdateAnimations_NORMALFLOW
    }
}

CAnimBlendAssociationSAInterface* __cdecl CAnimBlendAssocGroup_CopyAnimation(RpClump* pClump, eAnimGroup u32AnimGroupID, eAnimID animID)
{
    auto pAnimAssociationInterface =
        reinterpret_cast<CAnimBlendAssociationSAInterface*>(CAnimBlendAssociation_NewOperator(sizeof(CAnimBlendAssociationSAInterface)));

    if (pAnimAssociationInterface)
    {
        auto pAnimAssocGroupInterface = getAnimAssocGroupInterface(u32AnimGroupID);
        if (!pAnimAssocGroupInterface->pAnimBlock || pAnimAssocGroupInterface->groupID == -1)
        {
            auto pAnimStyleDescriptorInterface = getAnimStyleDescriptorInterface(u32AnimGroupID);
            auto pAnimBlock = pGameInterface->GetAnimManager()->GetAnimationBlock(pAnimStyleDescriptorInterface->blockName);
            pAnimBlock->Request(BLOCKING, true);
        }
        m_pAssocGroupCopyAnimationHandler(pAnimAssociationInterface, pClump, pAnimAssocGroupInterface, animID);
    }
    return pAnimAssociationInterface;
}

void _declspec(naked) HOOK_CAnimManager_AddAnimation()
{
    _asm
    {
        lea     edx, [esp + 8]  // animationGroupID address
        lea     eax, [esp + 12] // animationID address
        push    eax
        push    edx
        call    OnCAnimBlendAssocGroupCopyAnimation_FixBadAnim
        add     esp, 8
        pushad
    }

    if (m_pAddAnimationHandler)
    {
        _asm
        {
            popad
            mov     ecx, [esp + 4]  // animationClump
            mov     edx, [esp + 8]  // animationGroupID
            mov     eax, [esp + 12] // animationID
            push    eax
            push    edx
            push    ecx
            call    CAnimBlendAssocGroup_CopyAnimation
            add     esp, 12
            push    esi
            push    edi
            jmp     RETURN_CAnimManager_AddAnimation

        }
    }

    _asm
    {
        popad
        mov     eax, dword ptr[esp + 0Ch]
        mov     edx, dword ptr ds : [0B4EA34h]
        jmp     RETURN_CAnimManager_AddAnimation_NORMAL_FLOW
    }
}

void _declspec(naked) HOOK_CAnimManager_AddAnimationAndSync()
{
     _asm
     {
         lea     edx, [esp + 12] // animationGroup address
         lea     eax, [esp + 16] // animationID address
         push    eax
         push    edx
         call    OnCAnimBlendAssocGroupCopyAnimation_FixBadAnim
         add     esp, 8
         pushad
     }

    if (m_pAddAnimationAndSyncHandler)
    {
         _asm
         {
             popad
             mov     ecx, [esp + 4]  // animationClump
             mov     edx, [esp + 12] // animationGroupID
             mov     eax, [esp + 16] // animationID
             push    eax
             push    edx
             push    ecx
             call    CAnimBlendAssocGroup_CopyAnimation
             add     esp, 12
             push    esi
             push    edi
             jmp     RETURN_CAnimManager_AddAnimationAndSync
         }
    }

     _asm
     {

         popad
             mov     eax, dword ptr[esp + 10h]
             mov     edx, dword ptr ds : [0B4EA34h]
             jmp     RETURN_CAnimManager_AddAnimationAndSync_NORMAL_FLOW
     }
}

void _declspec(naked) HOOK_CAnimManager_BlendAnimation_Hierarchy()
{
    _asm
    {
        pushad
    }

    if (m_pBlendAnimationHierarchyHandler)
    {
        _asm
        {
            popad
            push    eax // pAnimAssociation
            push    ecx // pAnimHierarchy

            push    ebp
            mov     ebp, esp
            sub     esp, 8

            // call our handler function
            mov     edx, [ebp+28h+4+12]
            lea     ecx, [ebp+28h+4+20]
            push    edx  // pClump
            push    ecx  // pFlags
            lea     edx, [ebp-4]
            push    edx  //  CAnimBlendHierarchySAInterface ** pOutAnimHierarchy
            mov     edx, [esp+28]
            push    edx  // pAnimAssociation
            call    m_pBlendAnimationHierarchyHandler //CAnimManager_BlendAnimation_Hierarchy
            add     esp, 10h

            mov     ecx, [ebp-4] // pCustomAnimHierarchy

            add     esp, 8 // remove space for local var
            mov     esp, ebp
            pop     ebp

             // Check if it's a custom animation or not
            cmp     al, 00
            je      NOT_CUSTOM_ANIMATION_CAnimManager_BlendAnimation_Hierarchy

            // Replace animation hierarchy with our custom one
            mov     [esp], ecx
            mov     [esp+28h+8+8], ecx

            NOT_CUSTOM_ANIMATION_CAnimManager_BlendAnimation_Hierarchy:
            pop ecx
            pop eax
            pushad
            jmp NORMAL_FLOW_BlendAnimation_Hierarchy
        }
    }

    _asm
    {
        NORMAL_FLOW_BlendAnimation_Hierarchy:
        popad
        mov     edx, [esp+28h+4]
        push    ecx      // pAnimHierarchy
        push    edx      // pClump
        mov     ecx, eax // pAnimAssociation
        call    FUNC_CAnimBlendAssociation__CAnimBlendAssociation_hierarchy
        mov     esi, eax
        mov     ax, word ptr [esp+28h+0Ch] // flags
        mov     ecx, esi
        mov     [esp+28h-4], 0FFFFFFFFh // var_4
        mov     [esp+28h-18h], esi
        mov     [esi+2Eh], ax
        call    FUNC_CAnimBlendAssociation__ReferenceAnimBlock
        mov     ecx, [esp+28h+8] // pAnimHierarchy
        push    ecx
        call    FUNC_UncompressAnimation
        jmp    RETURN_CAnimManager_BlendAnimation_Hierarchy
    }
}
