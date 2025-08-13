/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseType.h
 *  PURPOSE:     Generic database and connection
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRegistry.h"

// Only used for identifying 8 byte integers in varargs list
#define SQLITE_INTEGER64 10

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
    virtual ~CDatabaseType() {}

    virtual SString              GetDataSourceTag() = 0;
    virtual CDatabaseConnection* Connect(const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strDriverOptions) = 0;
    virtual void                 NotifyConnectionDeleted(CDatabaseConnection* pConnection) = 0;
    virtual void                 NotifyConnectionChanged(CDatabaseConnection* pConnection) = 0;
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
    virtual ~CDatabaseConnection() {}

    // CDatabaseConnection
    virtual bool           IsValid() = 0;
    virtual const SString& GetLastErrorMessage() = 0;
    virtual uint           GetLastErrorCode() = 0;
    virtual void           AddRef() = 0;
    virtual void           Release() = 0;
    virtual bool           Query(const SString& strQuery, CRegistryResult& registryResult) = 0;
    virtual void           Flush() = 0;
    virtual int            GetShareCount() = 0;

    bool          m_bLoggingEnabled;
    SString       m_strLogTag;
    SString       m_strOtherTag;
    std::set<int> m_SuppressedErrorCodes;
};

//
// Create interfaces for sqlite
//
CDatabaseType*       NewDatabaseTypeSqlite();
CDatabaseConnection* NewDatabaseConnectionSqlite(CDatabaseType* pManager, const SString& strPath, const SString& strOptions);

//
// Create interfaces for mysql
//
CDatabaseType* NewDatabaseTypeMySql();
typedef CDatabaseConnection*(NewDatabaseConnectionMySql_t)(CDatabaseType* pManager, const SString& strHost, const SString& strUsername,
                                                           const SString& strPassword, const SString& strOptions);
