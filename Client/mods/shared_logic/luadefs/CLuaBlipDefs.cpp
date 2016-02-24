/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaBlipDefs.cpp
*  PURPOSE:     Lua blip definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaBlipDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "createBlip", CreateBlip );
    CLuaCFunctions::AddFunction ( "createBlipAttachedTo", CreateBlipAttachedTo );
    CLuaCFunctions::AddFunction ( "getBlipIcon", GetBlipIcon );
    CLuaCFunctions::AddFunction ( "getBlipSize", GetBlipSize );
    CLuaCFunctions::AddFunction ( "getBlipColor", GetBlipColor );
    CLuaCFunctions::AddFunction ( "getBlipOrdering", GetBlipOrdering );
    CLuaCFunctions::AddFunction ( "getBlipVisibleDistance", GetBlipVisibleDistance );

    CLuaCFunctions::AddFunction ( "setBlipIcon", SetBlipIcon );
    CLuaCFunctions::AddFunction ( "setBlipSize", SetBlipSize );
    CLuaCFunctions::AddFunction ( "setBlipColor", SetBlipColor );
    CLuaCFunctions::AddFunction ( "setBlipOrdering", SetBlipOrdering );
    CLuaCFunctions::AddFunction ( "setBlipVisibleDistance", SetBlipVisibleDistance );
}


void CLuaBlipDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createBlip" );
    lua_classfunction ( luaVM, "createAttachedTo", "createBlipAttachedTo" );

    lua_classfunction ( luaVM, "getColor", "getBlipColor" );
    lua_classfunction ( luaVM, "getVisibleDistance", "getBlipVisibleDistance" );
    lua_classfunction ( luaVM, "getOrdering", "getBlipOrdering" );
    lua_classfunction ( luaVM, "getSize", "getBlipSize" );
    lua_classfunction ( luaVM, "getIcon", "getBlipIcon" );

    lua_classfunction ( luaVM, "setColor", "setBlipColor" );
    lua_classfunction ( luaVM, "setVisibleDistance", "setBlipVisibleDistance" );
    lua_classfunction ( luaVM, "setOrdering", "setBlipOrdering" );
    lua_classfunction ( luaVM, "setSize", "setBlipSize" );
    lua_classfunction ( luaVM, "setIcon", "setBlipIcon" );

    lua_classvariable ( luaVM, "icon", "setBlipIcon", "getBlipIcon" );
    lua_classvariable ( luaVM, "size", "setBlipSize", "getBlipSize" );
    lua_classvariable ( luaVM, "ordering", "setBlipOrdering", "getBlipOrdering" );
    lua_classvariable ( luaVM, "visibleDistance", "setBlipVisibleDistance", "getBlipVisibleDistance" );
    //lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor" );

    lua_registerclass ( luaVM, "Blip", "Element" );
}


int CLuaBlipDefs::CreateBlip ( lua_State* luaVM )
{
    CVector vecPosition;
    unsigned char ucIcon = 0;
    unsigned char ucSize = 2;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering = 0;
    unsigned short usVisibleDistance = 16383;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( ucIcon, 0 );
    argStream.ReadNumber ( ucSize, 2 );
    argStream.ReadNumber ( color.R, 255 );
    argStream.ReadNumber ( color.G, 0 );
    argStream.ReadNumber ( color.B, 0 );
    argStream.ReadNumber ( color.A, 255 );
    argStream.ReadNumber ( sOrdering, 0 );
    argStream.ReadNumber ( usVisibleDistance, 16383 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create the blip
                CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlip ( *pResource, vecPosition, ucIcon, ucSize, color, sOrdering, usVisibleDistance );
                if ( pMarker )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( pMarker );
                    }

                    lua_pushelement ( luaVM, pMarker );
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


int CLuaBlipDefs::CreateBlipAttachedTo ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    // Default colors and size
    unsigned char ucIcon = 0;
    unsigned char ucSize = 2;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering = 0;
    unsigned short usVisibleDistance = 16383;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucIcon, 0 );
    argStream.ReadNumber ( ucSize, 2 );
    argStream.ReadNumber ( color.R, 255 );
    argStream.ReadNumber ( color.G, 0 );
    argStream.ReadNumber ( color.B, 0 );
    argStream.ReadNumber ( color.A, 255 );
    argStream.ReadNumber ( sOrdering, 0 );
    argStream.ReadNumber ( usVisibleDistance, 16383 );

    // Element in place?
    if ( !argStream.HasErrors () )
    {
        // Grab the element and verify it
        if ( pEntity )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResource* pResource = pLuaMain->GetResource ();
                if ( pResource )
                {
                    // Create the blip
                    CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlipAttachedTo ( *pResource, *pEntity, ucIcon, ucSize, color, sOrdering, usVisibleDistance );
                    if ( pMarker )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup ();
                        if ( pGroup )
                        {
                            pGroup->Add ( pMarker );
                        }
                        lua_pushelement ( luaVM, pMarker );
                        return 1;
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::GetBlipIcon ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        if ( pMarker )
        {
            unsigned char ucIcon = static_cast < unsigned char > ( pMarker->GetSprite () );
            lua_pushnumber ( luaVM, ucIcon );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::GetBlipSize ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        if ( pMarker )
        {
            unsigned char ucSize = static_cast < unsigned char > ( pMarker->GetScale () );
            lua_pushnumber ( luaVM, ucSize );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::GetBlipColor ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        if ( pMarker )
        {
            SColor color = pMarker->GetColor ();
            lua_pushnumber ( luaVM, color.R );
            lua_pushnumber ( luaVM, color.G );
            lua_pushnumber ( luaVM, color.B );
            lua_pushnumber ( luaVM, color.A );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::GetBlipOrdering ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        if ( pMarker )
        {
            short sOrdering = pMarker->GetOrdering ();
            lua_pushnumber ( luaVM, sOrdering );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::GetBlipVisibleDistance ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        if ( pMarker )
        {
            unsigned short usVisibleDistance = pMarker->GetVisibleDistance ();
            lua_pushnumber ( luaVM, usVisibleDistance );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::SetBlipIcon ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    unsigned char ucIcon = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucIcon );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipIcon ( *pEntity, ucIcon ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::SetBlipSize ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    unsigned char ucSize = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucSize );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipSize ( *pEntity, ucSize ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::SetBlipColor ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    SColor color;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( color.R );
    argStream.ReadNumber ( color.G );
    argStream.ReadNumber ( color.B );
    argStream.ReadNumber ( color.A );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipColor ( *pEntity, color ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::SetBlipOrdering ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    short sOrdering;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( sOrdering );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipOrdering ( *pEntity, sOrdering ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaBlipDefs::SetBlipVisibleDistance ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    unsigned short usVisibleDistance;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( usVisibleDistance );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipVisibleDistance ( *pEntity, usVisibleDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}