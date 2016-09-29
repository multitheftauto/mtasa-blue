/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaColShapeDefs.cpp
*  PURPOSE:     Lua definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaColShapeDefs::LoadFunctions ( void )
{
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
    lua_classvariable ( luaVM, "elementsWithin", NULL, "getElementsWithinColShape" );

    lua_registerclass ( luaVM, "ColShape", "Element" );
}

int CLuaColShapeDefs::CreateColCircle ( lua_State* luaVM )
{
    CVector2D vecPosition;
    float fRadius = 0.1f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector2D ( vecPosition );
    argStream.ReadNumber ( fRadius );

    if ( fRadius < 0.0f )
    {
        fRadius = 0.1f;
    }

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CClientColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle ( *pResource, vecPosition, fRadius );
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
    CVector vecPosition, vecSize;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecSize );

    if ( vecSize.fX < 0.0f )
    {
        vecSize.fX = 0.1f;
    }
    if ( vecSize.fY < 0.0f )
    {
        vecSize.fY = 0.1f;
    }

    if ( !argStream.HasErrors () )
    {
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
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( (CClientEntity*) pShape );
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
    float fRadius = 0.1f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fRadius );

    if ( fRadius < 0.0f )
    {
        fRadius = 0.1f;
    }

    if ( !argStream.HasErrors () )
    {
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
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( (CClientEntity*) pShape );
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

    if ( vecSize.fX < 0.0f )
    {
        vecSize.fX = 0.1f;
    }
    if ( vecSize.fY < 0.0f )
    {
        vecSize.fY = 0.1f;
    }

    if ( !argStream.HasErrors () )
    {
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
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( (CClientEntity*) pShape );
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
    CVector2D vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector2D ( vecPosition );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it and return it
                CClientColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon ( *pResource, vecPosition );
                if ( pShape )
                {
                    // Get the points
                    while ( argStream.NextCouldBeNumber () && argStream.NextCouldBeNumber ( 1 ) )
                    {
                        argStream.ReadVector2D ( vecPosition );
                        pShape->AddPoint ( vecPosition );
                    }

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
    float fRadius = 0.1f, fHeight = 0.1f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fRadius );
    argStream.ReadNumber ( fHeight );

    if ( fRadius < 0.0f )
    {
        fRadius = 0.1f;
    }

    if ( fHeight < 0.0f )
    {
        fHeight = 0.1f;
    }

    if ( !argStream.HasErrors () )
    {
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
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( (CClientEntity*) pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}
