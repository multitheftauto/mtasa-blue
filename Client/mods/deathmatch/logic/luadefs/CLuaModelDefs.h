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
    static std::variant<bool, std::uint32_t> GetModelLowLOD(std::uint32_t hLODModel) noexcept;
    static std::variant<bool, std::uint32_t> GetModelHighLOD(std::uint32_t lLODModel) noexcept;
    static bool                              SetModelLOD(std::string modelPurpose, std::uint32_t hLODModel, std::uint32_t lLODModel);
    static bool                              ResetModelLODByHigh(std::uint32_t hLODModel) noexcept;
    static bool                              ResetModelLODByLow(std::uint32_t lLODModel) noexcept;
    static void                              ResetAllModelLOD() noexcept;
};
