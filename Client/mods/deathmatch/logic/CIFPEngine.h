/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CIFPEngine.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "game/CAnimBlendHierarchy.h"
class CAnimBlendHierarchy;

class CIFPEngine
{
public:
    enum eRestoreAnimation
    {
        SINGLE = 0,
        BLOCK,
        ALL
    };

    static std::shared_ptr<CClientIFP> LoadIFP(CResource* resource, CClientManager* clientManager, const SString& blockName, bool isRawInput, SString input, std::vector<SString>&& uncompressedAnims);
    static bool                        EngineReplaceAnimation(CClientEntity* pEntity, const SString& strInternalBlockName, const SString& strInternalAnimName,
                                                              const SString& strCustomBlockName, const SString& strCustomAnimName);
    static bool                        EngineRestoreAnimation(CClientEntity* pEntity, const SString& strInternalBlockName, const SString& strInternalAnimName,
                                                              const eRestoreAnimation& eRestoreType);
    static bool                        EngineApplyAnimation(CClientPed& Ped, CAnimBlendHierarchySAInterface* pOriginalHierarchyInterface,
                                                            CAnimBlendHierarchySAInterface* pAnimHierarchyInterface);
    static bool                        IsIFPData(const SString& strData);
};
