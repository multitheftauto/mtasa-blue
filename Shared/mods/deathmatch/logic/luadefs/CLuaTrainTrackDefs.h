/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "luadefs/CLuaDefs.h"

class CLuaTrainTrackDefs : public CLuaDefs
{
public:
#ifdef MTA_CLIENT
    using TrainTrack = uchar;
#else
    using TrainTrack = CTrainTrack*;
#endif

    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static TrainTrack GetDefaultTrack(uchar trackID);

#ifndef MTA_CLIENT
    // Track creation isn't wired up client-side yet, so keep it server-only for now
    static TrainTrack                  CreateTrainTrack(lua_State* luaVM, std::vector<CVector> nodePositions, std::optional<bool> linkLastNodes);
    static uint                        GetTrainTrackNodeCount(CTrainTrack* pTrainTrack);
    static std::variant<CVector, bool> GetTrainTrackNodePosition(CTrainTrack* pTrainTrack, uint nodeIndex);
    static bool                        SetTrainTrackNodePosition(CTrainTrack* pTrainTrack, uint nodeIndex, CVector position);
#endif
};
