/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaBodyClothesDefs.h"
#include "lua/CLuaFunctionParser.h"

void CLuaBodyClothesDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getBodyPartName", ArgumentParserWarn<false, GetBodyPartName>},
        {"getClothesByTypeIndex", ArgumentParserWarn<false, GetClothesByTypeIndex>},
        {"getTypeIndexFromClothes", ArgumentParserWarn<false, GetTypeIndexFromClothes>},
        {"getClothesTypeName", ArgumentParserWarn<false, GetClothesTypeName>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

std::variant<const char*, bool> CLuaBodyClothesDefs::GetBodyPartName(std::uint8_t bodyPartId) noexcept
{
    if (bodyPartId >= 10)
        return false;

    return CPed::GetBodyPartName(bodyPartId);
}

std::variant<bool, CLuaMultiReturn<const char*, const char*>> CLuaBodyClothesDefs::GetClothesByTypeIndex(std::uint8_t clothesType, std::uint8_t clothesIndex)
{
    const SPlayerClothing* clothingGroup = CPlayerClothes::GetClothingGroup(clothesType);

    if (!clothingGroup || clothesIndex >= CPlayerClothes::GetClothingGroupMax(clothesType))
        return false;

    return CLuaMultiReturn<const char*, const char*>{clothingGroup[clothesIndex].szTexture, clothingGroup[clothesIndex].szModel};
}

std::variant<bool, CLuaMultiReturn<std::uint8_t, std::uint8_t>> CLuaBodyClothesDefs::GetTypeIndexFromClothes(std::string                clothesTexture,
                                                                                                             std::optional<std::string> clothesModel)
{
    const bool matchAnyModel = !clothesModel.has_value() || clothesModel->empty();

    for (std::uint8_t clothesType = 0; clothesType < PLAYER_CLOTHING_SLOTS; clothesType++)
    {
        const SPlayerClothing* clothingGroup = CPlayerClothes::GetClothingGroup(clothesType);
        if (!clothingGroup)
            continue;

        for (std::size_t clothesIndex = 0; clothingGroup[clothesIndex].szTexture != nullptr; clothesIndex++)
        {
            if (clothesTexture != clothingGroup[clothesIndex].szTexture)
                continue;

            if (!matchAnyModel && *clothesModel != clothingGroup[clothesIndex].szModel)
                continue;

            return CLuaMultiReturn<std::uint8_t, std::uint8_t>{clothesType, static_cast<std::uint8_t>(clothesIndex)};
        }
    }

    return false;
}

std::variant<const char*, bool> CLuaBodyClothesDefs::GetClothesTypeName(std::uint8_t clothesType) noexcept
{
    if (const char* clothesTypeName = CPlayerClothes::GetClothingName(clothesType))
        return clothesTypeName;

    return false;
}
