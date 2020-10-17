/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "luadefs/CLuaDefs.h"

class CLuaTrainTrackDefs : public CLuaDefs
{
public:
#ifdef MTA_CLIENT
    using TrainTrack = CClientTrainTrack*;
    using TrainTrackManager = CClientTrainTrackManager*;
#else
    using TrainTrack = CTrainTrack*;
    using TrainTrackManager = shared_ptr<CTrainTrackManager>;
#endif

    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static TrainTrack GetDefaultTrack(uchar trackID);
    static int        GetTrackNodeCount(TrainTrack track);

    static std::tuple<float, float, float> GetTrackNodePosition(TrainTrack track, uint nodeIndex);

    static bool SetTrackNodePosition(TrainTrack track, uint nodeIndex, CVector position);

#ifdef MTA_CLIENT
    static float GetTrackLength(TrainTrack track);
#else
    static TrainTrack CreateTrack(lua_State* L, std::vector<CVector> nodes, bool linkNodes);
#endif

private:
    static CResource*        GetResource(lua_State* L);
    static TrainTrackManager GetManager();
};
