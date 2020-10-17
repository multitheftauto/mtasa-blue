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

void CLuaTrainTrackDefs::LoadFunctions()
{
    CLuaCFunctions::AddFunction("getDefaultTrack", ArgumentParser<GetDefaultTrack>);

    CLuaCFunctions::AddFunction("getTrackNodeCount", ArgumentParser<GetTrackNodeCount>);
    CLuaCFunctions::AddFunction("getTrackNodePosition", ArgumentParser<GetTrackNodePosition>);

#ifdef MTA_CLIENT
    CLuaCFunctions::AddFunction("getTrackLength", GetTrackLength);
#else
    CLuaCFunctions::AddFunction("createTrack", CreateTrack);
    CLuaCFunctions::AddFunction("setTrackNodePosition", SetTrackNodePosition);
#endif
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getDefault", "getDefaultTrack");


#ifdef MTA_CLIENT
    lua_classfunction(luaVM, "getNodeCount", "getTrackNodeCount");
    lua_classfunction(luaVM, "getNodePosition", "getTrackNodePosition");
    lua_classfunction(luaVM, "getLength", "getTrackLength");

    lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

    lua_classvariable(luaVM, "length", nullptr, "getTrackLength");
#else

    lua_classfunction(luaVM, "create", "createTrack");
    lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

    lua_classfunction(luaVM, "getNodePosition", "getTrackNodePosition");
    lua_classfunction(luaVM, "setNodePosition", "setTrackNodePosition");

    lua_classfunction(luaVM, "getNodeCount", "getTrackNodeCount");
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
