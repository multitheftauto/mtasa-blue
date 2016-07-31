/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaColShapeDefs.cpp
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaColShapeDefs::LoadFunctions ()
{
    // Shape create funcs
    CLuaCFunctions::AddFunction ( "createColCircle", CreateColCircle );
    CLuaCFunctions::AddFunction ( "createColCuboid", CreateColCuboid );
    CLuaCFunctions::AddFunction ( "createColSphere", CreateColSphere );
    CLuaCFunctions::AddFunction ( "createColRectangle", CreateColRectangle );
    CLuaCFunctions::AddFunction ( "createColPolygon", CreateColPolygon );
    CLuaCFunctions::AddFunction ( "createColTube", CreateColTube );
}

void CLuaColShapeDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "Circle", "createColCircle" );
    lua_classfunction ( luaVM, "Cuboid", "createColCuboid" );
    lua_classfunction ( luaVM, "Rectangle", "createColRectangle" );
    lua_classfunction ( luaVM, "Sphere", "createColSphere" );
    lua_classfunction ( luaVM, "Tube", "createColTube" );
    lua_classfunction ( luaVM, "Polygon", "createColPolygon" );

    lua_classfunction ( luaVM, "getElementsWithin", "getElementsWithinColShape" );
    lua_registerclass ( luaVM, "ColShape", "Element" );
}

int CLuaColShapeDefs::CreateColCircle ( lua_State* luaVM )
{
    CVector2D vecPosition;
    float fRadius;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector2D ( vecPosition );
    argStream.ReadNumber ( fRadius );

    if ( !argStream.HasErrors () )
    {
        if ( fRadius < 0.0f ) fRadius = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle ( pResource, vecPosition, fRadius );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;

}


int CLuaColShapeDefs::CreateColCuboid ( lua_State* luaVM )
{
    CVector vecPosition;
    CVector vecSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecSize );


    if ( !argStream.HasErrors () )
    {
        if ( vecSize.fX < 0.0f ) vecSize.fX = 0.1f;
        if ( vecSize.fY < 0.0f ) vecSize.fY = 0.1f;
        if ( vecSize.fZ < 0.0f ) vecSize.fZ = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid ( pResource, vecPosition, vecSize );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;

}


int CLuaColShapeDefs::CreateColSphere ( lua_State* luaVM )
{
    CVector vecPosition;
    float fRadius;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fRadius );

    if ( !argStream.HasErrors () )
    {
        if ( fRadius < 0.0f ) fRadius = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere ( pResource, vecPosition, fRadius );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;

}


int CLuaColShapeDefs::CreateColRectangle ( lua_State* luaVM )
{
    CVector2D vecPosition;
    CVector2D vecSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector2D ( vecPosition );
    argStream.ReadVector2D ( vecSize );

    if ( !argStream.HasErrors () )
    {
        if ( vecSize.fX < 0.0f ) vecSize.fX = 0.1f;
        if ( vecSize.fY < 0.0f ) vecSize.fY = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle ( pResource, vecPosition, vecSize );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;

}

int CLuaColShapeDefs::CreateColPolygon ( lua_State* luaVM )
{
    //  colshape createColPolygon ( float fX, float fY, float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, ... )
    std::vector < CVector2D > vecPointList;

    CScriptArgReader argStream ( luaVM );
    for ( uint i = 0; i < 4 || argStream.NextCouldBeNumber (); i++ )
    {
        CVector2D vecPoint;
        argStream.ReadVector2D ( vecPoint );
        vecPointList.push_back ( vecPoint );
    }

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon ( pResource, vecPointList );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;

}

int CLuaColShapeDefs::CreateColTube ( lua_State* luaVM )
{
    CVector vecPosition;
    float fHeight, fRadius;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fRadius );
    argStream.ReadNumber ( fHeight );

    if ( !argStream.HasErrors () )
    {
        if ( fRadius < 0.0f ) fRadius = 0.1f;
        if ( fHeight < 0.0f ) fHeight = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CColTube* pShape = CStaticFunctionDefinitions::CreateColTube ( pResource, vecPosition, fRadius, fHeight );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
