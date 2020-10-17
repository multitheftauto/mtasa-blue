/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaTrainTrackDefs.cpp
*  PURPOSE:     Lua train track definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"

int CLuaTrainTrackDefs::CreateTrack(lua_State* luaVM)
{
//  train-track createTrack ( table nodes, bool linkNodes )
    bool linkLastNodes; std::vector<CLuaVector3D*> vecNodeList;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserDataTable(vecNodeList);
    argStream.ReadBool(linkLastNodes);

    if (!argStream.HasErrors())
    {
        if (vecNodeList.size() < 2)
            return luaL_error(luaVM, "Please provide at least two nodes");

        auto pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            auto pResource = pLuaMain->GetResource();
            if (pResource)
            {
                auto pTrainTrack = CStaticFunctionDefinitions::CreateTrainTrack(pResource, vecNodeList, linkLastNodes);
                if (pTrainTrack)
                {
                    auto pGroup = pResource->GetElementGroup();
                    if (pGroup)
                        pGroup->Add(pTrainTrack);

                    lua_pushelement(luaVM, pTrainTrack);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTrainTrackDefs::GetTrackNodePosition(lua_State* luaVM)
{
    CTrainTrack* pTrack;
    uint nodeIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTrack);
    argStream.ReadNumber(nodeIndex);

    if (!argStream.HasErrors())
    {
        CVector position;
        if (pTrack->GetTrackNodePosition(nodeIndex, position))
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

int CLuaTrainTrackDefs::SetTrackNodePosition(lua_State* luaVM)
{
    CTrainTrack* pTrack;
    uint nodeIndex;
    CVector position;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTrack);
    argStream.ReadNumber(nodeIndex);
    argStream.ReadVector3D(position);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pTrack->SetTrackNodePosition(nodeIndex, position));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTrainTrackDefs::GetTrackNodeCount(lua_State* luaVM)
{
    CTrainTrack* pTrack;

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
