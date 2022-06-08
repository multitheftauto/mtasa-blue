/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaModelDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "StdInc.h"
#include "luadefs/CLuaDefs.h"

class CLuaModelDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static uint AllocateModel(eModelInfoType eModel, std::optional<uint> uiParentModel, std::optional<uint> uiModelID);
    static bool AllocateModelFromParent(uint32_t uiModelID, uint32_t uiParentModelID);

    static std::vector<uint> GetAllocatedModels(std::optional<eModelInfoType> eType);
};
