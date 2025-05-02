/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLua2DFXDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLua2DFXDefs.h"
#include <lua/CLuaFunctionParser.h>
#include <CClient2DFXManager.h>
#include <game/C2DEffects.h>

void CLua2DFXDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Create/destroy functions
        {"addModel2DFX", ArgumentParser<AddModel2DFX>},
        {"removeModel2DFX", ArgumentParser<RemoveModel2DFX>},
        {"restoreModel2DFX", ArgumentParser<RestoreModel2DFX>},
        {"resetModel2DFXEffects", ArgumentParser<ResetModel2DFXEffects>},

        // Set functions
        {"setModel2DFXPosition", ArgumentParser<SetModel2DFXPosition>},
        {"setModel2DFXProperty", ArgumentParser<SetModel2DFXProperty>},
        {"resetModel2DFXProperty", ArgumentParser<ResetModel2DFXProperty>},
        {"resetModel2DFXPosition", ArgumentParser<ResetModel2DFXPosition>},

        // Get functions
        {"getModel2DFXPosition", ArgumentParser<GetModel2DFXPosition>},
        {"getModel2DFXProperty", ArgumentParser<GetModel2DFXProperty>},
        {"getModel2DFXEffects", ArgumentParser<GetModel2DFXEffects>},
        {"getModel2DFXCount", ArgumentParser<GetModel2DFXCount>},
        {"getModel2DFXType", ArgumentParser<GetModel2DFXType>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

bool CLua2DFXDefs::AddModel2DFX(std::uint32_t model, CVector position, e2dEffectType effectType, effectPropertiesMap effectData)
{
    if (effectType == e2dEffectType::NONE || effectType == e2dEffectType::UNKNOWN)
        return false;

    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    const char* error = CClient2DFXManager::IsValidEffectData(effectType, effectData);
    if (error)
        throw LuaFunctionError(error);

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return false;

    bool effect = modelInfo->Add2DFXEffect(effectType, position, CClient2DFXManager::InitProperties(effectType, effectData));
    if (effect && effectType == e2dEffectType::PARTICLE)
    {
        if (CClientObjectManager::IsValidModel(model))
            m_pManager->GetObjectManager()->RestreamObjects(model);
        else if (CClientVehicleManager::IsValidModel(model))
            m_pManager->GetVehicleManager()->RestreamVehicles(model);
        else if (CClientPlayerManager::IsValidModel(model))
            m_pManager->GetPedManager()->RestreamPeds(model);
    }

    return effect;
}

bool CLua2DFXDefs::RemoveModel2DFX(std::uint32_t model, std::uint32_t index)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return false;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    return modelInfo->Remove2DFXEffect(index);
}

void CLua2DFXDefs::RestoreModel2DFX(std::uint32_t model, std::uint32_t index)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    modelInfo->Restore2DFXEffect(index);
}

void CLua2DFXDefs::ResetModel2DFXEffects(std::uint32_t model)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return;

    // Restore original properties
    for (std::size_t i = 0; i < modelInfo->Get2DFXEffectsCount(); i++)
        modelInfo->Reset2DFXEffectProperties(i);

    // Destroy custom effects & restore "removed" original effects
    modelInfo->Reset2DFXEffects();
}

void CLua2DFXDefs::SetModel2DFXPosition(std::uint32_t model, std::uint32_t index, CVector position)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    // Save original position
    modelInfo->Store2DFXDefaultProperties(index);
    g_pGame->Get2DFXEffects()->Set2DFXEffectPosition(model, index, position);
    modelInfo->Store2DFXProperties(index, e2dEffectProperty::POSITION);
}

bool CLua2DFXDefs::SetModel2DFXProperty(std::uint32_t model, std::uint32_t index, e2dEffectProperty property,
                                        std::variant<bool, float, std::string, std::vector<float>, effectFlagsTable> propertyValue)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return false;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    const e2dEffectType& type = g_pGame->Get2DFXEffects()->Get2DFXEffectType(model, index);

    if (!CClient2DFXManager::IsValidPropertyValue(type, property, propertyValue))
        throw std::invalid_argument("Invalid property value");

    // Save original properties
    modelInfo->Store2DFXDefaultProperties(index);

    // We could use std::visit here, but CGameSA and CMultiplayerSA are in C++14, so...
    if (property == e2dEffectProperty::FLASH_TYPE)
    {
        e2dCoronaFlashType val;
        StringToEnum(std::get<std::string>(propertyValue), val);
        g_pGame->Get2DFXEffects()->Set2DFXProperty(model, index, property, val);
    }
    else if (property == e2dEffectProperty::FLAGS && std::holds_alternative<effectFlagsTable>(propertyValue))
    {
        std::uint16_t flags = 0;
        if (!CClient2DFXManager::InitFlagsFromTable(type, std::get<effectFlagsTable>(propertyValue), flags))
            return false;

        g_pGame->Get2DFXEffects()->Set2DFXProperty(model, index, property, static_cast<float>(flags));
    }
    else if (std::holds_alternative<bool>(propertyValue))
        g_pGame->Get2DFXEffects()->Set2DFXProperty(model, index, property, std::get<bool>(propertyValue));
    else if (std::holds_alternative<float>(propertyValue))
        g_pGame->Get2DFXEffects()->Set2DFXProperty(model, index, property, std::get<float>(propertyValue));
    else if (std::holds_alternative<std::string>(propertyValue))
        g_pGame->Get2DFXEffects()->Set2DFXProperty(model, index, property, std::get<std::string>(propertyValue));
    else if (std::holds_alternative<std::vector<float>>(propertyValue))
        g_pGame->Get2DFXEffects()->Set2DFXProperty(model, index, property, std::get<std::vector<float>>(propertyValue));

    modelInfo->Store2DFXProperties(index, property);

    if (type != e2dEffectType::LIGHT)
        g_pGame->Get2DFXEffects()->ReInitEffect(model, index);

    return true;
}

void CLua2DFXDefs::ResetModel2DFXProperty(std::uint32_t model, std::uint32_t index, e2dEffectProperty property)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    if (!CClient2DFXManager::IsValidProperty(g_pGame->Get2DFXEffects()->Get2DFXEffectType(model, index), property))
        throw std::invalid_argument("Invalid property for this type of effect");

    modelInfo->Reset2DFXProperty(index, property);
}

void CLua2DFXDefs::ResetModel2DFXPosition(std::uint32_t model, std::uint32_t index)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    modelInfo->Reset2DFXProperty(index, e2dEffectProperty::POSITION);
}

std::variant<bool, CLuaMultiReturn<float, float, float>> CLua2DFXDefs::GetModel2DFXPosition(std::uint32_t model, std::uint32_t index)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return false;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    CVector& pos = g_pGame->Get2DFXEffects()->Get2DFXEffectPosition(model, index);
    return CLuaMultiReturn<float, float, float>{pos.fX, pos.fY, pos.fZ};
}

std::variant<bool, float, std::string, CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>, CLuaMultiReturn<float, float, float>,
             effectFlagsTable>
CLua2DFXDefs::GetModel2DFXProperty(std::uint32_t model, std::uint32_t index, e2dEffectProperty property, std::optional<bool> getFlagsAsTable)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return false;

    if (index > GetModel2DFXCount(model, true) - 1)
        throw std::invalid_argument("Index out of bounds");

    if (!CClient2DFXManager::IsValidProperty(g_pGame->Get2DFXEffects()->Get2DFXEffectType(model, index), property))
        throw std::invalid_argument("Invalid property for this type of effect");

    auto* effects = g_pGame->Get2DFXEffects();

    std::variant<bool, float, std::string, CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>, CLuaMultiReturn<float, float, float>,
                 effectFlagsTable>
        valueToReturn;
    if (effects->IsPropertyValueNumber(property))
    {
        float val{};
        effects->Get2DFXProperty(model, index, property, val);

        valueToReturn = val;

        if (property == e2dEffectProperty::FLAGS && getFlagsAsTable.value_or(false))
            valueToReturn = CClient2DFXManager::InitFlagsTable(effects->Get2DFXEffectType(model, index), static_cast<std::uint16_t>(val));
    }
    else if (effects->IsPropertyValueString(property))
    {
        std::string val{};
        effects->Get2DFXProperty(model, index, property, val);

        valueToReturn = std::move(val);
    }
    else if (effects->IsPropertyValueTable(property))
    {
        std::vector<float> val;
        val.reserve(property == e2dEffectProperty::SIZE ? 2 : 3);
        effects->Get2DFXProperty(model, index, property, val);

        valueToReturn = CLuaMultiReturn<float, float, float>{val[0], val[1], val.size() > 2 ? val[2] : 0};
    }
    else
    {
        switch (property)
        {
            case e2dEffectProperty::CORONA_REFLECTION:
            {
                bool val = false;
                effects->Get2DFXProperty(model, index, property, val);

                valueToReturn = val;
                break;
            }
            case e2dEffectProperty::FLASH_TYPE:
            {
                e2dCoronaFlashType val = e2dCoronaFlashType::DEFAULT;
                effects->Get2DFXProperty(model, index, property, val);

                std::string str = EnumToString(val);
                valueToReturn = std::move(str);
                break;
            }
            case e2dEffectProperty::COLOR:
            {
                SColor val{};
                effects->Get2DFXProperty(model, index, property, val);

                valueToReturn = CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>{val.R, val.G, val.B, val.A};
                break;
            }
        }
    }

    return valueToReturn;
}

std::variant<bool, effectsMap> CLua2DFXDefs::GetModel2DFXEffects(std::uint32_t model, std::optional<bool> includeCustomEffects)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return false;

    auto        tbl = effectsMap();
    C2DEffects* effects = g_pGame->Get2DFXEffects();

    for (std::size_t i = 0; i < modelInfo->Get2DFXEffectsCount(includeCustomEffects.value_or(true)); i++)
    {
        CVector&             position = effects->Get2DFXEffectPosition(model, i);
        const e2dEffectType& type = effects->Get2DFXEffectType(model, i);

        tbl[i] = {
            {"position", std::vector<float>{position.fX, position.fY, position.fZ}},
            {"type", EnumToString(type)},
            {"properties", CClient2DFXManager::GetProperties(model, i)},
        };
    }

    return tbl;
}

std::uint32_t CLua2DFXDefs::GetModel2DFXCount(std::uint32_t model, std::optional<bool> includeCustomEffects)
{
    if (!CClient2DFXManager::IsValidModel(model))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(model);
    if (!modelInfo)
        return 0;

    return modelInfo->Get2DFXEffectsCount(includeCustomEffects.value_or(true));
}

e2dEffectType CLua2DFXDefs::GetModel2DFXType(std::uint32_t model, std::uint32_t index)
{
    return g_pGame->Get2DFXEffects()->Get2DFXEffectType(model, index);
}
