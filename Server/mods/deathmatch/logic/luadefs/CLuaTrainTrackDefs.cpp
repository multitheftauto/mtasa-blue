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
    CLuaCFunctions::AddFunction("createTrack", CreateTrainTrack);

    //CLuaCFunctions::AddFunction ( "getTrackNode", GetTrainTrackPosition );
    //CLuaCFunctions::AddFunction ( "getTrackNodes", GetTrainTrackNumberOfNodes );
}


void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createTrack");

    //lua_classfunction(luaVM, "getNode", "getTrackNode");
    //lua_classfunction(luaVM, "getNodes", "getTrackNodes");

    lua_registerclass(luaVM, "TrainTrack", "Element");
}

int CLuaTrainTrackDefs::GetTrainTrackPosition(lua_State* luaVM)
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

int CLuaTrainTrackDefs::CreateTrainTrack(lua_State* luaVM)
{
    bool linkLastNodes;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(linkLastNodes);

    // TODO: Refactor the following
    std::vector<CVector> vecNodeList;
    for (uint i = 0; i == 0 || argStream.NextIsVector3D(); i++)
    {
        CVector vecNode;
        argStream.ReadVector3D(vecNode);
        vecNodeList.push_back(vecNode);
    }

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

