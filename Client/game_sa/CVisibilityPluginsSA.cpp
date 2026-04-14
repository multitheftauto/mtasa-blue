/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVisibilityPluginsSA.cpp
 *  PURPOSE:     RenderWare entity visibility plugin
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVisibilityPluginsSA.h"

#define FUNC_CVisibilityPlugins_InsertEntityIntoEntityList 0x733DD0

// m_alphaEntitiesList
static RenderingListState alphaEntitiesList{(CLinkListSA<AlphaObjectInfoSA>*)0xC88120, (*(std::size_t*)0x733B05) / sizeof(CLinkSA<AlphaObjectInfoSA>), false};

// m_alphaUnderwaterEntityList
static RenderingListState underwaterEntitiesList{(CLinkListSA<AlphaObjectInfoSA>*)0xC88178, (*(std::size_t*)0x733BD5) / sizeof(CLinkSA<AlphaObjectInfoSA>),
                                                 false};

void CVisibilityPluginsSA::SetClumpAlpha(RpClump* pClump, int iAlpha)
{
    DWORD dwFunc = FUNC_CVisiblityPlugins_SetClumpAlpha;
    // clang-format off
    __asm
    {
        push    iAlpha
        push    pClump
        call    dwFunc
        add     esp, 0x8
    }
    // clang-format on
}

// Some AtomicId bits are:
//      0x0001 - Has two versions, and this is the undamaged one
//      0x0002 - Has two versions, and this is the damaged one
//
// Some AtomicId bits might be:
//      0x0004 - Left door?
//      0x0008 - Right door? (or mirrored?)
//      0x0010 - Front part?
//      0x0020 - Rear part?
//      0x0040 - Has alpha?
//      0x0080 - Some door flag?
//      0x0100 - Front door flag?
//      0x0200 - Rear door flag?
//      0x8000 - Some door flag?
int CVisibilityPluginsSA::GetAtomicId(RwObject* pAtomic)
{
    DWORD dwFunc = FUNC_CVisibilityPlugins_GetAtomicId;
    int   iResult = 0;
    // clang-format off
    __asm
    {
        push    pAtomic
        call    dwFunc
        add     esp, 0x4
        mov     iResult, eax
    }
    // clang-format on
    return iResult;
}

bool CVisibilityPluginsSA::IsAtomicVisible(RpAtomic* atomic) const
{
    if (!atomic)
        return false;

    return ((bool(__cdecl*)(RpAtomic*))0x732990)(atomic);
}

bool CVisibilityPluginsSA::InsertEntityIntoEntityList(void* entity, float distance, void* callback)
{
    return ((bool(_cdecl*)(void*, float, void*))FUNC_CVisibilityPlugins_InsertEntityIntoEntityList)(entity, distance, callback);
}

void CVisibilityPluginsSA::SetRenderingListSize(RenderingEntityListType listType, std::size_t elementsCount)
{
    RenderingListState& state = listType == RenderingEntityListType::ENTITY_LIST_TYPE_ALPHA ? alphaEntitiesList : underwaterEntitiesList;
    if (state.size == elementsCount)
        return;

    state.size = elementsCount;
    state.pendingReinit = true;
}

void CVisibilityPluginsSA::CheckRenderingList(RenderingListState& state)
{
    if (!state.pendingReinit)
        return;

    ReInitRenderingList(state.list, state.size);
    state.pendingReinit = false;
}

void CVisibilityPluginsSA::ReInitRenderingList(CLinkListSA<AlphaObjectInfoSA>* list, std::size_t count)
{
    if (!list || count == 0)
        return;

    list->Shutdown();
    list->Init(count);
}

void* __fastcall CVisibilityPluginsSA::InsertAlphaEntityIntoSortedList(CLinkListSA<AlphaObjectInfoSA>* entitiesList, void*, AlphaObjectInfoSA* info)
{
    CheckRenderingList(alphaEntitiesList);
    return entitiesList->InsertSorted(*info);
}

void* __fastcall CVisibilityPluginsSA::InsertUnderwaterEntityIntoSortedList(CLinkListSA<AlphaObjectInfoSA>* entitiesList, void*, AlphaObjectInfoSA* info)
{
    CheckRenderingList(underwaterEntitiesList);
    return entitiesList->InsertSorted(*info);
}

void CVisibilityPluginsSA::ResetRenderingEntityLists()
{
    alphaEntitiesList.size = DEFAULT_MAX_ALPHA_ENTITIES;
    underwaterEntitiesList.size = DEFAULT_MAX_UNDERWATER_ENTITIES;

    ReInitRenderingList(alphaEntitiesList.list, alphaEntitiesList.size);
    ReInitRenderingList(underwaterEntitiesList.list, underwaterEntitiesList.size);
}

void CVisibilityPluginsSA::StaticSetHooks()
{
    HookInstallCall(0x7345F2, (DWORD)InsertAlphaEntityIntoSortedList);  // CVisibilityPlugins::InsertEntityIntoSortedList
    HookInstallCall(0x733DF5, (DWORD)InsertAlphaEntityIntoSortedList);  // CVisibilityPlugins::InsertObjectIntoSortedList

    HookInstallCall(0x7345D9, (DWORD)InsertUnderwaterEntityIntoSortedList);  // CVisibilityPlugins::InsertEntityIntoSortedList
    HookInstallCall(0x733DB5, (DWORD)InsertUnderwaterEntityIntoSortedList);  // CVisibilityPlugins::InsertEntityIntoUnderwaterList
}
