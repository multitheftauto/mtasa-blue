/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"
#include "CLuaBodyClothesDefs.h"

void CLuaBodyClothesDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getBodyPartName", ArgumentParserWarn<false, GetBodyPartName>},
        {"getClothesByTypeIndex", ArgumentParserWarn<false, GetClothesByTypeIndex>},
        {"getTypeIndexFromClothes", ArgumentParserWarn<false, GetTypeIndexFromClothes>},
        {"getClothesTypeName", ArgumentParserWarn<false, GetClothesTypeName>},
        {"addClothingModel", ArgumentParserWarn<false, AddClothingModel>},
        {"removeClothingModel", ArgumentParserWarn<false, RemoveClothingModel>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

std::variant<const char*, bool> CLuaBodyClothesDefs::GetBodyPartName(std::uint8_t bodyPartId) noexcept
{
    if (bodyPartId > BODYPART_HEAD)
        return false;

    return CClientPed::GetBodyPartName(bodyPartId);
}

std::variant<bool, CLuaMultiReturn<std::string, std::string>> CLuaBodyClothesDefs::GetClothesByTypeIndex(std::uint8_t clothesType, std::uint8_t clothesIndex)
{
    const auto clothingGroup = CClientPlayerClothes::GetClothingGroup(clothesType);

    if (static_cast<std::size_t>(clothesIndex) >= clothingGroup.size())
        return false;

    const SPlayerClothing* clothing = clothingGroup[clothesIndex];

    return CLuaMultiReturn<std::string, std::string>{clothing->texture, clothing->model};
}

std::variant<bool, CLuaMultiReturn<std::uint8_t, std::uint8_t>> CLuaBodyClothesDefs::GetTypeIndexFromClothes(std::string                clothesTexture,
                                                                                                             std::optional<std::string> clothesModel)
{
    const bool matchAnyModel = !clothesModel.has_value() || clothesModel->empty();

    for (std::uint8_t clothesType = 0; clothesType < PLAYER_CLOTHING_SLOTS; clothesType++)
    {
        const auto clothingGroup = CClientPlayerClothes::GetClothingGroup(clothesType);

        for (std::size_t clothesIndex = 0; clothesIndex < clothingGroup.size(); clothesIndex++)
        {
            const SPlayerClothing* clothing = clothingGroup[clothesIndex];

            if (clothing->texture != clothesTexture)
                continue;

            if (!matchAnyModel && clothing->model != *clothesModel)
                continue;

            return CLuaMultiReturn<std::uint8_t, std::uint8_t>{clothesType, static_cast<std::uint8_t>(clothesIndex)};
        }
    }

    return false;
}

std::variant<const char*, bool> CLuaBodyClothesDefs::GetClothesTypeName(std::uint8_t clothesType) noexcept
{
    if (const char* clothesTypeName = CClientPlayerClothes::GetClothingName(clothesType))
        return clothesTypeName;

    return false;
}

bool CLuaBodyClothesDefs::AddClothingModel(std::string clothesTexture, std::string clothesModel, std::uint8_t clothesType)
{
    return CClientPlayerClothes::AddClothingModel(clothesTexture.c_str(), clothesModel.c_str(), clothesType);
}

bool CLuaBodyClothesDefs::RemoveClothingModel(std::string clothesTexture, std::string clothesModel, std::uint8_t clothesType)
{
    return CClientPlayerClothes::RemoveClothingModel(clothesTexture.c_str(), clothesModel.c_str(), clothesType);
}
