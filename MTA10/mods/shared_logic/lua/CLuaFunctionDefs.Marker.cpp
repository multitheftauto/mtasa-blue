/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Marker.cpp
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

int CLuaFunctionDefs::CreateMarker ( lua_State* luaVM )
{
    CVector vecPosition;
    float fSize = 4.0f;
    SColorRGBA color ( 0, 0, 255, 255 );
    SString strType = "default";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadString ( strType, "default" );
    argStream.ReadNumber ( fSize, 4.0f );
    argStream.ReadNumber ( color.R, 0 );
    argStream.ReadNumber ( color.G, 0 );
    argStream.ReadNumber ( color.B, 255 );
    argStream.ReadNumber ( color.A, 255 );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            {
                // Create it
                CClientMarker* pMarker = CStaticFunctionDefinitions::CreateMarker ( *pResource, vecPosition, strType, fSize, color );
                if ( pMarker )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pMarker );
                    }

                    lua_pushelement ( luaVM, pMarker );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMarkerCount ( lua_State* luaVM )
{
    unsigned int uiCount = m_pMarkerManager->Count ();
    lua_pushnumber ( luaVM, static_cast < lua_Number > ( uiCount ) );
    return 1;
}


int CLuaFunctionDefs::GetMarkerType ( lua_State* luaVM )
{
    // Verify the argument
    CClientMarker* pMarker = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            SString strMarkerType;
            if ( CClientMarker::TypeToString ( pMarker->GetMarkerType (), strMarkerType ) )
            {
                lua_pushstring ( luaVM, strMarkerType );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "marker", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMarkerSize ( lua_State* luaVM )
{
    // Verify the argument
    CClientMarker* pMarker = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            float fSize = pMarker->GetSize ();
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( fSize ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "marker", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMarkerColor ( lua_State* luaVM )
{
    // Verify the argument
    CClientMarker* pMarker = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            SColor color = pMarker->GetColor ();
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.R ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.G ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.B ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.A ) );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "marker", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMarkerTarget ( lua_State* luaVM )
{
    // Verify the argument
    CClientMarker* pMarker = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            CVector vecTarget;
            if ( CStaticFunctionDefinitions::GetMarkerTarget ( *pMarker, vecTarget ) )
            {
                lua_pushnumber ( luaVM, vecTarget.fX );
                lua_pushnumber ( luaVM, vecTarget.fY );
                lua_pushnumber ( luaVM, vecTarget.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "marker", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMarkerIcon ( lua_State* luaVM )
{
    // Verify the argument
    CClientMarker* pMarker = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            CClientCheckpoint* pCheckpoint = pMarker->GetCheckpoint ();
            if ( pCheckpoint )
            {
                SString strMarkerIcon;
                CClientCheckpoint::IconToString ( pCheckpoint->GetIcon (), strMarkerIcon );
                lua_pushstring ( luaVM, strMarkerIcon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "marker", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetMarkerType ( lua_State* luaVM )
{
    // Verify the argument
    SString strType = "";
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strType );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            bool bSuccess = CStaticFunctionDefinitions::SetMarkerType ( *pEntity, strType );
            lua_pushboolean ( luaVM, bSuccess );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetMarkerSize ( lua_State* luaVM )
{
    // Verify the argument
    float fSize = 0.0f;
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fSize );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetMarkerSize ( *pEntity, fSize ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetMarkerColor ( lua_State* luaVM )
{
    // Verify the argument
    double dRed = 0;
    double dGreen = 0;
    double dBlue = 0;
    double dAlpha = 255;
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( dRed );
    argStream.ReadNumber ( dGreen );
    argStream.ReadNumber ( dBlue );
    argStream.ReadNumber ( dAlpha, 255 );

    if ( !argStream.HasErrors ( ) )
    {
        // Verify the pointer
        if ( pEntity )
        {
            // Check the ranges
            if ( dRed >= 0 && dRed <= 255 &&
                dGreen >= 0 && dGreen <= 255 &&
                dBlue >= 0 && dBlue <= 255 &&
                dAlpha >= 0 && dAlpha <= 255 )
            {
                SColor color;
                color.R = static_cast < unsigned char > ( dRed );
                color.G = static_cast < unsigned char > ( dGreen );
                color.B = static_cast < unsigned char > ( dBlue );
                color.A = static_cast < unsigned char > ( dAlpha );

                // Set the new color
                if ( CStaticFunctionDefinitions::SetMarkerColor ( *pEntity, color ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetMarkerTarget ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CVector vecTarget;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( vecTarget.fX );
    argStream.ReadNumber ( vecTarget.fY );
    argStream.ReadNumber ( vecTarget.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        // Do it
        if ( CStaticFunctionDefinitions::SetMarkerTarget ( *pEntity, &vecTarget ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetMarkerIcon ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    SString strIcon = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strIcon );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetMarkerIcon ( *pEntity, strIcon ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


