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
};
