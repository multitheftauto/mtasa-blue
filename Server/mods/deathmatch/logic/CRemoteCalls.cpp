/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRemoteCalls.cpp
 *  PURPOSE:     Remote HTTP call (callRemote) class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRemoteCalls.h"
#include "CGame.h"

CRemoteCalls::CRemoteCalls()
{
}

CRemoteCalls::~CRemoteCalls()
{
    for (auto pRemoteCall : m_calls)
    {
        delete pRemoteCall;
    }

    m_calls.clear();
}

CRemoteCall* CRemoteCalls::Call(const char* szServerHost, const char* szResourceName, const char* szFunctionName, CLuaArguments* arguments, CLuaMain* luaMain,
                                const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs)
{
    CRemoteCall* pRemoteCall =
        new CRemoteCall(szServerHost, szResourceName, szFunctionName, arguments, luaMain, iFunction, strQueueName, uiConnectionAttempts, uiConnectTimeoutMs);

    m_calls.push_back(pRemoteCall);
    m_calls.back()->MakeCall();

    return pRemoteCall;
}

CRemoteCall* CRemoteCalls::Call(const char* szURL, CLuaArguments* arguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName,
                                uint uiConnectionAttempts, uint uiConnectTimeoutMs)
{
    CRemoteCall* pRemoteCall = new CRemoteCall(szURL, arguments, luaMain, iFunction, strQueueName, uiConnectionAttempts, uiConnectTimeoutMs);

    m_calls.push_back(pRemoteCall);
    m_calls.back()->MakeCall();

    return pRemoteCall;
}

CRemoteCall* CRemoteCalls::Call(const char* szURL, CLuaArguments* fetchArguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction,
                                const SString& strQueueName, const SHttpRequestOptions& options)
{
    CRemoteCall* pRemoteCall = new CRemoteCall(szURL, fetchArguments, luaMain, iFunction, strQueueName, options);

    m_calls.push_back(pRemoteCall);
    m_calls.back()->MakeCall();

    return pRemoteCall;
}

void CRemoteCalls::OnLuaMainDestroy(CLuaMain* lua)
{
    for (auto pRemoteCall : m_calls)
    {
        if (pRemoteCall->GetVM() == lua)
        {
            pRemoteCall->OnLuaMainDestroy();
        }
    }
}

void CRemoteCalls::Remove(CRemoteCall* call)
{
    m_calls.remove(call);
    delete call;
}

bool CRemoteCalls::CallExists(CRemoteCall* call)
{
    return ListContains(m_calls, call);
}

// Map queue index into download manager id
EDownloadModeType CRemoteCalls::GetDownloadModeFromQueueIndex(uint uiIndex)
{
    uiIndex %= MAX_CALL_REMOTE_QUEUES;
    uiIndex += EDownloadMode::CALL_REMOTE;
    return (EDownloadModeType)uiIndex;
}

// Map queue name to download manager id
EDownloadModeType CRemoteCalls::GetDownloadModeForQueueName(const SString& strQueueName)
{
    uint* pIndex = MapFind(m_QueueIndexMap, strQueueName);
    if (pIndex)
    {
        return GetDownloadModeFromQueueIndex(*pIndex);
    }
    else
    {
        // Find lowest unused index
        uint idx = 0;
        while (MapContainsValue(m_QueueIndexMap, idx))
        {
            idx++;
        }
        // Add new mapping
        MapSet(m_QueueIndexMap, strQueueName, idx);
        return GetDownloadModeFromQueueIndex(idx);
    }
}

void CRemoteCalls::ProcessQueuedFiles()
{
    for (auto iter = m_QueueIndexMap.cbegin(); iter != m_QueueIndexMap.cend();)
    {
        EDownloadModeType downloadMode = GetDownloadModeFromQueueIndex(iter->second);
        if (g_pNetServer->GetHTTPDownloadManager(downloadMode)->ProcessQueuedFiles())
        {
            // Queue empty, so remove name mapping if not default queue
            if (iter->first != CALL_REMOTE_DEFAULT_QUEUE_NAME)
            {
                iter = m_QueueIndexMap.erase(iter);
                continue;
            }
        }
        ++iter;
    }

    // Maybe abort downloads from resources which have been stopped/restarted
    for (auto pRemoteCall : m_calls)
    {
        // Check remote call is no longer associated with a resource
        if (pRemoteCall->GetVM() == nullptr)
        {
            const SDownloadStatus& status = pRemoteCall->GetDownloadStatus();
            if (status.uiBytesReceived > 50000)
            {
                // Only abort after some data has been received so remote HTTP server has probably processed original request
                pRemoteCall->CancelDownload();
                Remove(pRemoteCall);
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

CRemoteCall::CRemoteCall(const char* szServerHost, const char* szResourceName, const char* szFunctionName, CLuaArguments* arguments, CLuaMain* luaMain,
                         const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs)
{
    m_VM = luaMain;
    m_iFunction = iFunction;

    arguments->WriteToJSONString(m_options.strPostData, true);
    m_options.bPostBinary = false;
    m_options.bIsLegacy = true;
    m_options.requestHeaders["Content-Type"] = "application/json";
    m_bIsFetch = false;

    m_strURL = SString("http://%s/%s/call/%s", szServerHost, szResourceName, szFunctionName);
    m_strQueueName = strQueueName;
    m_options.uiConnectionAttempts = uiConnectionAttempts;
    m_options.uiConnectTimeoutMs = uiConnectTimeoutMs;
}

// arbitary URL version
CRemoteCall::CRemoteCall(const char* szURL, CLuaArguments* arguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName,
                         uint uiConnectionAttempts, uint uiConnectTimeoutMs)
{
    m_VM = luaMain;
    m_iFunction = iFunction;

    arguments->WriteToJSONString(m_options.strPostData, true);
    m_options.bPostBinary = false;
    m_options.bIsLegacy = true;
    m_options.requestHeaders["Content-Type"] = "application/json";
    m_bIsFetch = false;

    m_strURL = szURL;
    m_strQueueName = strQueueName;
    m_options.uiConnectionAttempts = uiConnectionAttempts;
    m_options.uiConnectTimeoutMs = uiConnectTimeoutMs;
}

// Fetch version
CRemoteCall::CRemoteCall(const char* szURL, CLuaArguments* fetchArguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName,
                         const SHttpRequestOptions& options)
    : m_FetchArguments(*fetchArguments)
{
    m_VM = luaMain;
    m_iFunction = iFunction;
    m_bIsFetch = true;
    m_strURL = szURL;
    m_strQueueName = strQueueName;
    m_options = options;
}

CRemoteCall::~CRemoteCall()
{
}

void CRemoteCall::MakeCall()
{
    m_iStartTime = GetTickCount64_();

    m_downloadMode = g_pGame->GetRemoteCalls()->GetDownloadModeForQueueName(m_strQueueName);
    CNetHTTPDownloadManagerInterface* pDownloadManager = g_pNetServer->GetHTTPDownloadManager(m_downloadMode);
    pDownloadManager->QueueFile(m_strURL, NULL, this, DownloadFinishedCallback, m_options);
}

void CRemoteCall::DownloadFinishedCallback(const SHttpDownloadResult& result)
{
    CRemoteCall* pCall = (CRemoteCall*)result.pObj;
    if (!g_pGame->GetRemoteCalls()->CallExists(pCall))
        return;

    // Save final download status
    pCall->m_lastDownloadStatus.uiAttemptNumber = result.uiAttemptNumber;
    pCall->m_lastDownloadStatus.uiContentLength = result.uiContentLength;
    pCall->m_lastDownloadStatus.uiBytesReceived = result.dataSize;
    pCall->m_downloadMode = EDownloadModeType::NONE;

    CLuaArguments arguments;
    if (pCall->IsLegacy())
    {
        if (result.bSuccess)
        {
            if (pCall->IsFetch())
                arguments.PushString(std::string(result.pData, result.dataSize));
            else
                arguments.ReadFromJSONString(result.pData);
            arguments.PushNumber(0);
        }
        else
        {
            arguments.PushString("ERROR");
            arguments.PushNumber(result.iErrorCode);
        }
    }
    else
    {
        // Append response body
        arguments.PushString(std::string(result.pData, result.dataSize));

        // Append info table
        CLuaArguments info;
        info.PushString("success");
        info.PushBoolean(result.iErrorCode >= 200 && result.iErrorCode <= 299);
        info.PushString("statusCode");
        info.PushNumber(result.iErrorCode);

        // Headers as a subtable
        CLuaArguments        headers;
        std::vector<SString> headerLineList;
        SStringX(result.szHeaders).Split("\n", headerLineList);
        for (const SString& strLine : headerLineList)
        {
            SString strKey, strValue;
            if (strLine.Split(": ", &strKey, &strValue))
            {
                headers.PushString(strKey);
                headers.PushString(strValue);
            }
        }
        info.PushString("headers");
        info.PushTable(&headers);

        arguments.PushTable(&info);
    }

    // Append stored arguments
    if (pCall->IsFetch())
    {
        for (CLuaArgument* argument : pCall->GetFetchArguments())
            arguments.PushArgument(*argument);
    }

    if (pCall->m_VM)
        arguments.Call(pCall->m_VM, pCall->m_iFunction);
    g_pGame->GetRemoteCalls()->Remove(pCall);
}

// Return true if cancel was done
bool CRemoteCall::CancelDownload()
{
    if (m_downloadMode != EDownloadModeType::NONE)
    {
        return g_pNetServer->GetHTTPDownloadManager(m_downloadMode)->CancelDownload(this, DownloadFinishedCallback);
    }
    return false;
}

const SDownloadStatus& CRemoteCall::GetDownloadStatus()
{
    if (m_downloadMode != EDownloadModeType::NONE)
    {
        SDownloadStatus newDownloadStatus;
        if (g_pNetServer->GetHTTPDownloadManager(m_downloadMode)->GetDownloadStatus(this, DownloadFinishedCallback, newDownloadStatus))
        {
            m_lastDownloadStatus = newDownloadStatus;
        }
    }
    return m_lastDownloadStatus;
}

// Notification that the remote call is no longer associated with a resource
void CRemoteCall::OnLuaMainDestroy()
{
    m_VM = nullptr;
}
