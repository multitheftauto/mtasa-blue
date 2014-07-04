/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/LuaUtils.cpp
*  PURPOSE:     Lua Utility functions
*  DEVELOPERS:  Cazomino05
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

SString GetDebugMessage ( lua_State* luaVM ) 
{
    SString strDebugMessage = "";
    lua_Debug debugInfo;    
    if ( luaVM && lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        if ( debugInfo.source[0] == '@' )
        {
            SString strFile = ConformResourcePath ( debugInfo.source );
            strDebugMessage = SString( "%s:%i", strFile.c_str (), debugInfo.currentline );
        }
        else
        {
            SString strFile = debugInfo.short_src;
            strDebugMessage = SString( "%s:%i", strFile.c_str (), debugInfo.currentline );
        }
    }
    return strDebugMessage;
}