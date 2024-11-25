/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaModelDefs.h
 *  PURPOSE:     Lua model definitions class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaModelDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    // Util func
    static bool IsValidModel(std::string modelPurpose, std::uint32_t id);

    // LOD funcs
    static std::variant<bool, std::uint32_t> GetLowLODModel(std::uint32_t hLODModel) noexcept;
    static std::variant<bool, std::uint32_t> GetHighLODModel(std::uint32_t lLODModel) noexcept;
    static bool                              SetLowLODModel(std::string modelPurpose, std::uint32_t hLODModel, std::uint32_t lLODModel);
    static bool                              ResetLowLODModel(std::uint32_t hLODModel) noexcept;
    static void                              ResetLowLODModels() noexcept;
};
