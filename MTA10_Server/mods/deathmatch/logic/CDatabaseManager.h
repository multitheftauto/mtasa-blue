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

typedef uint SConnectionHandle;
typedef uint SJobHandle;
#define INVALID_DB_HANDLE (0)

enum EResultStatus
{
    STATUS_SUCCESS,
    STATUS_FAIL,
};

struct SQueryResult
{
    EResultStatus       status;
    SString             strReason;
    SConnectionHandle   connectionHandle;
    CRegistryResult     registryResult;
};


///////////////////////////////////////////////////////////////
//
// CDatabaseManager
//
//   How it works:
//
//    Connect ->     \
//    Query* ->         -> DatabaseManager -> JobQueue -> DatabaseType -> DatabaseConnection
//    Disconnect ->  /
//
///////////////////////////////////////////////////////////////
class CDatabaseManager
{
public:
    virtual                         ~CDatabaseManager       ( void ) {}

    virtual SConnectionHandle       Connect                 ( const SString& strType, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions ) = 0;
    virtual bool                    Disconnect              ( SConnectionHandle hConnection ) = 0;
    virtual SJobHandle              QueryStart              ( SConnectionHandle hConnection, const SString& strQuery, CLuaArguments* pArgs ) = 0;
    virtual bool                    QueryPoll               ( SQueryResult& result, SJobHandle hQuery, ulong ulTimeout ) = 0;
    virtual bool                    QueryFree               ( SJobHandle hQuery ) = 0;
    virtual bool                    IsValidQuery            ( SJobHandle hQuery ) = 0;
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
