#include "StdInc.h"
#include "CAPI.h"
#include "CGame.h"
#include "CMainConfig.h"
#include <SharedUtil.String.h>

namespace Endpoints
{
    inline const std::string API_ENDPOINT = "/api";
    namespace Server
    {
        inline const std::string PLAYERS = "/players";
        inline const std::string MAX_PLAYERS = PLAYERS + "/max";
    }
}

const bool GetNextPathFragment(std::string& path) noexcept
{
    auto index = path.find('/', 1);
    if (index == path.npos)
        return false;

    path = path.substr(index);
    return true;
};

ResponseCode CAPI::HandleRequest(HttpRequest* request, HttpResponse* response)
{
    response->oResponseHeaders["content-type"] = "application/json";

    std::string uri = request->sOriginalUri;    

    if (!SharedUtil::StartsWith(uri, Endpoints::API_ENDPOINT))
        return HTTPRESPONSECODE_404_NOTFOUND;

    if (!GetNextPathFragment(uri))
        return HTTPRESPONSECODE_404_NOTFOUND;

    if (SharedUtil::StartsWith(uri, Endpoints::Server::PLAYERS))
    {
        return HandlePlayers(request, response);
    }

    std::string body("{\"error\": \"404: Not Found\"}");
    response->SetBody(body.c_str(), body.size());
    return HTTPRESPONSECODE_404_NOTFOUND;
}

ResponseCode CAPI::HandlePlayers(HttpRequest* request, HttpResponse* response)
{
    std::string uri = request->sOriginalUri;
    // skip /api/server
    if (!GetNextPathFragment(uri) && !GetNextPathFragment(uri))
    {
        std::string body("{\"error\": \"404: Not Found\"}");
        response->SetBody(body.c_str(), body.size());
        return HTTPRESPONSECODE_404_NOTFOUND;
    }

    if (!GetNextPathFragment(uri))
    {
        // /players

        auto players = g_pGame->GetPlayerManager()->GetPlayers();
        SString body("{\"players\": %llu}", players.size());
        response->SetBody(body.c_str(), body.size());
        return HTTPRESPONSECODE_200_OK;
    }
    if (uri == Endpoints::Server::MAX_PLAYERS)
    {
        // players/max

        SString body("{\"max\": %llu}", g_pGame->GetConfig()->GetMaxPlayers());
        response->SetBody(body.c_str(), body.size());
        return HTTPRESPONSECODE_200_OK;
    }

    std::string body("{\"error\": \"404: Not Found\"}");
    response->SetBody(body.c_str(), body.size());
    return HTTPRESPONSECODE_404_NOTFOUND;
}
