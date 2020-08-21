/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"

#define MIN_CLIENT_REQ_CALLREMOTE_QUEUE_NAME                "1.5.3-9.11270"
#define MIN_CLIENT_REQ_FETCHREMOTE_CONNECT_TIMEOUT          "1.3.5"
#define MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_TABLE             "1.5.4-9.11342"
#define MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_FORMFIELDS        "1.5.4-9.11413"

CLuaManager*               CLuaFunctionDefs::m_pLuaManager;
CScriptDebugging*          CLuaFunctionDefs::m_pScriptDebugging;
CClientGame*               CLuaFunctionDefs::m_pClientGame;
CClientManager*            CLuaFunctionDefs::m_pManager;
CClientEntity*             CLuaFunctionDefs::m_pRootEntity;
CClientGUIManager*         CLuaFunctionDefs::m_pGUIManager;
CClientPedManager*         CLuaFunctionDefs::m_pPedManager;
CClientPlayerManager*      CLuaFunctionDefs::m_pPlayerManager;
CClientRadarMarkerManager* CLuaFunctionDefs::m_pRadarMarkerManager;
CResourceManager*          CLuaFunctionDefs::m_pResourceManager;
CClientVehicleManager*     CLuaFunctionDefs::m_pVehicleManager;
CClientColManager*         CLuaFunctionDefs::m_pColManager;
CClientObjectManager*      CLuaFunctionDefs::m_pObjectManager;
CClientTeamManager*        CLuaFunctionDefs::m_pTeamManager;
CRenderWare*               CLuaFunctionDefs::m_pRenderWare;
CClientMarkerManager*      CLuaFunctionDefs::m_pMarkerManager;
CClientPickupManager*      CLuaFunctionDefs::m_pPickupManager;
CClientDFFManager*         CLuaFunctionDefs::m_pDFFManager;
CClientColModelManager*    CLuaFunctionDefs::m_pColModelManager;
CRegisteredCommands*       CLuaFunctionDefs::m_pRegisteredCommands;

void CLuaFunctionDefs::Initialize(CLuaManager* pLuaManager, CScriptDebugging* pScriptDebugging, CClientGame* pClientGame)
{
    m_pLuaManager = pLuaManager;
    m_pScriptDebugging = pScriptDebugging;
    m_pClientGame = pClientGame;
    m_pManager = pClientGame->GetManager();
    m_pRootEntity = m_pClientGame->GetRootEntity();
    m_pGUIManager = m_pClientGame->GetGUIManager();
    m_pPedManager = m_pClientGame->GetPedManager();
    m_pPlayerManager = m_pClientGame->GetPlayerManager();
    m_pRadarMarkerManager = m_pClientGame->GetRadarMarkerManager();
    m_pResourceManager = m_pClientGame->GetResourceManager();
    m_pColManager = m_pManager->GetColManager();
    m_pVehicleManager = m_pManager->GetVehicleManager();
    m_pObjectManager = m_pManager->GetObjectManager();
    m_pTeamManager = m_pManager->GetTeamManager();
    m_pRenderWare = g_pGame->GetRenderWare();
    m_pMarkerManager = m_pManager->GetMarkerManager();
    m_pPickupManager = m_pManager->GetPickupManager();
    m_pDFFManager = m_pManager->GetDFFManager();
    m_pColModelManager = m_pManager->GetColModelManager();
    m_pRegisteredCommands = m_pClientGame->GetRegisteredCommands();
}

// Call a function on a remote server
int CLuaFunctionDefs::FetchRemote(lua_State* luaVM)
{
    //  bool fetchRemote ( string URL [, string queueName ][, int connectionAttempts = 10, int connectTimeout = 10000 ], callback callbackFunction, [ string
    //  postData, bool bPostBinary, arguments... ] ) bool fetchRemote ( string URL [, table options ], callback callbackFunction[, table callbackArguments ] )
    CScriptArgReader    argStream(luaVM);
    SString             strURL;
    SHttpRequestOptions httpRequestOptions;
    SString             strQueueName;
    CLuaFunctionRef     iLuaFunction;
    CLuaArguments       callbackArguments;

    argStream.ReadString(strURL);
    if (!argStream.NextIsTable())
    {
        if (argStream.NextIsString())
            MinClientReqCheck(argStream, MIN_CLIENT_REQ_CALLREMOTE_QUEUE_NAME, "'queue name' is being used");
        argStream.ReadIfNextIsString(strQueueName, CALL_REMOTE_DEFAULT_QUEUE_NAME);
        argStream.ReadIfNextIsNumber(httpRequestOptions.uiConnectionAttempts, 10);
        if (argStream.NextIsNumber())
            MinClientReqCheck(argStream, MIN_CLIENT_REQ_FETCHREMOTE_CONNECT_TIMEOUT, "'connect timeout' is being used");
        argStream.ReadIfNextIsNumber(httpRequestOptions.uiConnectTimeoutMs, 10000);
        argStream.ReadFunction(iLuaFunction);
        argStream.ReadString(httpRequestOptions.strPostData, "");
        argStream.ReadBool(httpRequestOptions.bPostBinary, false);
        argStream.ReadLuaArguments(callbackArguments);
        argStream.ReadFunctionComplete();

        if (!argStream.HasErrors())
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (luaMain)
            {
                httpRequestOptions.bIsLegacy = true;
                CRemoteCall* pRemoteCall = g_pClientGame->GetRemoteCalls()->Call(strURL, &callbackArguments, luaMain, iLuaFunction, strQueueName, httpRequestOptions);

                lua_pushuserdata(luaVM, pRemoteCall);
                return 1;
            }
        }
    }
    else
    {
        CStringMap optionsMap;

        argStream.ReadStringMap(optionsMap);
        argStream.ReadFunction(iLuaFunction);
        if (argStream.NextIsTable())
            argStream.ReadLuaArgumentsTable(callbackArguments);
        argStream.ReadFunctionComplete();

        optionsMap.ReadNumber("connectionAttempts", httpRequestOptions.uiConnectionAttempts, 10);
        optionsMap.ReadNumber("connectTimeout", httpRequestOptions.uiConnectTimeoutMs, 10000);
        optionsMap.ReadString("method", httpRequestOptions.strRequestMethod, "");
        optionsMap.ReadString("queueName", strQueueName, CALL_REMOTE_DEFAULT_QUEUE_NAME);
        optionsMap.ReadString("postData", httpRequestOptions.strPostData, "");
        optionsMap.ReadBool("postIsBinary", httpRequestOptions.bPostBinary, false);
        optionsMap.ReadNumber("maxRedirects", httpRequestOptions.uiMaxRedirects, 8);
        optionsMap.ReadString("username", httpRequestOptions.strUsername, "");
        optionsMap.ReadString("password", httpRequestOptions.strPassword, "");
        optionsMap.ReadStringMap("headers", httpRequestOptions.requestHeaders);
        optionsMap.ReadStringMap("formFields", httpRequestOptions.formFields);

        if (httpRequestOptions.formFields.empty())
            MinClientReqCheck(argStream, MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_TABLE, "'options' table is being used");
        else
            MinClientReqCheck(argStream, MIN_CLIENT_REQ_CALLREMOTE_OPTIONS_FORMFIELDS, "'formFields' is being used");

        if (!argStream.HasErrors())
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (luaMain)
            {
                CRemoteCall* pRemoteCall = g_pClientGame->GetRemoteCalls()->Call(strURL, &callbackArguments, luaMain, iLuaFunction, strQueueName, httpRequestOptions);

                lua_pushuserdata(luaVM, pRemoteCall);
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

// table getRemoteRequests([resource theResource = nil])
int CLuaFunctionDefs::GetRemoteRequests(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CResource* pResource = nullptr;
    CLuaMain* pLuaMain = nullptr;
    int              iIndex = 0;

    argStream.ReadUserData(pResource, NULL);

    // Grab virtual machine
    if (pResource)
        pLuaMain = pResource->GetVM();

    lua_newtable(luaVM);
    for (const auto& request : g_pClientGame->GetRemoteCalls()->GetCalls())
    {
        if (!pLuaMain || request->GetVM() == pLuaMain)
        {
            lua_pushnumber(luaVM, ++iIndex);
            lua_pushuserdata(luaVM, request);
            lua_settable(luaVM, -3);
        }
    }

    return 1;
}

// table getRemoteRequestInfo(element theRequest[, number postDataLength = 0[, bool includeHeaders = false]])
int CLuaFunctionDefs::GetRemoteRequestInfo(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaArguments    info, requestedHeaders;
    CRemoteCall* pRemoteCall = nullptr;
    CResource* pThisResource = g_pClientGame->GetResourceManager()->GetResourceFromLuaState(luaVM);
    int              iPostDataLength = 0;
    bool             bIncludeHeaders = false;

    argStream.ReadUserData(pRemoteCall);
    argStream.ReadNumber(iPostDataLength, 0);
    argStream.ReadBool(bIncludeHeaders, false);

    if (!argStream.HasErrors())
    {
        CResource* pResource = nullptr;
        if (pRemoteCall->GetVM())
            pResource = pRemoteCall->GetVM()->GetResource();

        bool bExtendedInfo = (pResource == pThisResource);

        info.PushString("type");
        info.PushString((pRemoteCall->IsFetch() ? "fetch" : "call"));

        // remove query_string from url when bExtendedInfo isn't set
        SString sURL = pRemoteCall->GetURL();

        if (!bExtendedInfo)
            sURL = sURL.ReplaceI("%3F", "?").Replace("#", "?").SplitLeft("?");

        info.PushString("url");
        info.PushString(sURL);

        info.PushString("queue");
        info.PushString(pRemoteCall->GetQueueName());

        info.PushString("resource");

        if (pResource)
            info.PushResource(pResource);
        else
            info.PushBoolean(false);

        info.PushString("start");
        info.PushNumber(static_cast<double>(pRemoteCall->GetStartTime()));

        if (bExtendedInfo)
        {
            if (iPostDataLength == -1 || iPostDataLength > 0)
            {
                info.PushString("postData");
                const SString& sPostData = pRemoteCall->GetOptions().strPostData;
                if (iPostDataLength > 0 && iPostDataLength < static_cast<int>(sPostData.length()))
                    info.PushString(sPostData.SubStr(0, iPostDataLength));
                else
                    info.PushString(sPostData);
            }

            // requested headers
            if (bIncludeHeaders)
            {
                info.PushString("headers");

                for (auto const& header : pRemoteCall->GetOptions().requestHeaders)
                {
                    requestedHeaders.PushString(header.first);
                    requestedHeaders.PushString(header.second);
                }

                info.PushTable(&requestedHeaders);
            }
        }

        info.PushString("method");
        info.PushString((pRemoteCall->GetOptions().strRequestMethod.length() >= 1 ? pRemoteCall->GetOptions().strRequestMethod.ToUpper().c_str() : "POST"));

        info.PushString("connectionAttempts");
        info.PushNumber(pRemoteCall->GetOptions().uiConnectionAttempts);

        info.PushString("connectionTimeout");
        info.PushNumber(pRemoteCall->GetOptions().uiConnectTimeoutMs);

        // download info
        const SDownloadStatus downloadInfo = pRemoteCall->GetDownloadStatus();

        info.PushString("bytesReceived");
        info.PushNumber(downloadInfo.uiBytesReceived);

        info.PushString("bytesTotal");
        info.PushNumber(downloadInfo.uiContentLength);

        info.PushString("currentAttempt");
        info.PushNumber(downloadInfo.uiAttemptNumber);

        info.PushAsTable(luaVM);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

// bool abortRemoteRequest(element theRequest)
int CLuaFunctionDefs::AbortRemoteRequest(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CRemoteCall* pRemoteCall = nullptr;

    argStream.ReadUserData(pRemoteCall);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pRemoteCall->CancelDownload());
        g_pClientGame->GetRemoteCalls()->Remove(pRemoteCall);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}
