/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClient2DFXManager.h
 *  PURPOSE:     Handling 2DFX effects manager
 *
 *****************************************************************************/
#pragma once

class CClient2DFXManager
{
public:
    CClient2DFXManager(class CClientManager* mainManager) : m_mainManager(mainManager) {};

    static bool InitFlagsFromTable(const e2dEffectType& effectType, const effectFlagsTable& tbl, std::uint16_t& flags);
    static effectFlagsTable InitFlagsTable(const e2dEffectType& effectType, std::uint16_t flags);

    static S2DEffectData InitProperties(const e2dEffectType& effectType, const effectPropertiesMap& properties);
    static effectPropertiesMap GetProperties(std::uint32_t model, std::uint32_t index);

    static bool          IsValidModel(std::uint32_t model);
    static const char*   IsValidEffectData(const e2dEffectType& effectType, const effectPropertiesMap& effectData);
    static bool          IsValidPropertyValue(const e2dEffectType& effectType, const e2dEffectProperty& property, std::variant<bool, float, std::string, std::vector<float>, effectFlagsTable> value);
    static bool          IsValidProperty(const e2dEffectType& effectType, const e2dEffectProperty& property);

private:
    class CClientManager* m_mainManager;
};
