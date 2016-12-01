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

void CLuaTrainTrackDefs::LoadFunctions()
{
    CLuaCFunctions::AddFunction("createTrack", CreateTrack);
    CLuaCFunctions::AddFunction("getDefaultTrack", GetDefaultTrack);

    CLuaCFunctions::AddFunction("getTrackNodePosition", GetTrackNodePosition);
    CLuaCFunctions::AddFunction("setTrackNodePosition", SetTrackNodePosition);

    CLuaCFunctions::AddFunction("getTrackNodeCount", GetTrackNodeCount);
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createTrack");
    lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

    lua_classfunction(luaVM, "getNodePosition", "getTrackNodePosition");
    lua_classfunction(luaVM, "setNodePosition", "setTrackNodePosition");

    lua_classfunction(luaVM, "getNodeCount", "getTrackNodeCount");

    lua_registerclass(luaVM, "TrainTrack", "Element");
}

int CLuaTrainTrackDefs::CreateTrack(lua_State* luaVM)
{
//  train-track createTrack ( table nodes, bool linkNodes )
    bool linkLastNodes; std::vector<CLuaVector3D*> vecNodeList;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserDataTable(vecNodeList);
    argStream.ReadBool(linkLastNodes);

    if (!argStream.HasErrors())
    {
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

int CLuaTrainTrackDefs::GetDefaultTrack(lua_State* luaVM)
{
    uint trackId;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(trackId);

    if (!argStream.HasErrors())
    {
        if (trackId <= 3)
        {
            CTrainTrack* pTrainTrack = g_pGame->GetTrainTrackManager()->GetTrainTrackByIndex(trackId);
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
            lua_pushvector(luaVM, position);
            return 1;
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
