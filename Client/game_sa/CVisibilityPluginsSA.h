/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVisibilityPluginsSA.h
 *  PURPOSE:     Header file for RenderWare entity visibility plugin class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CVisibilityPlugins.h>
#include "CLinkListSA.h"

#define FUNC_CVisiblityPlugins_SetClumpAlpha 0x732B00
#define FUNC_CVisibilityPlugins_GetAtomicId  0x732370

struct AlphaObjectInfoSA
{
    void* object;
    void* callback;
    float distance;
};

struct RenderingListState
{
    CLinkListSA<AlphaObjectInfoSA>* list;
    std::size_t                     size;
    bool                            pendingReinit;
};

class CVisibilityPluginsSA : public CVisibilityPlugins
{
public:
    void SetClumpAlpha(RpClump* pClump, int iAlpha);
    int  GetAtomicId(RwObject* pAtomic);

    bool IsAtomicVisible(RpAtomic* atomic) const override;

    bool InsertEntityIntoEntityList(void* entity, float distance, void* callback);
    void SetRenderingListSize(RenderingEntityListType listType, std::size_t elementsCount) override;

    static void ResetRenderingEntityLists();
    static void StaticSetHooks();

private:
    static void CheckRenderingList(RenderingListState& state);
    static void ReInitRenderingList(CLinkListSA<AlphaObjectInfoSA>* list, std::size_t count);

    static void* __fastcall InsertAlphaEntityIntoSortedList(CLinkListSA<AlphaObjectInfoSA>* entitiesList, void*, AlphaObjectInfoSA* info);
    static void* __fastcall InsertUnderwaterEntityIntoSortedList(CLinkListSA<AlphaObjectInfoSA>* entitiesList, void*, AlphaObjectInfoSA* info);
};
