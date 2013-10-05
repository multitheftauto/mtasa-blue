/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseType.h
*  PURPOSE:     Generic database and connection
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CDatabaseConnection;

///////////////////////////////////////////////////////////////
//
// CDatabaseType
//
// Creates connections to required database type
//
///////////////////////////////////////////////////////////////
class CDatabaseType
{
public:
    virtual                         ~CDatabaseType              ( void ) {}
   
    virtual SString                 GetDataSourceTag            ( void ) = 0;
    virtual CDatabaseConnection*    Connect                     ( const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strDriverOptions ) = 0;
    virtual void                    NotifyConnectionDeleted     ( CDatabaseConnection* pConnection ) = 0;
    virtual void                    NotifyConnectionChanged     ( CDatabaseConnection* pConnection ) = 0;
};


///////////////////////////////////////////////////////////////
//
// CDatabaseConnection
//
// Routes queries and parses results
//
///////////////////////////////////////////////////////////////
class CDatabaseConnection
{
public:
    virtual                 ~CDatabaseConnection    ( void ) {}

    // CDatabaseConnection
    virtual bool            IsValid                 ( void ) = 0;
    virtual const SString&  GetLastErrorMessage     ( void ) = 0;
    virtual uint            GetLastErrorCode        ( void ) = 0;
    virtual uint            GetNumAffectedRows      ( void ) = 0;
    virtual uint64          GetLastInsertId         ( void ) = 0;
    virtual void            AddRef                  ( void ) = 0;
    virtual void            Release                 ( void ) = 0;
    virtual bool            Query                   ( const SString& strQuery, CRegistryResult& registryResult ) = 0;
    virtual void            Flush                   ( void ) = 0;
    virtual int             GetShareCount           ( void ) = 0;

    bool                    m_bLoggingEnabled;
    SString                 m_strLogTag;
    SString                 m_strOtherTag;
    std::set < int >        m_SuppressedErrorCodes;
};


//
// Create interfaces for sqlite
//
CDatabaseType*          NewDatabaseTypeSqlite       ( void );
CDatabaseConnection*    NewDatabaseConnectionSqlite ( CDatabaseType* pManager, const SString& strPath, const SString& strOptions );

//
// Create interfaces for mysql
//
CDatabaseType*          NewDatabaseTypeMySql        ( void );
typedef CDatabaseConnection* ( NewDatabaseConnectionMySql_t )( CDatabaseType* pManager, const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions );
