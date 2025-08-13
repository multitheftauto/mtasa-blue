/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDatabaseTypeSqlite.cpp
 *  PURPOSE:     Sqlite connection maker
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDatabaseType.h"
#include "CDatabaseManager.h"
#include "CPerfStatManager.h"

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
    CDatabaseTypeSqlite();
    virtual ~CDatabaseTypeSqlite();

    // CDatabaseType
    virtual SString              GetDataSourceTag();
    virtual CDatabaseConnection* Connect(const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strDriverOptions);
    virtual void                 NotifyConnectionDeleted(CDatabaseConnection* pConnection);
    virtual void                 NotifyConnectionChanged(CDatabaseConnection* pConnection);

    // CDatabaseTypeSqlite
    void UpdateStats();

    std::map<SString, CDatabaseConnection*> m_SharedConnectionMap;
    std::set<CDatabaseConnection*>          m_AllConnectionMap;
    SString                                 m_strStatsKeyHead;
};

///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CDatabaseType* NewDatabaseTypeSqlite()
{
    return new CDatabaseTypeSqlite();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::CDatabaseTypeSqlite
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeSqlite::CDatabaseTypeSqlite()
{
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::CDatabaseTypeSqlite
//
//
//
///////////////////////////////////////////////////////////////
CDatabaseTypeSqlite::~CDatabaseTypeSqlite()
{
    assert(m_SharedConnectionMap.empty());
    assert(m_AllConnectionMap.empty());
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::GetDataSourceTag
//
// Return database type as a string
//
///////////////////////////////////////////////////////////////
SString CDatabaseTypeSqlite::GetDataSourceTag()
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
void CDatabaseTypeSqlite::NotifyConnectionDeleted(CDatabaseConnection* pConnection)
{
    assert(MapContains(m_AllConnectionMap, pConnection));
    MapRemove(m_AllConnectionMap, pConnection);
    MapRemoveByValue(m_SharedConnectionMap, pConnection);
    UpdateStats();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::NotifyConnectionChanged
//
// Update things that need to know
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeSqlite::NotifyConnectionChanged(CDatabaseConnection* pConnection)
{
    UpdateStats();
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::Connect
//
// strHost is the absolute path to the sqlite database file
// strUsername is not used
// strPassword is not used
// strOptions contains key=value pairs separated by semicolons
//
//      Options are:
//          share=1     // Share this connection with anything else (defaults to share=1)
//
///////////////////////////////////////////////////////////////
CDatabaseConnection* CDatabaseTypeSqlite::Connect(const SString& strHost, const SString& strUsername, const SString& strPassword, const SString& strOptions)
{
    // Parse file path
    SString strPath = strHost;

    // Parse options
    bool bShareConnection = true;
    GetOption<CDbOptionsMap>(strOptions, "share", bShareConnection, true);

    CDatabaseConnection* pConnection = NULL;

    // Can we share a connection?
    if (!bShareConnection)
    {
        // No sharing so create a new connection
        pConnection = NewDatabaseConnectionSqlite(this, strPath, strOptions);
        if (pConnection)
            pConnection->m_strOtherTag = strHost + "%" + strOptions;
    }
    else
    {
        // Yes sharing, so make a key
        SString strShareKey = strPath + "%" + strOptions;

        // Look for a match
        pConnection = MapFindRef(m_SharedConnectionMap, strShareKey);
        if (!pConnection)
        {
            // No match, so create a new connection
            pConnection = NewDatabaseConnectionSqlite(this, strPath, strOptions);
            if (pConnection)
                MapSet(m_SharedConnectionMap, strShareKey, pConnection);
        }
        else
        {
            // Yes match, so add a ref to existing connection
            pConnection->AddRef();
        }
    }

    if (pConnection)
        MapInsert(m_AllConnectionMap, pConnection);

    // For stats
    SString strQueueName;
    GetOption<CDbOptionsMap>(strOptions, "queue", strQueueName);
    m_strStatsKeyHead = SString("dbcon sqlite [%s] ", *strQueueName);
    UpdateStats();

    return pConnection;
}

///////////////////////////////////////////////////////////////
//
// CDatabaseTypeSqlite::UpdateStats
//
// Tracking connections
//
///////////////////////////////////////////////////////////////
void CDatabaseTypeSqlite::UpdateStats()
{
    SString strModPath = PathConform(g_pServerInterface->GetModManager()->GetModPath());

    // Remove all lines with this key
    CPerfStatDebugTable::GetSingleton()->RemoveLines(m_strStatsKeyHead + "*");

    int                            iIndex = 0;
    std::set<CDatabaseConnection*> unsharedConnectionMap = m_AllConnectionMap;

    // Add shared info
    for (std::map<SString, CDatabaseConnection*>::iterator iter = m_SharedConnectionMap.begin(); iter != m_SharedConnectionMap.end(); ++iter)
    {
        const SString&       strShareKey = iter->first.TrimStart(strModPath);
        CDatabaseConnection* pConnection = iter->second;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton()->UpdateLine(m_strStatsKeyHead + SString("%d", iIndex++), 0, "Database connection: sqlite (shared)", *strShareKey,
                                                        *SString("Share count: %d", pConnection->GetShareCount()), NULL);

        // Update unshared map for the second part
        MapRemove(unsharedConnectionMap, pConnection);
    }

    // Add unshared info
    for (std::set<CDatabaseConnection*>::iterator iter = unsharedConnectionMap.begin(); iter != unsharedConnectionMap.end(); ++iter)
    {
        CDatabaseConnection* pConnection = *iter;

        // Add new line with this key
        CPerfStatDebugTable::GetSingleton()->UpdateLine(m_strStatsKeyHead + SString("%d", iIndex++), 0, "Database connection: sqlite (unshared)",
                                                        *pConnection->m_strOtherTag, *SString("Refs: %d", pConnection->GetShareCount()), NULL);
    }
}
