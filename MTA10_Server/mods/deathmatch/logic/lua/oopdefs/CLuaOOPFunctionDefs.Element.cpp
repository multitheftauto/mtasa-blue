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

int CLuaOOPDefs::GetElementMatrix(lua_State* luaVM)
{
    CElement* pEntity = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        CMatrix matrix;
        pEntity->GetMatrix ( matrix );

        lua_pushmatrix ( luaVM, matrix );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage ( ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::GetElementPosition ( lua_State* luaVM )
{
    CElement* pElement = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        CVector vector = pElement->GetPosition();

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
    CElement* pElement = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        CMatrix matrix;
        CVector vecRotation;
        pElement->GetMatrix ( matrix );

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
    CElement* pElement = NULL;
    CVector vecRotation;
    eEulerRotationOrder rotationOrder = EULER_DEFAULT;
    bool bNewWay = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecRotation );
    /*argStream.ReadString ( strRotationOrder, "default" );
    argStream.ReadBool ( bNewWay, false );*/

    if ( !argStream.HasErrors ( ) )
    {
        CMatrix matrix;

        // fill in our matrix
        pElement->GetMatrix ( matrix );

        // degrees to radians
        ConvertDegreesToRadians ( vecRotation );

        // set the matrix rotation (takes radians)
        matrix.SetRotation ( vecRotation );

        // set the element matrix using the element specific SetMatrix function
        pElement->SetMatrix ( matrix );

        // get the rotation (outputs radians)
        pElement->GetRotation( vecRotation );

        // convert radians to degrees
        ConvertRadiansToDegrees ( vecRotation );

        if ( CStaticFunctionDefinitions::SetElementRotation ( pElement, vecRotation, rotationOrder, bNewWay ) )
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
