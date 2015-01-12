/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Water.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::CreateWater ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CVector v1, v2, v3, v4;
    CVector* pv4 = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( v1 );
    argStream.ReadVector3D ( v2 );
    argStream.ReadVector3D ( v3 );

    if ( argStream.NextCouldBeNumber () )
    {
        argStream.ReadVector3D ( v4 );
        pv4 = &v4;
    }

    if ( !argStream.HasErrors () )
    {
        CWater* pWater = CStaticFunctionDefinitions::CreateWater ( pLuaMain->GetResource (), &v1, &v2, &v3, pv4 );
        if ( pWater )
        {
            CElementGroup * pGroup = pLuaMain->GetResource ()->GetElementGroup ();
            if ( pGroup )
            {
                pGroup->Add ( pWater );
            }
            lua_pushelement ( luaVM, pWater );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWaterLevel ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    CResource* pResource = pLuaMain ? pLuaMain->GetResource () : NULL;
    if ( !pResource )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserData () )
    {
        // Call type 1
        //  bool setWaterLevel ( [water theWater,] float level )
        CWater* pWater; float fLevel;

        argStream.ReadUserData ( pWater );
        argStream.ReadNumber ( fLevel );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetElementWaterLevel ( pWater, fLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
    {
        // Call type 2
        //  bool setWaterLevel ( float level, bool bIncludeWorldNonSeaLevel, bool bIncludeAllWaterElements )
        float fLevel; bool bIncludeWorldNonSeaLevel; bool bIncludeAllWaterElements;

        argStream.ReadNumber ( fLevel );
        argStream.ReadBool ( bIncludeWorldNonSeaLevel, true );
        argStream.ReadBool ( bIncludeAllWaterElements, true );

        if ( !argStream.HasErrors () )
        {
            if ( bIncludeAllWaterElements )
                CStaticFunctionDefinitions::SetAllElementWaterLevel ( fLevel );
            if ( CStaticFunctionDefinitions::SetWorldWaterLevel ( fLevel, bIncludeWorldNonSeaLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetWaterLevel ( lua_State* luaVM )
{
    CStaticFunctionDefinitions::ResetWorldWaterLevel ();
    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::GetWaterVertexPosition ( lua_State* luaVM )
{
    CWater *pWater;
    int iVertexIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWater );
    argStream.ReadNumber ( iVertexIndex );

    if ( !argStream.HasErrors () )
    {
        CVector vecPosition;
        if ( CStaticFunctionDefinitions::GetWaterVertexPosition ( pWater, iVertexIndex, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetWaterVertexPosition ( lua_State* luaVM )
{
    CWater* pElement;
    int iVertex;
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( iVertex );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWaterVertexPosition ( pElement, iVertex, vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetWaterColor ( lua_State* luaVM )
{
    unsigned char ucRed, ucGreen, ucBlue, ucAlpha;

    bool bColorOverride = CStaticFunctionDefinitions::GetWaterColor ( ucRed, ucGreen, ucBlue, ucAlpha );

    if ( bColorOverride )
    {
        lua_pushnumber ( luaVM, ucRed );
        lua_pushnumber ( luaVM, ucGreen );
        lua_pushnumber ( luaVM, ucBlue );
        lua_pushnumber ( luaVM, ucAlpha );

        return 4;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetWaterColor ( lua_State* luaVM )
{
    unsigned char ucR;
    unsigned char ucG;
    unsigned char ucB;
    unsigned char ucA;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucR );
    argStream.ReadNumber ( ucG );
    argStream.ReadNumber ( ucB );
    argStream.ReadNumber ( ucA, 200 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWaterColor ( ucR, ucG, ucB, ucA ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetWaterColor ( lua_State *luaVM )
{
    bool bSuccess = CStaticFunctionDefinitions::ResetWaterColor ();

    lua_pushboolean ( luaVM, bSuccess );
    return 1;
}

