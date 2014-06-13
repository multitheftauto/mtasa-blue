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
    // Needs further attention before adding
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

int CLuaOOPDefs::GetCameraPosition(lua_State* luaVM)
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors ( ) )
    {
        CVector vecPosition;
        CPlayerCamera * pCamera = pPlayer->GetCamera ( );
        if ( pCamera )
        {
            pCamera->GetPosition ( vecPosition );
            lua_pushvector ( luaVM, vecPosition );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::SetCameraPosition ( lua_State* luaVM )
{
    // Needs further attention before adding
    CPlayer* pPlayer;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors ( ) )
    {
        CPlayerCamera * pCamera = pPlayer->GetCamera ( );
        if ( pCamera )
        {
            /*if (!pCamera->IsInFixedMode())
            {
            pCamera->ToggleCameraFixedMode(true);
            }*/

            pCamera->SetPosition ( vecPosition );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::GetCameraRotation(lua_State* luaVM)
{
    // Needs further attention before adding
    CPlayer* pPlayer;
    CVector vecRotation;
    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors ( ) )
    {

        CPlayerCamera * pCamera = pPlayer->GetCamera ( );
        if ( pCamera )
        {
            // TODO
        }
        lua_pushvector ( luaVM, vecRotation );
        return 1;
    }
}

int CLuaOOPDefs::SetCameraRotation(lua_State* luaVM)
{
    // Needs further attention before adding
    CPlayer* pPlayer;
    CVector vecRotation;
    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pPlayer );

    argStream.ReadVector3D ( vecRotation );

    if (!argStream.HasErrors())
    {

        CPlayerCamera * pCamera = pPlayer->GetCamera ( );
        /*if (!pCamera->IsInFixedMode())
        {
            pCamera->ToggleCameraFixedMode(true);
        }*/

        // Needs to be in degrees
        pCamera->SetRotation(vecRotation);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}
