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

#define FUNC_CVisiblityPlugins_SetClumpAlpha    0x732B00
#define FUNC_CVisibilityPlugins_GetAtomicId     0x732370
#define FUNC_CVisibilityPlugins_SetAtomicId     0x732230

class CVisibilityPluginsSA : public CVisibilityPlugins
{
public:
    void SetClumpAlpha(RpClump* pClump, int iAlpha);
    int  GetAtomicId(RwObject* pAtomic);
    void SetAtomicId(RpAtomic* atomic, int id) override;

    bool InsertEntityIntoEntityList(void* entity, float distance, void* callback);
};
