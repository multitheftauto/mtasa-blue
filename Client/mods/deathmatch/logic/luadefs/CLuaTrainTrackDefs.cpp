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
    //CLuaCFunctions::AddFunction ( "getTrackNodes", GetTrainTrackNumberOfNodes );
    CLuaCFunctions::AddFunction("getTrackID", GetTrainTrackID);
    CLuaCFunctions::AddFunction("getTrackLength", GetTrainTrackLength);
    CLuaCFunctions::AddFunction("getTrackNode", GetTrainTrackPosition);
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getTrackID", "getTrackID");
    lua_classfunction(luaVM, "getLength", "getTrackLength");
    lua_classfunction(luaVM, "getNode", "getTrackNode");
    lua_classfunction(luaVM, "getNodes", "getTrackNodes");

    lua_classvariable(luaVM, "length", nullptr, "getTrackLength");
    lua_classvariable(luaVM, "trackID", nullptr, "getTrackID");

    lua_registerclass(luaVM, "TrainTrack", "Element");
}

int CLuaTrainTrackDefs::GetTrainTrackPosition(lua_State* luaVM)
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

int CLuaTrainTrackDefs::GetTrainTrackLength(lua_State* luaVM)
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

int CLuaTrainTrackDefs::GetTrainTrackID(lua_State* luaVM)
{
    CClientTrainTrack* pTrainTrack;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTrainTrack);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pTrainTrack->GetTrackIndex());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
