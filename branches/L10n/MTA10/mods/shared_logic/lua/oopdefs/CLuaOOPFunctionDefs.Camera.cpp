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
    CMatrix matrix;

    // camera has it different
    matrix.vRight = -matrix.vRight;
    m_pManager->GetCamera ()->GetMatrix ( matrix );

    lua_pushmatrix ( luaVM, matrix );
    return 1;
}

int CLuaOOPDefs::GetCameraPosition ( lua_State* luaVM )
{
    CVector vecPosition;
    m_pManager->GetCamera ()->GetPosition ( vecPosition );

    lua_pushvector ( luaVM, vecPosition );
    return 1;
}

int CLuaOOPDefs::SetCameraPosition ( lua_State* luaVM )
{
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors () )
    {
        CClientCamera* pCamera = m_pManager->GetCamera ();
        if ( !pCamera->IsInFixedMode () )        
        {
            pCamera->ToggleCameraFixedMode ( true );
        }

        pCamera->SetPosition ( vecPosition );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::GetCameraRotation ( lua_State* luaVM )
{
    CVector vecPosition;
    m_pManager->GetCamera ()->GetRotationDegrees ( vecPosition );

    lua_pushvector ( luaVM, vecPosition );
    return 1;
}

int CLuaOOPDefs::SetCameraRotation ( lua_State* luaVM )
{
    CVector vecRotation;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecRotation );

    if ( !argStream.HasErrors () )
    {
        CClientCamera* pCamera = m_pManager->GetCamera ();
        if ( !pCamera->IsInFixedMode () )        
        {
            pCamera->ToggleCameraFixedMode ( true );
        }

        pCamera->SetRotationDegrees ( vecRotation );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}