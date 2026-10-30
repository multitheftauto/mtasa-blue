/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaBodyClothesDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static std::variant<const char*, bool>                                 GetBodyPartName(std::uint8_t bodyPartId) noexcept;
    static std::variant<bool, CLuaMultiReturn<std::string, std::string>>   GetClothesByTypeIndex(std::uint8_t clothesType, std::uint8_t clothesIndex);
    static std::variant<bool, CLuaMultiReturn<std::uint8_t, std::uint8_t>> GetTypeIndexFromClothes(std::string                clothesTexture,
                                                                                                   std::optional<std::string> clothesModel);
    static std::variant<const char*, bool>                                 GetClothesTypeName(std::uint8_t clothesType) noexcept;
    static bool AddClothingModel(std::string clothesTexture, std::string clothesModel, std::uint8_t clothesType);
    static bool RemoveClothingModel(std::string clothesTexture, std::string clothesModel, std::uint8_t clothesType);
};
