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

    static std::tuple<float, float, float> CLuaTrainTrackDefs::GetTrackNodePosition(TrainTrack track, uint nodeIndex);

#ifdef MTA_CLIENT
    LUA_DECLARE(GetTrackLength);
#else
    LUA_DECLARE(CreateTrack);
    LUA_DECLARE(SetTrackNodePosition);
#endif

private:
    static TrainTrackManager GetManager();
};
