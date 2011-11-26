/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseTypeSqlite.cpp
*  PURPOSE:     Sqlite connection maker
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDatabaseType.h"


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite
//
//
///////////////////////////////////////////////////////////////
class CDatabaseTypeSqlite : public CDatabaseType
{
public:
    ZERO_ON_NEW
                                    CDatabaseTypeSqlite         ( void );
    virtual                         ~CDatabaseTypeSqlite        ( void );

    // CDatabaseType
    virtual SString                 GetDataSourceTag            ( void );
    virtual CDatabaseConnection*    Connect                     ( const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strDriverOptions );
    virtual void                    NotifyConnectionDeleted     ( CDatabaseConnection* pConnection );
    virtual void                    NotifyConnectionChanged     ( CDatabaseConnection* pConnection );

    // CDatabaseTypeSqlite
    void                            UpdateStats                 ( void );

    std::map < SString, CDatabaseConnection* >    m_SharedConnectionMap;
    std::set < CDatabaseConnection* >             m_AllConnectionMap;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseType* NewDatabaseTypeSqlite ( void )
{
    return new CDatabaseTypeSqlite ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::CDatabaseTypeSqlite
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeSqlite::CDatabaseTypeSqlite ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::CDatabaseTypeSqlite
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeSqlite::~CDatabaseTypeSqlite ( void )
{
    assert ( m_SharedConnectionMap.empty () );
    assert ( m_AllConnectionMap.empty () );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::GetDataSourceTag
//
// Return database type as a string
//
///////////////////////////////////////////////////////////////
SString CDatabaseTypeSqlite::GetDataSourceTag ( void )
{
    return "sqlite";
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::NotifyConnectionDeleted
//
// Remove connection from internal lists
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeSqlite::NotifyConnectionDeleted ( CDatabaseConnection* pConnection )
{
    assert ( MapContains ( m_AllConnectionMap, pConnection ) );
    MapRemove ( m_AllConnectionMap, pConnection );
    MapRemoveByValue ( m_SharedConnectionMap, pConnection );
    UpdateStats ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::NotifyConnectionChanged
//
// Update things that need to know
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeSqlite::NotifyConnectionChanged ( CDatabaseConnection* pConnection )
{
    UpdateStats ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::Connect
//
// strHost is the absolute path to the sqlite database file
// strUsername is not used
// strPassword is not used
// strOptions contains key=value pairs seperated by semicolons
//
//      Options are:
//          share=1     // Share this connection with anything else (defaults to share=1)
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseTypeSqlite::Connect ( const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
{
    // Parse file path
    SString strPath = strHost;

    // Parse options
    bool bShareConnection = true;
    GetOption < CDbOptionsMap > ( strOptions, "share", bShareConnection, true );

    CDatabaseConnection* pConnection = NULL;

    // Can we share a connection?
    if ( !bShareConnection )
    {
        // No sharing so create a new connection
        pConnection = NewDatabaseConnectionSqlite ( this, strPath, strOptions );
        pConnection->m_strOtherTag = strHost + "%" + strOptions;
    }
    else
    {
        // Yes sharing, so make a key
        SString strShareKey = strPath + "%" + strOptions;

        // Look for a match
        pConnection = MapFindRef ( m_SharedConnectionMap, strShareKey );
        if ( !pConnection )
        {
            // No match, so create a new connection
            pConnection = NewDatabaseConnectionSqlite ( this, strPath, strOptions );
            MapSet ( m_SharedConnectionMap, strShareKey, pConnection );
        }
        else
        {
            // Yes match, so add a ref to existing connection
            pConnection->AddRef ();
        }
    }

    MapInsert ( m_AllConnectionMap, pConnection );
    UpdateStats ();

    return pConnection;
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::UpdateStats
//
// Tracking connections
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeSqlite::UpdateStats ( void )
{
    SString strModPath = PathConform ( g_pServerInterface->GetModManager ()->GetModPath () );

    // Remove all lines with this key
    CPerfStatDebugTable::GetSingleton ()->RemoveLines ( "dbcon sqlite*" );

    int iIndex = 0;
    std::set < CDatabaseConnection* > unsharedConnectionMap = m_AllConnectionMap;

    // Add shared info
    for ( std::map < SString, CDatabaseConnection* >::iterator iter = m_SharedConnectionMap.begin () ; iter != m_SharedConnectionMap.end () ; ++iter )
    {
        const SString& strShareKey = iter->first.TrimStart ( strModPath );
        CDatabaseConnection* pConnection = iter->second;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton ()->UpdateLine ( *SString ( "dbcon sqlite %d", iIndex++ ), 0
                                                          ,"Database connection: sqlite (shared)"
                                                          ,*strShareKey
                                                          ,*SString ( "Share count: %d", pConnection->GetShareCount () )
                                                          , NULL
                                                );

        // Update unshared map for the second part
        MapRemove ( unsharedConnectionMap, pConnection );
    }

    // Add unshared info
    for ( std::set < CDatabaseConnection* >::iterator iter = unsharedConnectionMap.begin () ; iter != unsharedConnectionMap.end () ; ++iter )
    {
        CDatabaseConnection* pConnection = *iter;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton ()->UpdateLine ( *SString ( "dbcon sqlite %d", iIndex++ ), 0
                                                          ,"Database connection: sqlite (unshared)"
                                                          ,*pConnection->m_strOtherTag
                                                          ,*SString ( "Refs: %d", pConnection->GetShareCount () )
                                                          , NULL
                                                );
    }
}
