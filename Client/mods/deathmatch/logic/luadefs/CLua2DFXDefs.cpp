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

void CLua2DFXDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Create / destroy functions
        {"addModel2DFX", ArgumentParser<AddModel2DFX>},
        {"removeModel2DFX", ArgumentParser<RemoveModel2DFX>},
        {"resetModel2DFX", ArgumentParser<ResetModel2DFX>},

        // Set functions
        {"setModel2DFXProperties", ArgumentParser<SetModel2DFXProperties>},
        {"set2DFXProperties", ArgumentParser<Set2DFXProperties>},
        {"setModel2DFXProperty", ArgumentParser<SetModel2DFXProperty>},
        {"set2DFXProperty", ArgumentParser<Set2DFXProperty>},
        {"setModel2DFXPosition", ArgumentParser<SetModel2DFXPosition>},
        {"set2DFXPosition", ArgumentParser<Set2DFXPosition>},

        // Get functions
        {"getModel2DFXProperties", ArgumentParser<GetModel2DFXProperties>},
        {"get2DFXProperties", ArgumentParser<Get2DFXProperties>},
        {"getModel2DFXProperty", ArgumentParser<GetModel2DFXProperty>},
        {"get2DFXProperty", ArgumentParser<Get2DFXProperty>},
        {"getModel2DFXPosition", ArgumentParser<GetModel2DFXPosition>},
        {"get2DFXPosition", ArgumentParser<Get2DFXPosition>},
        {"getModel2DFXCount", ArgumentParser<GetModel2DFXCount>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLua2DFXDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "add", "addModel2DFX");

    lua_classfunction(luaVM, "setProperties", "set2DFXProperties");
    lua_classfunction(luaVM, "setProperty", "set2DFXProperty");
    lua_classfunction(luaVM, "setPosition", "set2DFXPosition");

    lua_classfunction(luaVM, "getProperties", "get2DFXProperties");
    lua_classfunction(luaVM, "getProperty", "get2DFXProperty");
    lua_classfunction(luaVM, "getPosition", "get2DFXPosition");

    lua_classvariable(luaVM, "properties", "get2DFXProperties", "set2DFXProperties");
    lua_classvariable(luaVM, "position", "get2DFXPosition", "set2DFXPosition");

    lua_registerclass(luaVM, "2DFX", "Element");
}

std::variant<bool, CClient2DFX*> CLua2DFXDefs::AddModel2DFX(lua_State* luaVM, std::uint32_t modelID, CVector position, e2dEffectType effectType, effectDataMap effectData)
{
    // Only these effects make sense in MTA
    if (effectType != e2dEffectType::LIGHT && effectType != e2dEffectType::PARTICLE && effectType != e2dEffectType::ROADSIGN &&
        effectType != e2dEffectType::ESCALATOR && effectType != e2dEffectType::SUN_GLARE)
        return false;

    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    const char* error = CClient2DFXManager::IsValidEffectData(effectType, effectData);
    if (error)
        throw LuaFunctionError(error);

    CClient2DFX* effect = m_p2DFXManager->Add2DFX(modelID, position, effectType, effectData);
    if (!effect)
        return false;

    CResource* resource = &lua_getownerresource(luaVM);
    if (resource)
        effect->SetParent(resource->GetResource2DFXRoot());

    return effect;
}

bool CLua2DFXDefs::RemoveModel2DFX(std::uint32_t modelID, std::optional<std::uint32_t> index, std::optional<bool> includeDefault)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(modelID);
    if (!modelInfo)
        return false;

    if (index.has_value())
    {
        auto count = GetModel2DFXCount(modelID);
        if (std::holds_alternative<std::uint32_t>(count) && index >= std::get<std::uint32_t>(count))
            throw std::invalid_argument("Invalid effect index");
    }

    // If this is custom effect destroy it (CClient2DFX)
    if (m_p2DFXManager->TryDestroyCustom2DFXEffect(modelID, index.value_or(-1)))
        return true;

    return index.has_value() ? modelInfo->Remove2DFXEffectAtIndex(index.value(), includeDefault.value_or(false)) : modelInfo->RemoveAll2DFXEffects(includeDefault.value_or(false));
}

bool CLua2DFXDefs::ResetModel2DFX(std::uint32_t modelID, std::optional<bool> removeCustomEffects)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(modelID));
    if (!modelInfo)
        return false;

    if (removeCustomEffects.has_value() && removeCustomEffects.value())
        m_p2DFXManager->TryDestroyCustom2DFXEffect(modelID, -1);

    return modelInfo->Reset2DFXEffects();
}

bool CLua2DFXDefs::SetModel2DFXProperties(std::uint32_t modelID, std::uint32_t index, effectDataMap effectData)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    auto count = GetModel2DFXCount(modelID);
    if (std::holds_alternative<std::uint32_t>(count) && index >= std::get<std::uint32_t>(count))
        throw std::invalid_argument("Invalid effect index");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(modelID));
    if (!modelInfo)
        return false;

    auto* effect = modelInfo->Get2DFXFromIndex(index);
    if (!effect)
        return false;

    const char* error = CClient2DFXManager::IsValidEffectData(reinterpret_cast<C2DEffect*>(effect)->GetEffectType(), effectData);
    if (error)
        throw LuaFunctionError(error);

    modelInfo->StoreDefault2DFXEffect(effect);

    if (!m_p2DFXManager->Set2DFXProperties(reinterpret_cast<C2DEffect*>(effect), effectData))
        return false;

    modelInfo->Update2DFXEffect(effect);
    return true;
}

bool CLua2DFXDefs::Set2DFXProperties(CClient2DFX* effect, effectDataMap effectData)
{
    CModelInfo* modelInfo = g_pGame->GetModelInfo(effect->GetModelID());
    if (!modelInfo)
        return false;

    const char* error = CClient2DFXManager::IsValidEffectData(effect->Get2DFXType(), effectData);
    if (error)
        throw LuaFunctionError(error);

    if (!m_p2DFXManager->Set2DFXProperties(effect->Get2DFX(), effectData))
        return false;

    modelInfo->Update2DFXEffect(effect->Get2DFX());
    return true;
}

bool CLua2DFXDefs::SetModel2DFXProperty(std::uint32_t modelID, std::uint32_t index, e2dEffectProperty property, std::variant<bool, float, std::string> propertyValue)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    auto count = GetModel2DFXCount(modelID);
    if (std::holds_alternative<std::uint32_t>(count) && index >= std::get<std::uint32_t>(count))
        throw std::invalid_argument("Invalid effect index");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(modelID));
    if (!modelInfo)
        return false;

    auto* effect = modelInfo->Get2DFXFromIndex(index);
    if (!effect)
        return false;

    modelInfo->StoreDefault2DFXEffect(effect);

    if (!m_p2DFXManager->Set2DFXProperty(reinterpret_cast<C2DEffect*>(effect), property, propertyValue))
        return false;

    modelInfo->Update2DFXEffect(effect);
    return true;
}

bool CLua2DFXDefs::Set2DFXProperty(CClient2DFX* effect, e2dEffectProperty property, std::variant<bool, float, std::string> propertyValue)
{
    CModelInfo* modelInfo = g_pGame->GetModelInfo(effect->GetModelID());
    if (!modelInfo)
        return false;

    if (!m_p2DFXManager->Set2DFXProperty(effect->Get2DFX(), property, propertyValue))
        return false;

    modelInfo->Update2DFXEffect(effect->Get2DFX());
    return true;
}

bool CLua2DFXDefs::SetModel2DFXPosition(std::uint32_t modelID, std::uint32_t index, CVector position)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    auto count = GetModel2DFXCount(modelID);
    if (std::holds_alternative<std::uint32_t>(count) && index >= std::get<std::uint32_t>(count))
        throw std::invalid_argument("Invalid effect index");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(modelID));
    if (!modelInfo)
        return false;

    auto* effect = modelInfo->Get2DFXFromIndex(index);
    if (!effect)
        return false;

    modelInfo->StoreDefault2DFXEffect(effect);
    m_p2DFXManager->Set2DFXPosition(reinterpret_cast<C2DEffect*>(effect), position);

    modelInfo->Update2DFXEffect(effect);
    return true;
}

bool CLua2DFXDefs::Set2DFXPosition(CClient2DFX* effect, CVector position)
{
    CModelInfo* modelInfo = g_pGame->GetModelInfo(effect->GetModelID());
    if (!modelInfo)
        return false;

    m_p2DFXManager->Set2DFXPosition(effect->Get2DFX(), position);
    modelInfo->Update2DFXEffect(effect->Get2DFX());
    return true;
}

std::variant<bool, CLuaMultiReturn<float,float,float>> CLua2DFXDefs::GetModel2DFXPosition(std::uint32_t modelID, std::uint32_t index)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    auto count = GetModel2DFXCount(modelID);
    if (std::holds_alternative<std::uint32_t>(count) && index >= std::get<std::uint32_t>(count))
        throw std::invalid_argument("Invalid effect index");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(modelID));
    if (!modelInfo)
        return false;

    auto* effect = modelInfo->Get2DFXFromIndex(index);
    if (!effect)
        return false;

    CVector* position = m_p2DFXManager->Get2DFXPosition(reinterpret_cast<C2DEffect*>(effect));
    return std::make_tuple(position->fX, position->fY, position->fZ);
}

std::variant<bool, CLuaMultiReturn<float, float, float>> CLua2DFXDefs::Get2DFXPosition(CClient2DFX* effect)
{
    CVector* position = m_p2DFXManager->Get2DFXPosition(effect->Get2DFX());
    if (!position)
        return false;
    
    return std::make_tuple(position->fX, position->fY, position->fZ);
}

std::variant<bool, effectDataMap> CLua2DFXDefs::GetModel2DFXProperties(std::uint32_t modelID, std::uint32_t index)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    auto count = GetModel2DFXCount(modelID);
    if (std::holds_alternative<std::uint32_t>(count) && index >= std::get<std::uint32_t>(count))
        throw std::invalid_argument("Invalid effect index");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(modelID));
    if (!modelInfo)
        return false;

    auto* effect = modelInfo->Get2DFXFromIndex(index);
    if (!effect)
        return false;

    return m_p2DFXManager->Get2DFXProperties(reinterpret_cast<C2DEffect*>(effect));
}

std::variant<bool, effectDataMap> CLua2DFXDefs::Get2DFXProperties(CClient2DFX* effect)
{
    return m_p2DFXManager->Get2DFXProperties(effect->Get2DFX());
}

std::variant<float, bool, std::string> CLua2DFXDefs::GetModel2DFXProperty(std::uint32_t modelID, std::uint32_t index, e2dEffectProperty property)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    auto count = GetModel2DFXCount(modelID);
    if (std::holds_alternative<std::uint32_t>(count) && index >= std::get<std::uint32_t>(count))
        throw std::invalid_argument("Invalid effect index");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(modelID));
    if (!modelInfo)
        return false;

    auto* effect = modelInfo->Get2DFXFromIndex(index);
    if (!effect)
        return false;

    return m_p2DFXManager->Get2DFXProperty(reinterpret_cast<C2DEffect*>(effect), property);
}

std::variant<float, bool, std::string> CLua2DFXDefs::Get2DFXProperty(CClient2DFX* effect, e2dEffectProperty property)
{
    return m_p2DFXManager->Get2DFXProperty(effect->Get2DFX(), property);
}

std::variant<bool, std::uint32_t> CLua2DFXDefs::GetModel2DFXCount(std::uint32_t modelID)
{
    if (!CClient2DFXManager::IsValidModel(modelID))
        throw std::invalid_argument("Invalid model ID");

    CModelInfo* modelInfo = g_pGame->GetModelInfo(modelID);
    if (!modelInfo)
        return false;

    return modelInfo->Get2DFXCount();
}
