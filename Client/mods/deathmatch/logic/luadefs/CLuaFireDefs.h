/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaFireDefs.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CLuaDefs.h"

class CLuaFireDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static CClientFire* CreateFire(CVector position, float size, std::optional<std::uint32_t> lifetime, std::optional<bool> makeNoise);
    static bool         ExtinguishFire(std::optional<CVector> position, std::optional<float> radius);
};
