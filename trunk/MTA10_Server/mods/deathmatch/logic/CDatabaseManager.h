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
typedef uint SDbJobId;
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

using EJobCommand::EJobCommandType;
using EJobResult::EJobResultType;
using EJobStage::EJobStageType;


//
// All data realating to a database job
//
class CDbJobData
{
public:
    ZERO_ON_NEW

                CDbJobData      ( SDbJobId id ) : id ( id ) {}
    SDbJobId    GetId           ( void ) { return id; }

    EJobStageType       stage;
    SDbJobId            id;
    class CLuaCallback* pLuaCallback;

    struct
    {
        EJobCommandType     type;
        SConnectionHandle   connectionHandle;
        SString             strData;
    } command;

    struct
    {
        EJobResultType      status;
        SString             strReason;
        SConnectionHandle   connectionHandle;
        CRegistryResult     registryResult;
    } result;
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
    virtual bool                    Exec                    ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs ) = 0;
    virtual bool                    Execf                   ( SConnectionHandle hConnection, const char* szQuery, ... ) = 0;
    virtual CDbJobData*             QueryStart              ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs ) = 0;
    virtual CDbJobData*             QueryStartf             ( SConnectionHandle hConnection, const char* szQuery, ... ) = 0;
    virtual bool                    QueryPoll               ( CDbJobData* pJobData, ulong ulTimeout ) = 0;
    virtual bool                    QueryFree               ( CDbJobData* pJobData ) = 0;
    virtual CDbJobData*             GetQueryFromId          ( SDbJobId id ) = 0;
    virtual const SString&          GetLastErrorMessage     ( void ) = 0;
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
