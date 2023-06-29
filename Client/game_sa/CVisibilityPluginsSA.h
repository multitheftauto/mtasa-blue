/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CVisibilityPluginsSA.h
 *  PURPOSE:     Header file for RenderWare entity visibility plugin class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CVisibilityPlugins.h>

#define FUNC_CVisiblityPlugins_SetClumpAlpha    0x732B00
#define FUNC_CVisibilityPlugins_GetAtomicId     0x732370

class CVisibilityPluginsSA : public CVisibilityPlugins
{
public:
    void SetClumpAlpha(RpClump* pClump, int iAlpha);
    int  GetAtomicId(RwObject* pAtomic);
};
