/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Camera.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::GetCameraMatrix ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        CMatrix matrix;
        //pPlayer->GetCamera ()->GetMatrix ( matrix );

        lua_pushmatrix ( luaVM, matrix );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}