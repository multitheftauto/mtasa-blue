/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaTrainTrackDefs.cpp
*  PURPOSE:     Lua train track definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"

int CLuaTrainTrackDefs::GetTrackLength(lua_State* luaVM)
{
    CClientTrainTrack* pTrainTrack;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTrainTrack);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pTrainTrack->GetLength());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
