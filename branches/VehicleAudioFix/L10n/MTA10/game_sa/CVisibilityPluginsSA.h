/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVisibilityPluginsSA.h
*  PURPOSE:     Header file for RenderWare entity visibility plugin class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVisibilityPluginsSA_H
#define __CVisibilityPluginsSA_H

#define FUNC_CVisiblityPlugins_SetClumpAlpha    0x732B00
#define FUNC_CVisibilityPlugins_GetAtomicId     0x732370

#include <game/CVisibilityPlugins.h>
typedef unsigned long DWORD;

class CVisibilityPluginsSA : public CVisibilityPlugins
{
public:
    void                SetClumpAlpha       ( RpClump * pClump, int iAlpha );
    int                 GetAtomicId         ( RwObject * pAtomic );
};

#endif