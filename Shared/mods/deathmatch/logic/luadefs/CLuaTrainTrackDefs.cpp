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

void CLuaTrainTrackDefs::LoadFunctions()
{
    // CLuaCFunctions::AddFunction("getDefaultTrack", ArgumentParser<GetDefaultTrack>);
}

void CLuaTrainTrackDefs::AddClass(lua_State* luaVM)
{
    // lua_newclass(luaVM);

    // lua_classfunction(luaVM, "getDefault", "getDefaultTrack");

    // lua_registerclass(luaVM, "TrainTrack", "Element");
}

auto CLuaTrainTrackDefs::GetDefaultTrack(uchar trackID) -> CLuaTrainTrackDefs::TrainTrack
{
    if (trackID > 3)
        throw std::invalid_argument("Bad default track ID (0-3)");

#ifdef MTA_CLIENT
    return trackID;
#else
    return g_pGame->GetTrainTrackManager()->GetTrainTrackByIndex(trackID);
#endif
}
