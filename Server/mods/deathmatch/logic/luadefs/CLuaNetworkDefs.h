/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaNetworkDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    LUA_DECLARE(CallRemote);
    LUA_DECLARE(FetchRemote);
    LUA_DECLARE(GetRemoteRequests);
    LUA_DECLARE(GetRemoteRequestInfo);
    LUA_DECLARE(AbortRemoteRequest);
};
