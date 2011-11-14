/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseTypeMySql.cpp
*  PURPOSE:     MySql connection maker
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDatabaseType.h"


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql
//
//
///////////////////////////////////////////////////////////////
class CDatabaseTypeMySql : public CDatabaseType
{
public:
    ZERO_ON_NEW
                                    CDatabaseTypeMySql         ( void );
    virtual                         ~CDatabaseTypeMySql        ( void );

    // CDatabaseType
    virtual SString                 GetDataSourceTag            ( void );
    virtual CDatabaseConnection*    Connect                     ( const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strDriverOptions );
    virtual void                    NotifyConnectionDeleted     ( CDatabaseConnection* pConnection );

    // CDatabaseTypeMySql

    std::map < SString, CDatabaseConnection* >    m_SharedConnectionMap;
    std::set < CDatabaseConnection* >             m_AllConnectionMap;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseType* NewDatabaseTypeMySql ( void )
{
    return new CDatabaseTypeMySql ();
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::CDatabaseTypeMySql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeMySql::CDatabaseTypeMySql ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::CDatabaseTypeMySql
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeMySql::~CDatabaseTypeMySql ( void )
{
    assert ( m_SharedConnectionMap.empty () );
    assert ( m_AllConnectionMap.empty () );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::GetDataSourceTag
//
// Return database type as a string
//
///////////////////////////////////////////////////////////////
SString CDatabaseTypeMySql::GetDataSourceTag ( void )
{
    return "mysql";
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::NotifyConnectionDeleted
//
// Remove connection from internal lists
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeMySql::NotifyConnectionDeleted ( CDatabaseConnection* pConnection )
{
    assert ( MapContains ( m_AllConnectionMap, pConnection ) );
    MapRemove ( m_AllConnectionMap, pConnection );
    MapRemoveByValue ( m_SharedConnectionMap, pConnection );
}


///////////////////////////////////////////////////////////////
//
// CDatabaseTypeMySql::Connect
//
// strHost is like "dbname=testdb;host=127.0.0.1;port=3303"
// strUsername is username
// strPassword is password
// strOptions contains key=value pairs seperated by semicolons
//
//      Options are:
//          share=1     // Share this connection with anything else (defaults to share=1)
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseTypeMySql::Connect ( const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions )
{
    // Parse options
    int bShareConnection = false;
    {
        CArgMap argMap ( "=", ";" );
        argMap.SetFromString ( strOptions );
        argMap.Get ( "share", bShareConnection, 0 );
    }

    CDatabaseConnection* pConnection = NULL;

    // Can we share a connection?
    if ( !bShareConnection )
    {
        // No sharing so create a new connection
        pConnection = NewDatabaseConnectionMySql ( this, strHost, strUsername, strPassword, strOptions );
    }
    else
    {
        // Yes sharing, so make a key
        SString strShareKey = strHost + "*" + strOptions;

        // Look for a match
        pConnection = MapFindRef ( m_SharedConnectionMap, strShareKey );
        if ( !pConnection )
        {
            // No match, so create a new connection
            pConnection = NewDatabaseConnectionMySql ( this, strHost, strUsername, strPassword, strOptions );
            MapSet ( m_SharedConnectionMap, strShareKey, pConnection );
        }
        else
        {
            // Yes match, so add a ref to existing connection
            pConnection->AddRef ();
        }
    }

    MapInsert ( m_AllConnectionMap, pConnection );

    return pConnection;
}
