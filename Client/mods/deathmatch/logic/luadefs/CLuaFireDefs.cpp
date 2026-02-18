/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaFireDefs.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include "CLuaFireDefs.h"
#include "game/CFireManager.h"

void CLuaFireDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createFire", ArgumentParserWarn<false, CreateFire>},
        {"extinguishFire", ArgumentParserWarn<false, ExtinguishFire>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

CClientFire* CLuaFireDefs::CreateFire(CVector position, float size, std::optional<std::uint32_t> lifetime, std::optional<bool> makeNoise)
{
    CClientFire* fire = new CClientFire(m_pManager->GetFireManager(), position, size, lifetime.value_or(5000), makeNoise.value_or(true));
    return fire;
}

bool CLuaFireDefs::ExtinguishFire(std::optional<CVector> position, std::optional<float> radius)
{
    if (!position.has_value())
    {
        g_pGame->GetFireManager()->ExtinguishAllFires();
        return true; // backwards compatibilty
    }

    g_pGame->GetFireManager()->ExtinguishPoint(position.value(), radius.value_or(1.0f));
    return true; // backwards compatibilty
}
