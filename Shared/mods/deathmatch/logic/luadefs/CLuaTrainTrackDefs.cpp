/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

using TrainTrack = CLuaTrainTrackDefs::TrainTrack;
using TrainTrackManager = CLuaTrainTrackDefs::TrainTrackManager;

TrainTrackManager CLuaTrainTrackDefs::GetManager()
{
#ifdef MTA_CLIENT
    return g_pClientGame->GetManager()->GetTrainTrackManager();
#else
    return g_pGame->GetTrainTrackManager();
#endif
}

CResource* CLuaTrainTrackDefs::GetResource(lua_State* L)
{
#ifdef MTA_CLIENT
    auto game = g_pClientGame;
#else
    auto game = g_pGame;
#endif
    return game->GetLuaManager()->GetVirtualMachine(L)->GetResource();
}

void CLuaTrainTrackDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getDefaultTrack", ArgumentParser<GetDefaultTrack>},

#ifdef MTA_CLIENT
        {"getTrackLength", ArgumentParser<GetTrackLength>},
#else
        {"createTrack", ArgumentParser<CreateTrack>},
#endif

        {"getTrackNodeCount", ArgumentParser<GetTrackNodeCount>},
        {"getTrackNodePosition", ArgumentParser<GetTrackNodePosition>},

        {"setTrackNodePosition", ArgumentParser<SetTrackNodePosition>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

    lua_classfunction(luaVM, "getNodeCount", "getTrackNodeCount");
    lua_classfunction(luaVM, "getNodePosition", "getTrackNodePosition");

    lua_classfunction(luaVM, "setNodePosition", "setTrackNodePosition");

#ifdef MTA_CLIENT
    lua_classfunction(luaVM, "getLength", "getTrackLength");
    lua_classvariable(luaVM, "length", nullptr, "getTrackLength");
#else
    lua_classfunction(luaVM, "create", "createTrack");
#endif

    lua_registerclass(luaVM, "TrainTrack", "Element");
}

TrainTrack CLuaTrainTrackDefs::GetDefaultTrack(uchar trackID)
{
    if (trackID > 3)
        throw std::invalid_argument("Bad default track ID (0-3)");

    return GetManager()->GetTrainTrackByIndex(trackID);
}

int CLuaTrainTrackDefs::GetTrackNodeCount(TrainTrack track)
{
    return track->GetNumberOfNodes();
}

std::tuple<float, float, float> CLuaTrainTrackDefs::GetTrackNodePosition(TrainTrack track, uint nodeIndex)
{
    CVector position;
    track->GetNodePosition(nodeIndex, position);
    return {position.fX, position.fY, position.fZ};
}

bool CLuaTrainTrackDefs::SetTrackNodePosition(TrainTrack track, uint nodeIndex, CVector position)
{
    return track->SetNodePosition(nodeIndex, position);
}

#ifdef MTA_CLIENT
float CLuaTrainTrackDefs::GetTrackLength(TrainTrack track)
{
    return track->GetLength();
}
#else
TrainTrack CLuaTrainTrackDefs::CreateTrack(lua_State* L, std::vector<CVector> nodes, bool linkNodes)
{
    if (nodes.size() < 2)
        throw std::invalid_argument("Please provide at least two nodes");

    auto pResource = GetResource(L);
    auto pTrainTrack = CStaticFunctionDefinitions::CreateTrainTrack(pResource, nodes, linkNodes);
    auto pGroup = pResource->GetElementGroup();
    if (pGroup)
        pGroup->Add(pTrainTrack);

    return pTrainTrack;
}
#endif
