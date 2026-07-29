/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaTrainTrackDefs.h"
#include <lua/CLuaFunctionParser.h>

#ifndef MTA_CLIENT
    #include "CTrainTrackManager.h"
    #include "CGame.h"
    #include "CDummy.h"
    #include "common/CBitStream.h"
    #include "packets/CElementRPCPacket.h"
    #include "packets/CEntityAddPacket.h"
#endif

void CLuaTrainTrackDefs::LoadFunctions()
{
    CLuaCFunctions::AddFunction("getDefaultTrack", ArgumentParser<GetDefaultTrack>);

#ifndef MTA_CLIENT
    CLuaCFunctions::AddFunction("createTrainTrack", ArgumentParser<CreateTrainTrack>);
    CLuaCFunctions::AddFunction("getTrainTrackNodeCount", ArgumentParser<GetTrainTrackNodeCount>);
    CLuaCFunctions::AddFunction("getTrainTrackNodePosition", ArgumentParser<GetTrainTrackNodePosition>);
    CLuaCFunctions::AddFunction("setTrainTrackNodePosition", ArgumentParser<SetTrainTrackNodePosition>);
#endif
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

#ifndef MTA_CLIENT
    lua_classfunction(luaVM, "create", "createTrainTrack");
    lua_classfunction(luaVM, "getNodeCount", "getTrainTrackNodeCount");
    lua_classfunction(luaVM, "getNodePosition", "getTrainTrackNodePosition");
    lua_classfunction(luaVM, "setNodePosition", "setTrainTrackNodePosition");
#endif

    lua_registerclass(luaVM, "TrainTrack", "Element");
}

CLuaTrainTrackDefs::TrainTrack CLuaTrainTrackDefs::GetDefaultTrack(uchar trackID)
{
    if (trackID > 3)
        throw std::invalid_argument("Bad default track ID (0-3)");

#ifdef MTA_CLIENT
    return trackID;
#else
    return g_pGame->GetTrainTrackManager()->GetDefaultTrackByIndex(trackID);
#endif
}

#ifndef MTA_CLIENT
CLuaTrainTrackDefs::TrainTrack CLuaTrainTrackDefs::CreateTrainTrack(lua_State* luaVM, std::vector<CVector> nodePositions, std::optional<bool> linkLastNodes)
{
    if (nodePositions.empty())
        throw std::invalid_argument("createTrainTrack needs at least one node");

    std::vector<STrackNode> nodes(nodePositions.begin(), nodePositions.end());

    CResource&   resource = lua_getownerresource(luaVM);
    CTrainTrack* pTrainTrack = g_pGame->GetTrainTrackManager()->CreateTrainTrack(nodes, linkLastNodes.value_or(false), resource.GetDynamicElementRoot());
    if (!pTrainTrack)
        return nullptr;

    if (CElementGroup* elementGroup = resource.GetElementGroup())
        elementGroup->Add(pTrainTrack);

    // Clients need the node data before any train can be put on this track: setTrainTrack refers to
    // the track by element, and a client without that element cannot move a train onto it
    if (resource.IsClientSynced())
    {
        CEntityAddPacket packet;
        packet.Add(pTrainTrack);
        m_pPlayerManager->BroadcastOnlyJoined(packet);
    }

    return pTrainTrack;
}

uint CLuaTrainTrackDefs::GetTrainTrackNodeCount(CTrainTrack* pTrainTrack)
{
    return static_cast<uint>(pTrainTrack->GetNumberOfNodes());
}

std::variant<CVector, bool> CLuaTrainTrackDefs::GetTrainTrackNodePosition(CTrainTrack* pTrainTrack, uint nodeIndex)
{
    CVector position;
    if (!pTrainTrack->GetTrackNodePosition(nodeIndex, position))
        return false;

    return position;
}

bool CLuaTrainTrackDefs::SetTrainTrackNodePosition(CTrainTrack* pTrainTrack, uint nodeIndex, CVector position)
{
    if (!pTrainTrack->SetTrackNodePosition(nodeIndex, position))
        return false;

    // Tell clients that already have this track so they don't drift out of sync with it
    CBitStream BitStream;
    BitStream.pBitStream->Write(nodeIndex);
    BitStream.pBitStream->Write(position.fX);
    BitStream.pBitStream->Write(position.fY);
    BitStream.pBitStream->Write(position.fZ);

    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(pTrainTrack, SET_TRAIN_TRACK_NODE_POSITION, *BitStream.pBitStream));

    return true;
}
#endif
