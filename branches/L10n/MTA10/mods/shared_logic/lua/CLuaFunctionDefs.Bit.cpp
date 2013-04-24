/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Bit.cpp
*  PURPOSE:     Lua bitwise operators definitions class
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define mask(n) (~((0xFFFFFFFF << 1) << ((n) - 1)))

int CLuaFunctionDefs::BitAnd ( lua_State* luaVM )
{
//  uint bitAnd ( uint var1, uint var2, ... )
    uint uiVar1; uint uiVar2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar1 );
    argStream.ReadNumber ( uiVar2 );

    if ( !argStream.HasErrors () )
    {
        uint uiResult = uiVar1 & uiVar2;
        while ( argStream.NextIsNumber () )
        {
            uint uiVar;
            argStream.ReadNumber ( uiVar );

            uiResult &= uiVar;
        }

        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitNot ( lua_State* luaVM )
{
//  uint bitNot ( uint var )
    uint uiVar;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    
    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, ~uiVar );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitOr ( lua_State* luaVM )
{
//  uint bitOr ( uint var1, uint var2, ... )
    uint uiVar1; uint uiVar2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar1 );
    argStream.ReadNumber ( uiVar2 );

    if ( !argStream.HasErrors () )
    {
        uint uiResult = uiVar1 | uiVar2;
        while ( argStream.NextIsNumber () )
        {
            uint uiVar;
            argStream.ReadNumber ( uiVar );

            uiResult |= uiVar;
        }

        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitTest ( lua_State* luaVM )
{
//  uint bitTest ( uint var1, uint var2, ... )
    uint uiVar1; uint uiVar2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar1 );
    argStream.ReadNumber ( uiVar2 );

    if ( !argStream.HasErrors () )
    {
        uint uiResult = uiVar1 & uiVar2;
        while ( argStream.NextIsNumber () )
        {
            uint uiVar;
            argStream.ReadNumber ( uiVar );

            uiResult &= uiVar;
        }

        bool bTested = uiResult != 0;
        lua_pushboolean ( luaVM, bTested );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::BitXor ( lua_State* luaVM )
{
//  uint bitXor ( uint var1, uint var2 )
    uint uiVar1; uint uiVar2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar1 );
    argStream.ReadNumber ( uiVar2 );

    if ( !argStream.HasErrors () )
    {
        uint uiResult = uiVar1 ^ uiVar2;
        while ( argStream.NextIsNumber () )
        {
            uint uiVar;
            argStream.ReadNumber ( uiVar );

            uiResult ^= uiVar;
        }

        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitLRotate ( lua_State* luaVM )
{
//  uint bitLRotate ( uint var, int disp )
    uint uiVar; int iDisp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    argStream.ReadNumber ( iDisp );

    if ( !argStream.HasErrors () )
    {
        iDisp &= (32 - 1); // iDisp %= 32
        uint uiResult = (uiVar << iDisp) | (uiVar >> (32 - iDisp));
        
        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitRRotate ( lua_State* luaVM )
{
//  uint bitRRotate ( uint var, int disp )
    uint uiVar; int iDisp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    argStream.ReadNumber ( iDisp );

    if ( !argStream.HasErrors () )
    {
        iDisp &= (32 - 1); // iDisp %= 32
        uint uiResult = (uiVar >> iDisp) | (uiVar << (32 - iDisp));
        
        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitLShift ( lua_State* luaVM )
{
//  uint bitLShift ( uint var, int disp )
    uint uiVar; int iDisp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    argStream.ReadNumber ( iDisp );

    if ( !argStream.HasErrors () )
    {
        uint uiResult;

        if ( iDisp >= 32 )
            uiResult = 0;
        else
            uiResult = uiVar << iDisp;

        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitRShift ( lua_State* luaVM )
{
//  uint bitRShift ( uint var, int disp )
    uint uiVar; int iDisp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    argStream.ReadNumber ( iDisp );

    if ( !argStream.HasErrors () )
    {
        uint uiResult;

        if ( iDisp >= 32 )
            uiResult = 0;
        else
            uiResult = uiVar >> iDisp;

        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitArShift ( lua_State* luaVM )
{
//  uint bitArShift ( uint var, int disp )
    uint uiVar; int iDisp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    argStream.ReadNumber ( iDisp );

    if ( !argStream.HasErrors () )
    {
        uint uiResult;

        if ( iDisp < 0 || !(uiVar & ((uint)1 << (32-1))) )
        {
            if ( iDisp <= -32 || iDisp >= 32 )
                uiResult = 0;
            else
            {
                if ( iDisp < 0 )
                    uiResult = uiVar << -iDisp;
                else
                    uiResult = uiVar >> iDisp;
            }

            lua_pushnumber ( luaVM, uiResult );
            return 1;
        }

        if ( iDisp >= 32 )
            uiResult = 0xFFFFFFFF;
        else
            uiResult = (uiVar >> iDisp) | ~(~(uint)0 >> iDisp);

        lua_pushnumber ( luaVM, uiResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitExtract ( lua_State* luaVM )
{
//  uint bitExtract ( uint var, int field, int width = 1 )
    uint uiVar; int iField; int iWidth;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    argStream.ReadNumber ( iField );
    argStream.ReadNumber ( iWidth, 1 );

    if ( !argStream.HasErrors () )
    {
        if ( iField + iWidth > 32 )
            argStream.SetCustomError ( "Trying to access non-exisistent bits" );

        if ( iField < 0 )
            argStream.SetCustomError ( "field cannot be negative" );

        if ( iWidth <= 0 )
            argStream.SetCustomError ( "width must be positive" );

        if ( !argStream.HasErrors () )
        {
            lua_pushnumber ( luaVM, (uiVar >> iField) & mask(iWidth) );
            return 1;
        }
    }
    
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BitReplace ( lua_State* luaVM )
{
//  uint bitReplace ( uint var, uint replaceValue, int field, int width = 1 )
    uint uiVar; uint uiReplaceValue; int iField; int iWidth;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiVar );
    argStream.ReadNumber ( uiReplaceValue );
    argStream.ReadNumber ( iField );
    argStream.ReadNumber ( iWidth, 1 );

    if ( !argStream.HasErrors () )
    {
        if ( iField + iWidth > 32 )
            argStream.SetCustomError ( "Trying to access non-exisistent bits" );

        if ( iField < 0 )
            argStream.SetCustomError ( "field cannot be negative" );

        if ( iWidth <= 0 )
            argStream.SetCustomError ( "width must be positive" );

        if ( !argStream.HasErrors () )
        {
            int iMask = mask(iWidth);

            // Erase bits outside given width
            uiReplaceValue &= iMask;

            lua_pushnumber ( luaVM, (uiVar & ~(iMask << iField)) | (uiReplaceValue << iField) );
            return 1;
        }
    }
    
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return false;
}
