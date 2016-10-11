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

void CLuaTrainTrackDefs::LoadFunctions()
{
    CLuaCFunctions::AddFunction("getTrackNodeCount", GetTrackNodeCount);
    CLuaCFunctions::AddFunction("getTrackNodePosition", GetTrackNodePosition);
    CLuaCFunctions::AddFunction("getTrackLength", GetTrackLength);

    CLuaCFunctions::AddFunction("getDefaultTrack", GetDefaultTrack);
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getNodeCount", "getTrackNodeCount");
    lua_classfunction(luaVM, "getNodePosition", "getTrackNodePosition");
    lua_classfunction(luaVM, "getLength", "getTrackLength");

    lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

    lua_classvariable(luaVM, "length", nullptr, "getTrackLength");

    lua_registerclass(luaVM, "TrainTrack", "Element");
}

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
            lua_pushvector(luaVM, position);
            return 1;
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

int CLuaTrainTrackDefs::GetDefaultTrack(lua_State* luaVM)
{
	uint trackId;
	
	CScriptArgReader argStream(luaVM);
	argStream.ReadNumber(trackId);

	if (!argStream.HasErrors())
	{
        if (trackId <= 3)
        {
            CClientTrainTrack* pTrainTrack = g_pClientGame->GetManager()->GetTrainTrackManager()->GetTrainTrackByIndex(trackId);
            if (pTrainTrack)
            {
                lua_pushelement(luaVM, pTrainTrack);
                return 1;
            }
        }
        else
            argStream.SetCustomError("Bad default track id (0-3)");
	}
	else
		m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

	lua_pushboolean(luaVM, false);
	return 1;
}
