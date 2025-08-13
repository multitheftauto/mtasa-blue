/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseManager.cpp
 *  PURPOSE:     Outside world interface for enjoying asynchronous database functionality
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDatabaseManager.h"
#include "CDatabaseJobQueueManager.h"
#include "lua/CLuaArguments.h"
#include "CRegistry.h"
#include "CIdArray.h"
#include "CGame.h"

SString InsertQueryArgumentsSqlite(const SString& strQuery, CLuaArguments* pArgs);
SString InsertQueryArgumentsMySql(const SString& strQuery, CLuaArguments* pArgs);
SString InsertQueryArgumentsSqlite(const char* szQuery, va_list vl);
SString InsertQueryArgumentsMySql(const char* szQuery, va_list vl);

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl
//
//
///////////////////////////////////////////////////////////////
class CDatabaseManagerImpl : public CDatabaseManager
{
public:
    ZERO_ON_NEW
    CDatabaseManagerImpl();
    virtual ~CDatabaseManagerImpl();

    // CDatabaseManager
    virtual void              DoPulse();
    virtual SConnectionHandle Connect(const SString& strType, const SString& strHost, const SString& strUsername, const SString& strPassword,
                                      const SString& strOptions);
    virtual bool              Disconnect(SConnectionHandle hConnection);
    virtual SString           PrepareString(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs = nullptr);
    virtual SString           PrepareStringf(SConnectionHandle hConnection, const char* szQuery, ...);
    virtual CDbJobData*       Exec(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs = nullptr);
    virtual CDbJobData*       Execf(SConnectionHandle hConnection, const char* szQuery, ...);
    virtual CDbJobData*       QueryStart(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs = nullptr);
    virtual CDbJobData*       QueryStartf(SConnectionHandle hConnection, const char* szQuery, ...);
    virtual bool              QueryPoll(CDbJobData* pJobData, uint ulTimeout);
    virtual bool              QueryFree(CDbJobData* pJobData);
    virtual CDbJobData*       GetQueryFromId(SDbJobId id);
    virtual const SString&    GetLastErrorMessage() { return m_strLastErrorMessage; }
    virtual bool              IsLastErrorSuppressed() { return m_bLastErrorSuppressed; }
    virtual bool              QueryWithResultf(SConnectionHandle hConnection, CRegistryResult* pResult, const char* szQuery, ...);
    virtual bool              QueryWithCallback(SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const SString& strQuery,
                                                CLuaArguments* pArgs = nullptr);
    virtual bool              QueryWithCallbackf(SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const char* szQuery, ...);
    virtual void              SetLogLevel(EJobLogLevelType logLevel, const SString& strLogFilename);

    // CDatabaseManagerImpl
    SString InsertQueryArguments(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs);
    SString InsertQueryArguments(SConnectionHandle hConnection, const char* szQuery, va_list vl);
    SString HideQuestionMark(const SString& strQuery);
    SString RestoreQuestionMark(const SString& strQuery);
    void    ClearLastErrorMessage()
    {
        m_strLastErrorMessage.clear();
        m_bLastErrorSuppressed = false;
    }
    void SetLastErrorMessage(const SString& strMsg, bool bSuppressed = false)
    {
        m_strLastErrorMessage = strMsg;
        m_bLastErrorSuppressed = bSuppressed;
    }

    CDatabaseJobQueueManager*            m_JobQueue;
    std::map<SConnectionHandle, SString> m_ConnectionTypeMap;
    SString                              m_strLastErrorMessage;
    bool                                 m_bLastErrorSuppressed;
};

///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseManager* NewDatabaseManager()
{
    return new CDatabaseManagerImpl();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::CDatabaseManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseManagerImpl::CDatabaseManagerImpl()
{
    m_JobQueue = new CDatabaseJobQueueManager();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::CDatabaseManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseManagerImpl::~CDatabaseManagerImpl()
{
    // Disconnect all active connections
    std::map<SConnectionHandle, SString> connectionTypeMapCopy = m_ConnectionTypeMap;
    for (std::map<SConnectionHandle, SString>::iterator iter = connectionTypeMapCopy.begin(); iter != connectionTypeMapCopy.end(); iter++)
        Disconnect(iter->first);

    SAFE_DELETE(m_JobQueue);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::DoPulse
//
// Check if any callback functions are due
//
///////////////////////////////////////////////////////////////
void CDatabaseManagerImpl::DoPulse()
{
    m_JobQueue->DoPulse();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::Connect
//
// strType is one of the supported database types i.e. "sqlite"
//
///////////////////////////////////////////////////////////////
SConnectionHandle CDatabaseManagerImpl::Connect(const SString& strType, const SString& strHost, const SString& strUsername, const SString& strPassword,
                                                const SString& strOptions)
{
    ClearLastErrorMessage();

    SString strCombo = strType + "\1" + strHost + "\1" + strUsername + "\1" + strPassword + "\1" + strOptions;

    // Start connect
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::CONNECT, 0, strCombo);

    // Complete connect
    m_JobQueue->PollCommand(pJobData, -1);

    // Check for problems
    if (pJobData->result.status == EJobResult::FAIL)
    {
        SetLastErrorMessage(pJobData->result.strReason);
        return INVALID_DB_HANDLE;
    }

    // Process result
    MapSet(m_ConnectionTypeMap, pJobData->command.connectionHandle, strType);
    return pJobData->command.connectionHandle;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::Disconnect
//
//
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::Disconnect(uint hConnection)
{
    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return false;
    }

    // Start disconnect
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::DISCONNECT, hConnection, "");
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        MapRemove(m_ConnectionTypeMap, hConnection);
        return false;
    }

    // Complete disconnect
    m_JobQueue->PollCommand(pJobData, -1);

    // Check for problems
    if (pJobData->result.status == EJobResult::FAIL)
    {
        SetLastErrorMessage(pJobData->result.strReason, pJobData->result.bErrorSuppressed);
        return false;
    }

    // Remove connection refs
    MapRemove(m_ConnectionTypeMap, hConnection);
    m_JobQueue->IgnoreConnectionResults(hConnection);
    return true;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::PrepareString
//
// Safely insert supplied arguments into string
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::PrepareString(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs)
{
    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return NULL;
    }

    // Insert arguments with correct escapement
    return HideQuestionMark(InsertQueryArguments(hConnection, strQuery, pArgs));
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::Exec
//
// Start a query and ignore the result
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::Exec(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs)
{
    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = RestoreQuestionMark(InsertQueryArguments(hConnection, strQuery, pArgs));

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::QUERY, hConnection, strEscapedQuery);
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        return nullptr;
    }

    // Ignore result
    m_JobQueue->FreeCommand(pJobData);
    return pJobData;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::PrepareStringf
//
// Safely insert supplied arguments into string
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::PrepareStringf(SConnectionHandle hConnection, const char* szQuery, ...)
{
    va_list vl;
    va_start(vl, szQuery);

    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return NULL;
    }

    // Insert arguments with correct escapement
    return HideQuestionMark(InsertQueryArguments(hConnection, szQuery, vl));
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::Execf
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::Execf(SConnectionHandle hConnection, const char* szQuery, ...)
{
    va_list vl;
    va_start(vl, szQuery);

    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = RestoreQuestionMark(InsertQueryArguments(hConnection, szQuery, vl));

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::QUERY, hConnection, strEscapedQuery);
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        return nullptr;
    }

    // Ignore result
    m_JobQueue->FreeCommand(pJobData);
    return pJobData;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryStart
//
// Start a query
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::QueryStart(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs)
{
    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = RestoreQuestionMark(InsertQueryArguments(hConnection, strQuery, pArgs));

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::QUERY, hConnection, strEscapedQuery);
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        return nullptr;
    }
    return pJobData;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryStartf
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::QueryStartf(SConnectionHandle hConnection, const char* szQuery, ...)
{
    va_list vl;
    va_start(vl, szQuery);

    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return NULL;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = RestoreQuestionMark(InsertQueryArguments(hConnection, szQuery, vl));

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::QUERY, hConnection, strEscapedQuery);
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        return nullptr;
    }
    return pJobData;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryWithResultf
//
// Start a query and wait for the result
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryWithResultf(SConnectionHandle hConnection, CRegistryResult* pResult, const char* szQuery, ...)
{
    va_list vl;
    va_start(vl, szQuery);

    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return false;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = RestoreQuestionMark(InsertQueryArguments(hConnection, szQuery, vl));

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::QUERY, hConnection, strEscapedQuery);
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        return false;
    }

    // Wait for result
    QueryPoll(pJobData, -1);

    // Process result
    if (pJobData->result.status == EJobResult::FAIL)
    {
        if (pResult)
            *pResult = CRegistryResult();
        return false;
    }
    else
    {
        if (pResult)
            *pResult = pJobData->result.registryResult;
        return true;
    }
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryWithCallback
//
// Start a query and direct the result through a callback
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryWithCallback(SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const SString& strQuery,
                                             CLuaArguments* pArgs)
{
    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return false;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = RestoreQuestionMark(InsertQueryArguments(hConnection, strQuery, pArgs));

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::QUERY, hConnection, strEscapedQuery);
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        return false;
    }

    // Set callback vars
    pJobData->SetCallback(pfnDbResult, pCallbackContext);

    return true;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryWithCallbackf
//
// Start a query and direct the result through a callback
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryWithCallbackf(SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const char* szQuery, ...)
{
    va_list vl;
    va_start(vl, szQuery);

    ClearLastErrorMessage();

    // Check connection
    if (!MapContains(m_ConnectionTypeMap, hConnection))
    {
        SetLastErrorMessage("Invalid connection");
        return false;
    }

    // Insert arguments with correct escapement
    SString strEscapedQuery = RestoreQuestionMark(InsertQueryArguments(hConnection, szQuery, vl));

    // Start query
    CDbJobData* pJobData = m_JobQueue->AddCommand(EJobCommand::QUERY, hConnection, strEscapedQuery);
    if (!pJobData)
    {
        SetLastErrorMessage("Invalid connection");
        return false;
    }

    // Set callback vars
    pJobData->SetCallback(pfnDbResult, pCallbackContext);

    return true;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryPoll
//
// ulTimeout = 0   -  No wait if not ready
// ulTimeout > 0   -  Wait(ms) if not ready
// ulTimeout = -1  -  Wait infinity+1 if not ready
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryPoll(CDbJobData* pJobData, uint ulTimeout)
{
    ClearLastErrorMessage();

    if (m_JobQueue->PollCommand(pJobData, ulTimeout))
    {
        if (pJobData->result.status == EJobResult::FAIL)
            SetLastErrorMessage(pJobData->result.strReason, pJobData->result.bErrorSuppressed);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::QueryFree
//
//
//
///////////////////////////////////////////////////////////////
bool CDatabaseManagerImpl::QueryFree(CDbJobData* pJobData)
{
    ClearLastErrorMessage();
    return m_JobQueue->FreeCommand(pJobData);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::GetQueryFromId
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData* CDatabaseManagerImpl::GetQueryFromId(SDbJobId id)
{
    return m_JobQueue->FindCommandFromId(id);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::SetLogLevel
//
//
//
///////////////////////////////////////////////////////////////
void CDatabaseManagerImpl::SetLogLevel(EJobLogLevelType logLevel, const SString& strLogFilename)
{
    return m_JobQueue->SetLogLevel(logLevel, strLogFilename);
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::InsertQueryArguments
//
// Insert arguments and apply correct escapement for the connection type
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::InsertQueryArguments(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs)
{
    // Check for case of no arguments
    if (!pArgs)
        return strQuery;

    // Determine connection type
    SString* pstrType = MapFind(m_ConnectionTypeMap, hConnection);
    SString  strType = pstrType ? *pstrType : "";

    if (strType == "sqlite")
        return InsertQueryArgumentsSqlite(strQuery, pArgs);
    else if (strType == "mysql")
        return InsertQueryArgumentsMySql(strQuery, pArgs);

    // 'Helpful' error message
    CLogger::ErrorPrintf("DatabaseManager internal error #1\n");
    return "";
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::InsertQueryArguments
//
// Insert arguments and apply correct escapement for the connection type
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::InsertQueryArguments(SConnectionHandle hConnection, const char* szQuery, va_list vl)
{
    // Determine connection type
    SString* pstrType = MapFind(m_ConnectionTypeMap, hConnection);
    SString  strType = pstrType ? *pstrType : "";

    if (strType == "sqlite")
        return InsertQueryArgumentsSqlite(szQuery, vl);
    else if (strType == "mysql")
        return InsertQueryArgumentsMySql(szQuery, vl);

    // 'Helpful' error message
    CLogger::ErrorPrintf("DatabaseManager internal error #2");
    return "";
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::HideQuestionMark
//
// '?' must be hidden to prevent it being used as a variable placeholder in subsequent InsertQueryArguments calls.
// Uses an unlikely UTF-8 sequence.
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::HideQuestionMark(const SString& strQuery)
{
    return strQuery.Replace("?", "\xF3\xB0\x83\xBF\xF4\x80\x81\x9A");
}

///////////////////////////////////////////////////////////////
//
// CDatabaseManagerImpl::RestoreQuestionMark
//
// Undo HideQuestionMark()
//
///////////////////////////////////////////////////////////////
SString CDatabaseManagerImpl::RestoreQuestionMark(const SString& strQuery)
{
    return strQuery.Replace("\xF3\xB0\x83\xBF\xF4\x80\x81\x9A", "?");
}

///////////////////////////////////////////////////////////////
//
// CDbJobData::CDbJobData
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData::CDbJobData()
{
    id = CIdArray::PopUniqueId(this, EIdClass::DB_JOBDATA);
}

///////////////////////////////////////////////////////////////
//
// CDbJobData::~CDbJobData
//
//
//
///////////////////////////////////////////////////////////////
CDbJobData::~CDbJobData()
{
    CIdArray::PushUniqueId(this, EIdClass::DB_JOBDATA, id);
}

///////////////////////////////////////////////////////////////
//
// CDbJobData::SetCallback
//
// Set callback for a job data
// Returns false if callback could not be set
//
///////////////////////////////////////////////////////////////
bool CDbJobData::SetCallback(PFN_DBRESULT pfnDbResult, void* pContext)
{
    if (callback.bSet)
        return false;            // One has already been set

    if (this->stage > EJobStage::RESULT)
        return false;            // Too late to set a callback now

    // Set new
    callback.pfnDbResult = pfnDbResult;
    callback.pContext = pContext;
    callback.bSet = true;
    return true;
}

///////////////////////////////////////////////////////////////
//
// CDbJobData::HasCallback
//
// Returns true if callback has been set and has not been called yet
//
///////////////////////////////////////////////////////////////
bool CDbJobData::HasCallback()
{
    return callback.bSet && !callback.bDone;
}

///////////////////////////////////////////////////////////////
//
// CDbJobData::ProcessCallback
//
// Do callback
//
///////////////////////////////////////////////////////////////
void CDbJobData::ProcessCallback()
{
    assert(HasCallback());
    callback.bDone = true;
    callback.pfnDbResult(this, callback.pContext);
}

///////////////////////////////////////////////////////////////
//
// CDbJobData::GetCommandStringForLog
//
// Remove sensitive info
//
///////////////////////////////////////////////////////////////
SString CDbJobData::GetCommandStringForLog()
{
    if (command.type == EJobCommand::CONNECT)
    {
        std::vector<SString> parts;
        command.strData.Split("\1", parts);
        if (parts.size() >= 5)
        {
            // Remove host, username and password
            return parts[0] + " " + parts[4];
        }
    }
    return command.strData;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionElement::Unlink
//
///////////////////////////////////////////////////////////////
void CDatabaseConnectionElement::Unlink()
{
    g_pGame->GetDatabaseManager()->Disconnect(m_Connection);
}
