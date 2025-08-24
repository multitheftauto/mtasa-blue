/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaNetworkDefs.h"
#include "CRemoteCalls.h"
#include "CScriptArgReader.h"
#include "lua/LuaCommon.h"
#include "lua/CLuaFunctionParser.h"

#define MIN_SERVER_REQ_CALLREMOTE_QUEUE_NAME          "1.5.3-9.11270"
#define MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS "1.3.0-9.04563"
#define MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT     "1.3.5"
#define MIN_SERVER_REQ_CALLREMOTE_OPTIONS_TABLE       "1.5.4-9.11342"
#define MIN_SERVER_REQ_CALLREMOTE_OPTIONS_FORMFIELDS  "1.5.4-9.11413"

void CLuaNetworkDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"callRemote", CallRemote},
        {"fetchRemote", FetchRemote},
        {"getRemoteRequests", GetRemoteRequests},
        {"getRemoteRequestInfo", GetRemoteRequestInfo},
        {"abortRemoteRequest", AbortRemoteRequest},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

// Call a function on a remote server
int CLuaNetworkDefs::CallRemote(lua_State* luaVM)
{
    /* Determine if the argument stream is either a remote-server resource call or a web call:
     * a) element callRemote ( string host [, string queueName ][, int connectionAttempts = 10, int connectTimeout = 10000 ], string resourceName, string
     * functionName, callback callbackFunction, [ arguments... ] ) b) bool callRemote ( string URL [, string queueName ][, int connectionAttempts = 10, int
     * connectTimeout = 10000 ], callback callbackFunction, [ arguments... ] )
     */
    CScriptArgReader argStream(luaVM);
    SString          strHost;
    SString          strQueueName = CALL_REMOTE_DEFAULT_QUEUE_NAME;

    argStream.ReadString(strHost);

    /* Find out if the next parameter is the 'queueName' argument
     * 1) string queueName, int connectionAttempts, ...
     * 2) string queueName, callback callbackFunction, ...
     * 3) string queueName, string resourceName, string functionName, ...
     */
    if (argStream.NextIsString() && (argStream.NextIsNumber(1) || argStream.NextIsFunction(1) || (argStream.NextIsString(1) && argStream.NextIsString(2))))
    {
        MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_QUEUE_NAME, "'queue name' is being used");
        argStream.ReadString(strQueueName, CALL_REMOTE_DEFAULT_QUEUE_NAME);
    }

    // Read connectionAttempts and connectTimeout arguments if given
    uint uiConnectionAttempts = 10U;
    uint uiConnectTimeoutMs = 10000U;

    if (argStream.NextIsNumber())
        MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS, "'connection attempts' is being used");
    argStream.ReadIfNextIsNumber(uiConnectionAttempts, 10U);

    if (argStream.NextIsNumber())
        MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT, "'connect timeout' is being used");
    argStream.ReadIfNextIsNumber(uiConnectTimeoutMs, 10000U);

    // Continue with either call type a) or b)
    CLuaFunctionRef iLuaFunction;
    CLuaArguments   args;

    if (argStream.NextIsString() && argStream.NextIsString(1) && argStream.NextIsFunction(2))
    {
        SString strResourceName;
        SString strFunctionName;
        argStream.ReadString(strResourceName);
        argStream.ReadString(strFunctionName);
        argStream.ReadFunction(iLuaFunction);
        argStream.ReadLuaArguments(args);
        argStream.ReadFunctionComplete();

        if (!argStream.HasErrors())
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (luaMain)
            {
                CRemoteCall* pRemoteCall = g_pGame->GetRemoteCalls()->Call(strHost, strResourceName, strFunctionName, &args, luaMain, iLuaFunction,
                                                                           strQueueName, uiConnectionAttempts, uiConnectTimeoutMs);

                lua_pushuserdata(luaVM, pRemoteCall);
                return 1;
            }
        }
    }
    else if (argStream.NextIsFunction())
    {
        argStream.ReadFunction(iLuaFunction);
        argStream.ReadLuaArguments(args);
        argStream.ReadFunctionComplete();

        if (!argStream.HasErrors())
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (luaMain)
            {
                CRemoteCall* pRemoteCall =
                    g_pGame->GetRemoteCalls()->Call(strHost, &args, luaMain, iLuaFunction, strQueueName, uiConnectionAttempts, uiConnectTimeoutMs);

                lua_pushuserdata(luaVM, pRemoteCall);
                return 1;
            }
        }
    }
    else
    {
        argStream.SetCustomWarning("Unrecognized argument list for callRemote: bad arguments or missing arguments");
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

// Call a function on a remote server
int CLuaNetworkDefs::FetchRemote(lua_State* luaVM)
{
    //  element fetchRemote ( string URL [, string queueName ][, int connectionAttempts = 10, int connectTimeout = 10000 ], callback callbackFunction, [ string
    //  postData, bool bPostBinary, arguments... ] ) bool fetchRemote ( string URL [, table options ], callback callbackFunction[, table callbackArguments ] )
    CScriptArgReader    argStream(luaVM);
    SString             strURL;
    SString             strQueueName;
    SHttpRequestOptions httpRequestOptions;
    CLuaFunctionRef     iLuaFunction;
    CLuaArguments       callbackArguments;

    argStream.ReadString(strURL);
    if (!argStream.NextIsTable())
    {
        if (argStream.NextIsString())
            MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_QUEUE_NAME, "'queue name' is being used");
        argStream.ReadIfNextIsString(strQueueName, CALL_REMOTE_DEFAULT_QUEUE_NAME);
        if (argStream.NextIsNumber())
            MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS, "'connection attempts' is being used");
        argStream.ReadIfNextIsNumber(httpRequestOptions.uiConnectionAttempts, 10);
        if (argStream.NextIsNumber())
            MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT, "'connect timeout' is being used");
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
                CRemoteCall* pRemoteCall = g_pGame->GetRemoteCalls()->Call(strURL, &callbackArguments, luaMain, iLuaFunction, strQueueName, httpRequestOptions);

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
            MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_OPTIONS_TABLE, "'options' table is being used");
        else
            MinServerReqCheck(argStream, MIN_SERVER_REQ_CALLREMOTE_OPTIONS_FORMFIELDS, "'formFields' is being used");

        if (!argStream.HasErrors())
        {
            CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (luaMain)
            {
                CRemoteCall* pRemoteCall = g_pGame->GetRemoteCalls()->Call(strURL, &callbackArguments, luaMain, iLuaFunction, strQueueName, httpRequestOptions);

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
int CLuaNetworkDefs::GetRemoteRequests(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CResource*       pResource = nullptr;
    CLuaMain*        pLuaMain = nullptr;
    int              iIndex = 0;

    argStream.ReadUserData(pResource, NULL);

    if (pResource)
        pLuaMain = pResource->GetVirtualMachine();

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);

        for (const auto& request : g_pGame->GetRemoteCalls()->GetCalls())
        {
            if (!pResource || request->GetVM() == pLuaMain)
            {
                lua_pushnumber(luaVM, ++iIndex);
                lua_pushuserdata(luaVM, request);
                lua_settable(luaVM, -3);
            }
        }

        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

// table getRemoteRequestInfo(element theRequest[, number postDataLength = 0[, bool includeHeaders = false]])
int CLuaNetworkDefs::GetRemoteRequestInfo(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaArguments    info, requestedHeaders;
    CRemoteCall*     pRemoteCall = nullptr;
    CResource*       pThisResource = m_pResourceManager->GetResourceFromLuaState(luaVM);
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

        bool bExtendedInfo = false;

        // only extend informations when the called resource is the same OR has "general.fullRemoteRequestInfo" acl right
        if (pThisResource == pResource ||
            m_pACLManager->CanObjectUseRight(pThisResource->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, "fullRemoteRequestInfo",
                                             CAccessControlListRight::RIGHT_TYPE_GENERAL, false))
        {
            bExtendedInfo = true;
        }

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
        SDownloadStatus downloadInfo = pRemoteCall->GetDownloadStatus();

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

int CLuaNetworkDefs::AbortRemoteRequest(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CRemoteCall*     pRemoteCall = nullptr;

    argStream.ReadUserData(pRemoteCall);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pRemoteCall->CancelDownload());
        g_pGame->GetRemoteCalls()->Remove(pRemoteCall);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}
