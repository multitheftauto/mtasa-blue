/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAPI.h
 *  PURPOSE:     API interface for HTTP
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <ehs/ehs.h>

class CAPI : public EHS {
public:
    ResponseCode HandleRequest(HttpRequest* request, HttpResponse* response) override;

private:
    ResponseCode HandlePlayers(HttpRequest* request, HttpResponse* response);
};
