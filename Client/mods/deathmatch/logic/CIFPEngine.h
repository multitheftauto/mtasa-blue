/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CIFPEngine.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CIFPENGINE_H
#define __CIFPENGINE_H
#pragma once

#include "game/CAnimBlendHierarchy.h"
class CAnimBlendHierarchy;

class CIFPEngine
{
public:
    enum eRestoreAnimation
    {
        SINGLE,
        BLOCK,
        ALL
    };

    static std::shared_ptr<CClientIFP> EngineLoadIFP(CResource* pResource, CClientManager* pManager, const SString& strPath, const SString& strBlockName);
    static bool                        EngineReplaceAnimation(CClientEntity* pEntity, const SString& strInternalBlockName, const SString& strInternalAnimName,
                                                              const SString& strCustomBlockName, const SString& strCustomAnimName);
    static bool                        EngineRestoreAnimation(CClientEntity* pEntity, const SString& strInternalBlockName, const SString& strInternalAnimName,
                                                              const eRestoreAnimation& eRestoreType);
};

#endif
