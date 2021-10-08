/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_ObjectCollision.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

static void ColStoreAddRef(CObjectSAInterface* gameObject)
{
    CModelInfo* modelInfo = pGameInterface->GetModelInfo(gameObject->m_nModelIndex);

    if (modelInfo)
        modelInfo->AddColRef();
}

static void ColStoreRemoveRef(CObjectSAInterface* gameObject)
{
    CModelInfo* modelInfo = pGameInterface->GetModelInfo(gameObject->m_nModelIndex);

    if (modelInfo)
        modelInfo->RemoveColRef();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CObject::Init
//
// Replaces the code block around `CColStore::AddRef` with our `ColStoreAddRef` and also
// sets the necessary object flag for reference counting.
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x59F8BC | 74 1E             | jz      short loc_59F8DC
// >>> 0x59F8BE | 0F B6 47 28       | movzx   eax, byte ptr [edi+28h]
// >>> 0x59F8C2 | 50                | push    eax
// >>> 0x59F8C3 | E8 D8 0E E7 FF    | call    CColStore::AddRef(int)
// >>> 0x59F8C8 | 8B 86 40 01 00 00 | mov     eax, [esi+140h]
// >>> 0x59F8CE | 83 C4 04          | add     esp, 4
// >>> 0x59F8D1 | 0D 00 00 01 00    | or      eax, 10000h
// >>> 0x59F8D6 | 89 86 40 01 00 00 | mov     [esi+140h], eax
//     0x59F8DC | 0F BF 4E 22       | movsx   ecx, word ptr [esi+22h]
#define HOOKPOS_CObject_Init               0x59F8BE
#define HOOKSIZE_CObject_Init              30
static const DWORD CONTINUE_CObject_Init = 0x59F8DC;

static void _declspec(naked) HOOK_CObject_Init()
{
    _asm
    {
        pushad
        push    esi     // CObjectSAInterface*
        call    ColStoreAddRef
        add     esp, 4
        popad

        // this->m_objectFlags |= 0x10000u;
        mov     eax, [esi+140h]
        or      eax, 10000h
        mov     [esi+140h], eax

        jmp     CONTINUE_CObject_Init
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CObject::~CObject
//
// Replaces the call to `CColStore::RemoveRef` with our `ColStoreRemoveRef`
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x59F6EC | 74 1B                | jz      short loc_59F709
// >>> 0x59F6EE | 0F BF 56 22          | movsx   edx, word ptr [esi+22h]
// >>> 0x59F6F2 | 8B 04 95 C8 B0 A9 00 | mov     eax, ds:CModelInfo::ms_modelInfoPtrs
// >>> 0x59F6F9 | 8B 40 14             | mov     eax, [eax+14h]
// >>> 0x59F6FC | 0F B6 40 28          | movzx   eax, byte ptr [eax+28h]
// >>> 0x59F700 | 50                   | push    eax
// >>> 0x59F701 | E8 CA 10 E7 FF       | call    CColStore::RemoveRef(int)
// >>> 0x59F706 | 83 C4 04             | add     esp, 4
//     0x59F709 | 8B 3D 9C 44 B7 00    | mov     edi, ds:CPools::ms_pObjectPool
#define HOOKPOS_CObject_Destructor               0x59F6EE
#define HOOKSIZE_CObject_Destructor              27
static const DWORD CONTINUE_CObject_Destructor = 0x59F709;

static void _declspec(naked) HOOK_CObject_Destructor()
{
    _asm
    {
        pushad
        push    esi     // CObjectSAInterface*
        call    ColStoreRemoveRef
        add     esp, 4
        popad
        jmp     CONTINUE_CObject_Destructor
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_ObjectCollision
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_ObjectCollision()
{
    EZHookInstall(CObject_Init);
    EZHookInstall(CObject_Destructor);
}
