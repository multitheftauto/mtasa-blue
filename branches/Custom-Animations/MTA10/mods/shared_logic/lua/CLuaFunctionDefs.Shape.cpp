/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Shape.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::CreateColCircle ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), 0.0f );
        float fRadius = float ( lua_tonumber ( luaVM, 3 ) );
        if ( fRadius < 0.0f ) fRadius = 0.1f;

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CClientColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle ( *pResource, vecPosition, fRadius );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createColCircle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateColCuboid ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecSize = CVector ( float ( lua_tonumber ( luaVM, 4 ) ), float ( lua_tonumber ( luaVM, 5 ) ), float ( lua_tonumber ( luaVM, 6 ) ) );
        if ( vecSize.fX < 0.0f ) vecSize.fX = 0.1f;
        if ( vecSize.fY < 0.0f ) vecSize.fY = 0.1f;

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CClientColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid ( *pResource, vecPosition, vecSize );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createColCuboid" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateColSphere ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        float fRadius = float ( lua_tonumber ( luaVM, 4 ) );
        if ( fRadius < 0.0f ) fRadius = 0.1f;

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CClientColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere ( *pResource, vecPosition, fRadius );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createColSphere" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateColRectangle ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( 0 ) );
        CVector2D vecSize = CVector2D ( float ( lua_tonumber ( luaVM, 3 ) ), float ( lua_tonumber ( luaVM, 4 ) ) );
        if ( vecSize.fX < 0.0f ) vecSize.fX = 0.1f;
        if ( vecSize.fY < 0.0f ) vecSize.fY = 0.1f;

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CClientColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle ( *pResource, vecPosition, vecSize );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createColRectangle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateColPolygon ( lua_State* luaVM )
{ // Formerly createColSquare
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( ( float ) lua_tonumber ( luaVM, 1 ), ( float ) lua_tonumber ( luaVM, 2 ), 0.0f );

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CClientColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon ( *pResource, vecPosition );
                if ( pShape )
                {
                    // Get the points
                    int iArgument = 3;
                    int iArgumentX = lua_type ( luaVM, iArgument++ );
                    int iArgumentY = lua_type ( luaVM, iArgument++ );
                    while ( iArgumentX != LUA_TNONE && iArgumentY != LUA_TNONE )
                    {
                        if ( ( iArgumentX == LUA_TNUMBER || iArgumentX == LUA_TSTRING ) &&
                            ( iArgumentY == LUA_TNUMBER || iArgumentY == LUA_TSTRING ) )
                        {
                            pShape->AddPoint ( CVector2D ( ( float ) lua_tonumber ( luaVM, iArgument - 2 ),
                                ( float ) lua_tonumber ( luaVM, iArgument - 1 ) ) );

                            iArgumentX = lua_type ( luaVM, iArgument++ );
                            iArgumentY = lua_type ( luaVM, iArgument++ );
                        }
                        else
                        {
                            break;
                        }
                    }

                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createColPolygon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateColTube ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        float fRadius = float ( lua_tonumber ( luaVM, 4 ) );
        float fHeight = float ( lua_tonumber ( luaVM, 5 ) );
        if ( fRadius < 0.0f ) fRadius = 0.1f;
        if ( fHeight < 0.0f ) fRadius = 0.1f;

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CClientColTube* pShape = CStaticFunctionDefinitions::CreateColTube ( *pResource, vecPosition, fRadius, fHeight );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createColTube" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


