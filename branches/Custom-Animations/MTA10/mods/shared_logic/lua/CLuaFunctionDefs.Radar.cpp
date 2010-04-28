/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Radar.cpp
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

int CLuaFunctionDefs::CreateRadarArea ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );
    int iArgument8 = lua_type ( luaVM, 8 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNONE ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNONE ) &&
        ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING || iArgument7 == LUA_TNONE ) &&
        ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING || iArgument8 == LUA_TNONE ) )
    {
        CVector2D vecPosition = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ), static_cast < float > ( lua_tonumber ( luaVM, 2 ) ) );
        CVector2D vecSize = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ), static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );

        double dRed = 255;
        double dGreen = 0;
        double dBlue = 0;
        double dAlpha = 255;
        if ( iArgument5 != LUA_TNONE && iArgument6 != LUA_TNONE && iArgument7 != LUA_TNONE && iArgument8 != LUA_TNONE )
        {
            dRed = lua_tonumber ( luaVM, 5 );
            dGreen = lua_tonumber ( luaVM, 6 );
            dBlue = lua_tonumber ( luaVM, 7 );
            dAlpha = lua_tonumber ( luaVM, 8 );
        }


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

            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResource* pResource = pLuaMain->GetResource ();
                if ( pResource )
                {
                    // Create it
                    CClientRadarArea* pRadarArea = CStaticFunctionDefinitions::CreateRadarArea ( *pResource, vecPosition, vecSize, color );
                    if ( pRadarArea )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
                        if ( pGroup )
                        {
                            pGroup->Add ( ( CClientEntity* ) pRadarArea );
                        }
                        lua_pushelement ( luaVM, pRadarArea );
                        return 1;
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Bad color number sent to createRadarArea (0-255)" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createRadarArea" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetRadarAreaColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            SColor color;
            if ( CStaticFunctionDefinitions::GetRadarAreaColor ( pRadarArea, color ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.R ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.G ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.B ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.A ) );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getRadarAreaColor", "radararea", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getRadarAreaColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetRadarAreaSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            CVector2D vecSize;
            if ( CStaticFunctionDefinitions::GetRadarAreaSize ( pRadarArea, vecSize ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( vecSize.fX ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( vecSize.fY ) );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getRadarAreaSize", "radararea", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getRadarAreaSize" );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsRadarAreaFlashing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            bool bFlashing = false;
            CStaticFunctionDefinitions::IsRadarAreaFlashing ( pRadarArea, bFlashing );
            lua_pushboolean ( luaVM, bFlashing );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isRadarAreaFlashing", "radararea", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isRadarAreaFlashing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetRadarAreaColor ( lua_State* luaVM )
{
    int iR = lua_type ( luaVM, 2 );
    int iG = lua_type ( luaVM, 3 );
    int iB = lua_type ( luaVM, 4 );
    int iA = lua_type ( luaVM, 5 );

    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iR == LUA_TNUMBER || iR == LUA_TSTRING ) &&
        ( iG == LUA_TNUMBER || iG == LUA_TSTRING ) &&
        ( iB == LUA_TNUMBER || iB == LUA_TSTRING ) &&
        ( iA == LUA_TNUMBER || iA == LUA_TSTRING ) )
    {
        double dRed = lua_tonumber ( luaVM, 2 );
        double dGreen = lua_tonumber ( luaVM, 3 );
        double dBlue = lua_tonumber ( luaVM, 4 );
        double dAlpha = lua_tonumber ( luaVM, 5 );

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

            CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
            if ( pRadarArea )
            {
                if ( CStaticFunctionDefinitions::SetRadarAreaColor ( pRadarArea, color ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setRadarAreaColor", "element", 1 );
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Bad color values sent to setRadarAreaColor (0-255)" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRadarAreaColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetRadarAreaSize ( lua_State* luaVM )
{
    int iX = lua_type ( luaVM, 2 );
    int iY = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iX == LUA_TNUMBER || iX == LUA_TSTRING ) &&
        ( iY == LUA_TNUMBER || iY == LUA_TSTRING ) )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            CVector2D vecSize = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ), static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
            if ( CStaticFunctionDefinitions::SetRadarAreaSize ( pRadarArea, vecSize ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setRadarAreaSize", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRadarAreaSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetRadarAreaFlashing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            if ( CStaticFunctionDefinitions::SetRadarAreaFlashing ( pRadarArea, lua_toboolean ( luaVM, 2 ) ? true:false ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setRadarAreaFlashing", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRadarAreaFlashing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsInsideRadarArea ( lua_State* luaVM )
{
    int iX = lua_type ( luaVM, 2 );
    int iY = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iX == LUA_TNUMBER || iX == LUA_TSTRING ) &&
        ( iY == LUA_TNUMBER || iY == LUA_TSTRING ) )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            bool bInside = false;
            CVector2D vecPosition = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ), static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
            if ( CStaticFunctionDefinitions::IsInsideRadarArea ( pRadarArea, vecPosition, bInside ) )
            {
                lua_pushboolean ( luaVM, bInside );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isInsideRadarArea", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isInsideRadarArea" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


