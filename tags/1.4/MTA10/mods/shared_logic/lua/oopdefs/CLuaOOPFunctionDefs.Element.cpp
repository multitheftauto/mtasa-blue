/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Element.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::GetElementMatrix ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        CMatrix matrix;
        pEntity->GetMatrix ( matrix );

        lua_pushmatrix ( luaVM, matrix );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::GetElementPosition ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        CVector vector;
        pEntity->GetPosition ( vector );

        lua_pushvector ( luaVM, vector );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::GetElementRotation ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        CMatrix matrix;
        CVector vecRotation;
        pEntity->GetMatrix ( matrix );
        
        vecRotation = matrix.GetRotation ( );
        ConvertRadiansToDegrees ( vecRotation );

        lua_pushvector ( luaVM, vecRotation );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::SetElementRotation ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadVector3D ( vecRotation );

    if ( !argStream.HasErrors ( ) )
    {
        CMatrix matrix;
        pEntity->GetMatrix ( matrix );

        ConvertDegreesToRadians ( vecRotation );

        matrix.SetRotation ( vecRotation );

        pEntity->SetMatrix ( matrix );

        ConvertRadiansToDegrees ( vecRotation );

        eEulerRotationOrder rotationOrder = EULER_DEFAULT;
        if ( pEntity->GetType() == CCLIENTOBJECT )
            rotationOrder = EULER_ZYX;

        if ( CStaticFunctionDefinitions::SetElementRotation ( *pEntity, vecRotation, rotationOrder, true ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage ( ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}