/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseManager.h
 *  PURPOSE:     Outside world interface for enjoying asynchronous database functionality
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CDatabaseJobQueue;
typedef uint            SDbConnectionId;
typedef intptr_t        SDbJobId;
typedef SDbConnectionId SConnectionHandle;
#define INVALID_DB_HANDLE (0)
#define DB_SQLITE_QUEUE_NAME_INTERNAL   "sqlite internal"
#define DB_SQLITE_QUEUE_NAME_DEFAULT    "sqlite"            // Note: MySql default queue name is the host string

namespace EJobResult
{
    enum EJobResultType
    {
        NONE,
        SUCCESS,
        FAIL,
    };
};

namespace EJobCommand
{
    enum EJobCommandType
    {
        NONE,
        CONNECT,
        DISCONNECT,
        QUERY,
        FLUSH,
        SETLOGLEVEL,
    };
};

namespace EJobStage
{
    enum EJobStageType
    {
        NONE,
        COMMAND_QUEUE,
        PROCCESSING,
        RESULT,
        FINISHED,
    };
};

namespace EJobLogLevel
{
    enum EJobLogLevelType
    {
        NONE,
        ERRORS,
        ALL,
    };
};

using EJobCommand::EJobCommandType;
using EJobLogLevel::EJobLogLevelType;
using EJobResult::EJobResultType;
using EJobStage::EJobStageType;

typedef void (*PFN_DBRESULT)(CDbJobData* pJobData, void* pContext);

//
// Specialized map for the options string
//
class CDbOptionsMap : public CArgMap
{
public:
    CDbOptionsMap() : CArgMap("=", ";") {}
};

//
// All data realating to a database job
//
class CDbJobData
{
public:
    ZERO_ON_NEW

    CDbJobData();
    ~CDbJobData();
    SDbJobId           GetId() const { return id; }
    bool               SetCallback(PFN_DBRESULT pfnDbResult, void* pContext);
    bool               HasCallback();
    void               ProcessCallback();
    void               SetLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_LuaDebugInfo = luaDebugInfo; }
    CDatabaseJobQueue* GetQueue() { return command.pJobQueue; }
    SString            GetCommandStringForLog();

    EJobStageType stage;
    SDbJobId      id;
    SLuaDebugInfo m_LuaDebugInfo;

    struct
    {
        EJobCommandType    type;
        SConnectionHandle  connectionHandle;
        SString            strData;
        CDatabaseJobQueue* pJobQueue;
    } command;

    struct
    {
        EJobResultType  status;
        uint            uiErrorCode;
        SString         strReason;
        bool            bErrorSuppressed;
        CRegistryResult registryResult;
        CTickCount      timeReady;
        bool            bLoggedWarning;
        bool            bIgnoreResult;
    } result;

    struct
    {
        PFN_DBRESULT pfnDbResult;
        void*        pContext;
        bool         bSet;
        bool         bDone;
    } callback;
};

///////////////////////////////////////////////////////////////
//
// CDatabaseManager
//
//   How it works:
//
//    Connect ->     |
//    Query* ->      | -> DatabaseManager -> JobQueue -> DatabaseType -> DatabaseConnection
//    Disconnect ->  |
//
///////////////////////////////////////////////////////////////
class CDatabaseManager
{
public:
    virtual ~CDatabaseManager() {}

    virtual void              DoPulse() = 0;
    virtual SConnectionHandle Connect(const SString& strType, const SString& strHost, const SString& strUsername = "", const SString& strPassword = "",
                                      const SString& strOptions = "") = 0;
    virtual bool              Disconnect(SConnectionHandle hConnection) = 0;
    virtual SString           PrepareString(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs = nullptr) = 0;
    virtual SString           PrepareStringf(SConnectionHandle hConnection, const char* szQuery, ...) = 0;
    virtual CDbJobData*       Exec(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs = nullptr) = 0;
    virtual CDbJobData*       Execf(SConnectionHandle hConnection, const char* szQuery, ...) = 0;
    virtual CDbJobData*       QueryStart(SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs = nullptr) = 0;
    virtual CDbJobData*       QueryStartf(SConnectionHandle hConnection, const char* szQuery, ...) = 0;
    virtual bool              QueryPoll(CDbJobData* pJobData, uint ulTimeout) = 0;
    virtual bool              QueryFree(CDbJobData* pJobData) = 0;
    virtual CDbJobData*       GetQueryFromId(SDbJobId id) = 0;
    virtual const SString&    GetLastErrorMessage() = 0;
    virtual bool              IsLastErrorSuppressed() = 0;
    virtual bool              QueryWithResultf(SConnectionHandle hConnection, CRegistryResult* pResult, const char* szQuery, ...) = 0;
    virtual bool              QueryWithCallback(SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const SString& strQuery,
                                                CLuaArguments* pArgs = nullptr) = 0;
    virtual bool              QueryWithCallbackf(SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const char* szQuery, ...) = 0;
    virtual void              SetLogLevel(EJobLogLevelType logLevel, const SString& strLogFilename) = 0;
};

CDatabaseManager* NewDatabaseManager();

///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionElement
//
// Wraps a connection handle inside an element. (For auto-disconnect when a resource stops)
// TODO - Check it does not get synced to the client
//
///////////////////////////////////////////////////////////////
class CDatabaseConnectionElement final : public CElement
{
public:
    CDatabaseConnectionElement(CElement* pParent, SConnectionHandle connection) : CElement(pParent), m_Connection(connection)
    {
        m_iType = CElement::DATABASE_CONNECTION;
        SetTypeName("db-connection");
    }

    virtual ~CDatabaseConnectionElement() {}

    // CElement
    virtual void Unlink() { g_pGame->GetDatabaseManager()->Disconnect(m_Connection); }

    // CDatabaseConnectionElement
    SConnectionHandle GetConnectionHandle() { return m_Connection; }

protected:
    bool ReadSpecialData(const int iLine) override { return false; }

protected:
    SConnectionHandle m_Connection;
};
