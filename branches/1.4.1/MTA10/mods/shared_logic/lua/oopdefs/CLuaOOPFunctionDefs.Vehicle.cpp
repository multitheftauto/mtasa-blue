/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Vehicle.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::GetVehicleGravity ( lua_State* luaVM )
{
    CClientVehicle* pVehicle = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector vecGravity;
        pVehicle->GetGravity ( vecGravity );

        lua_pushvector ( luaVM, vecGravity );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}