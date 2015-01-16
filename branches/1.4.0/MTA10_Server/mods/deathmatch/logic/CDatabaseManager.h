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

typedef uint SDbConnectionId;
typedef intptr_t SDbJobId;
typedef SDbConnectionId SConnectionHandle;
#define INVALID_DB_HANDLE (0)

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
using EJobResult::EJobResultType;
using EJobStage::EJobStageType;
using EJobLogLevel::EJobLogLevelType;

typedef void (*PFN_DBRESULT) ( CDbJobData* pJobData, void* pContext );


//
// Specialized map for the options string
//
class CDbOptionsMap : public CArgMap
{
public:
    CDbOptionsMap ( void ) : CArgMap ( "=", ";" ) {}
};


//
// All data realating to a database job
//
class CDbJobData
{
public:
    ZERO_ON_NEW

                CDbJobData      ( void );
                ~CDbJobData     ( void );
    SDbJobId    GetId           ( void ) { return id; }
    bool        SetCallback     ( PFN_DBRESULT pfnDbResult, void* pContext );
    bool        HasCallback     ( void );
    void        ProcessCallback ( void );
    void        SetLuaDebugInfo ( const SLuaDebugInfo& luaDebugInfo ) { m_LuaDebugInfo = luaDebugInfo; }

    EJobStageType       stage;
    SDbJobId            id;
    SLuaDebugInfo       m_LuaDebugInfo;

    struct
    {
        EJobCommandType     type;
        SConnectionHandle   connectionHandle;
        SString             strData;
    } command;

    struct
    {
        EJobResultType      status;
        uint                uiErrorCode;
        SString             strReason;
        bool                bErrorSuppressed;
        SConnectionHandle   connectionHandle;
        uint                uiNumAffectedRows;
        uint64              ullLastInsertId;
        CRegistryResult     registryResult;
        CTickCount          timeReady;
        bool                bLoggedWarning;
        bool                bIgnoreResult;
    } result;

    struct
    {
        PFN_DBRESULT        pfnDbResult;
        void*               pContext;
        bool                bSet;
        bool                bDone;
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
    virtual                         ~CDatabaseManager       ( void ) {}

    virtual void                    DoPulse                 ( void ) = 0;
    virtual SConnectionHandle       Connect                 ( const SString& strType, const SString& strHost, const SString& strUsername = "", const SString& strPassword = "", const SString& strOptions = "" ) = 0;
    virtual bool                    Disconnect              ( SConnectionHandle hConnection ) = 0;
    virtual CDbJobData*             Exec                    ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs ) = 0;
    virtual CDbJobData*             Execf                   ( SConnectionHandle hConnection, const char* szQuery, ... ) = 0;
    virtual CDbJobData*             QueryStart              ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs ) = 0;
    virtual CDbJobData*             QueryStartf             ( SConnectionHandle hConnection, const char* szQuery, ... ) = 0;
    virtual bool                    QueryPoll               ( CDbJobData* pJobData, uint ulTimeout ) = 0;
    virtual bool                    QueryFree               ( CDbJobData* pJobData ) = 0;
    virtual CDbJobData*             GetQueryFromId          ( SDbJobId id ) = 0;
    virtual const SString&          GetLastErrorMessage     ( void ) = 0;
    virtual bool                    IsLastErrorSuppressed   ( void ) = 0;
    virtual bool                    QueryWithResultf        ( SConnectionHandle hConnection, CRegistryResult* pResult, const char* szQuery, ... ) = 0;
    virtual bool                    QueryWithCallbackf      ( SConnectionHandle hConnection, PFN_DBRESULT pfnDbResult, void* pCallbackContext, const char* szQuery, ... ) = 0;
    virtual void                    SetLogLevel             ( EJobLogLevelType logLevel, const SString& strLogFilename ) = 0;
};

CDatabaseManager* NewDatabaseManager ( void );


///////////////////////////////////////////////////////////////
//
// CDatabaseConnectionElement
//
// Wraps a connection handle inside an element. (For auto-disconnect when a resource stops)
// TODO - Check it does not get synced to the client
//
///////////////////////////////////////////////////////////////
class CDatabaseConnectionElement : public CElement
{
public:
                                CDatabaseConnectionElement  ( CElement* pParent, SConnectionHandle connection )
                                                                : CElement ( pParent )
                                                                , m_Connection ( connection )
                                                            {
                                                                m_iType = CElement::DATABASE_CONNECTION;
                                                                SetTypeName ( "db-connection" );
                                                            }

    virtual                     ~CDatabaseConnectionElement ( void ) {}

    // CElement
    virtual void                Unlink                      ( void )    { g_pGame->GetDatabaseManager ()->Disconnect ( m_Connection ); }
    virtual bool                ReadSpecialData             ( void )    { return false; }

    // CDatabaseConnectionElement
    SConnectionHandle           GetConnectionHandle         ( void )    { return m_Connection; }

protected:
    SConnectionHandle           m_Connection;
};
