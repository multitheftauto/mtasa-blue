/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.h
*  PURPOSE:     Lua function definitions class header
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

class CLuaFunctionDefs;

#ifndef __CLUAOOPDEFINITIONS_H
#define __CLUAOOPDEFINITIONS_H

#include "../LuaCommon.h"
#include "../CLuaMain.h"

class CRegisteredCommands;

class CLuaOOPDefs : public CLuaDefs
{
public:
    // Ped
    LUA_DECLARE ( SetPedOccupiedVehicle );

    // Player
    LUA_DECLARE ( OutputChat );

    // Connection
    LUA_DECLARE ( DbQuery );

    // RadarArea
    LUA_DECLARE ( GetRadarAreaSize );
};

#endif
