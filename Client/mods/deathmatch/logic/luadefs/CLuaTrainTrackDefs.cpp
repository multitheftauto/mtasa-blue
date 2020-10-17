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

int CLuaTrainTrackDefs::GetTrackNodeCount(lua_State* luaVM)
{
    CClientTrainTrack* pTrack;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTrack);

    if (!argStream.HasErrors())
    {
        lua_pushinteger(luaVM, pTrack->GetNumberOfNodes());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTrainTrackDefs::GetTrackNodePosition(lua_State* luaVM)
{
    CClientTrainTrack* pTrainTrack;
    uint nodeIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTrainTrack);
    argStream.ReadNumber(nodeIndex);

    if (!argStream.HasErrors())
    {
        CVector position;
        if (pTrainTrack->GetNodePosition(nodeIndex, position))
        {
            lua_pushnumber(luaVM, position.fX);
            lua_pushnumber(luaVM, position.fY);
            lua_pushnumber(luaVM, position.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

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
