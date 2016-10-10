/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CServerIdManagerImpl.cpp
*  PURPOSE:
*  DEVELOPERS:
*
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CServerIdManager.h"

#define MTA_SERVERID_LOOKUP_DIR       "priv"
#define MTA_SERVERID_LOOKUP_XML       "priv/server-ids.xml"

namespace
{
    struct CServerIdKey
    {
        SString         strId;          // Server id
        bool operator < ( const CServerIdKey& other ) const { return strId < other.strId; }
    };

    struct CServerIdInfo
    {
        SString         strDir;         // Server private directory
    };


    // Variables used for saving the file on a separate thread
    static bool ms_bIsSaving = false;
    static std::map < CServerIdKey, CServerIdInfo > ms_ServerIdMap;


    // Make unique directory helper
    SString IncrementCounter ( const SString& strText )
    {
        SString strMain, strCount;
        strText.Split ( "_", &strMain, &strCount );
        return SString ( "%s_%d", *strMain, atoi ( strCount ) + 1 );
    }
}


///////////////////////////////////////////////////////////////
//
//
// CServerIdManagerImpl
//
//
///////////////////////////////////////////////////////////////
class CServerIdManagerImpl : public CServerIdManager
{
public:
    ZERO_ON_NEW

    virtual SString         GetConnectionPrivateDirectory   ( bool bPreviousVer );

                            CServerIdManagerImpl            ( void );
                            ~CServerIdManagerImpl           ( void );
protected:
    void                    SaveServerIdMap                 ( bool bWait = false );
    const CServerIdInfo&    GetServerIdInfo                 ( const SString& strServerId );
    bool                    LoadServerIdMap                 ( void );
    static DWORD            StaticThreadProc                ( LPVOID lpdwThreadParam );
    static void             StaticSaveServerIdMap           ( void );

    bool                                        m_bListChanged;
    bool                                        m_bClearedDefaultDirectory;
    std::map < CServerIdKey, CServerIdInfo >    m_ServerIdMap;
    SString                                     m_strServerIdLookupBaseDir;
    SString                                     m_strTempErrorDir;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CServerIdManagerImpl* g_pServerIdManager = NULL;

CServerIdManager* CServerIdManager::GetSingleton ()
{
    if ( !g_pServerIdManager )
        g_pServerIdManager = new CServerIdManagerImpl ();
    return g_pServerIdManager;
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::CServerIdManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CServerIdManagerImpl::CServerIdManagerImpl ( void )
{
    // Calc private dir root
    m_strServerIdLookupBaseDir = PathJoin ( g_pClientGame->GetFileCacheRoot(), MTA_SERVERID_LOOKUP_DIR );
    MakeSureDirExists ( PathJoin ( m_strServerIdLookupBaseDir, "" ) );

    // Calc temp dir path incase of server id error
    m_strTempErrorDir = PathJoin ( m_strServerIdLookupBaseDir, "_error" );

    // If temp dir has been used, clean it
    if ( DirectoryExists ( m_strTempErrorDir ) )
        DelTree ( m_strTempErrorDir, m_strServerIdLookupBaseDir );

    LoadServerIdMap ();
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::~CServerIdManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CServerIdManagerImpl::~CServerIdManagerImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::LoadServerIdMap
//
// Load server id data from xml file
//
///////////////////////////////////////////////////////////////
bool CServerIdManagerImpl::LoadServerIdMap ( void )
{
    // Load config XML file
    CXMLFile* pConfigFile = g_pCore->GetXML ()->CreateXML ( PathJoin ( g_pClientGame->GetFileCacheRoot(), MTA_SERVERID_LOOKUP_XML ) );
    if ( !pConfigFile )
        return false;
    pConfigFile->Parse ();

    CXMLNode* pRoot = pConfigFile->GetRootNode ();
    if ( !pRoot )
        pRoot = pConfigFile->CreateRootNode ( "root" );

    m_ServerIdMap.clear ();

    // Read each node
    for ( uint i = 0 ; i < pRoot->GetSubNodeCount () ; i++ )
    {
        CXMLNode* pSubNode = pRoot->GetSubNode ( i );

        CServerIdKey key;
        CServerIdInfo info;
        key.strId = pSubNode->GetTagContent ();
        if ( CXMLAttribute* pAttribute = pSubNode->GetAttributes().Find ( "dir" ) )
            info.strDir = pAttribute->GetValue ();

        if ( !info.strDir.empty () )
            MapSet ( m_ServerIdMap, key, info );
    }

    // Maybe one day remove unwanted directories

    delete pConfigFile;
    return true;
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::SaveServerIdMap
//
// Save server id data to xml file
//
///////////////////////////////////////////////////////////////
void CServerIdManagerImpl::SaveServerIdMap ( bool bWait )
{
    // Check if need to save
    if ( !m_bListChanged )
        return;

    // Check if can save
    while ( ms_bIsSaving )
        Sleep ( 10 );

    m_bListChanged = false;

    // Copy vars for save thread
    ms_ServerIdMap = m_ServerIdMap;

    // Start save thread
    HANDLE hThread = CreateThread ( NULL, 0, (LPTHREAD_START_ROUTINE)CServerIdManagerImpl::StaticThreadProc, NULL, CREATE_SUSPENDED, NULL );
    if ( !hThread )
    {
        g_pCore->GetConsole ()->Printf ( "Could not create server-ids save thread." );
    }
    else
    {
        ms_bIsSaving = true;
        SetThreadPriority ( hThread, THREAD_PRIORITY_LOWEST );
        ResumeThread ( hThread );
    }

    // Wait for save to complete if required
    while ( bWait && ms_bIsSaving )
        Sleep ( 10 );
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::StaticThreadProc
//
// SaveServerIdMap thread
//
///////////////////////////////////////////////////////////////
DWORD CServerIdManagerImpl::StaticThreadProc ( LPVOID lpdwThreadParam )
{
    StaticSaveServerIdMap ();
    ms_bIsSaving = false;
    return 0;
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::StaticSaveServerIdMap
//
//
//
///////////////////////////////////////////////////////////////
void CServerIdManagerImpl::StaticSaveServerIdMap ( void )
{
    CXMLFile* pConfigFile = g_pCore->GetXML ()->CreateXML ( PathJoin ( g_pClientGame->GetFileCacheRoot(), MTA_SERVERID_LOOKUP_XML ) );
    if ( !pConfigFile )
        return;
    pConfigFile->Reset ();

    CXMLNode* pRoot = pConfigFile->GetRootNode ();
    if ( !pRoot )
        pRoot = pConfigFile->CreateRootNode ( "root" );

    // Transfer each item from m_ServerIdMap into the file
    for ( std::map < CServerIdKey, CServerIdInfo >::iterator it = ms_ServerIdMap.begin () ; it != ms_ServerIdMap.end () ; ++it )
    {
        const SString& strId = it->first.strId;
        const SString& strDir = it->second.strDir;

        CXMLNode* pSubNode = pRoot->CreateSubNode ( "id" );
        pSubNode->SetTagContent ( strId );
        pSubNode->GetAttributes().Create ( "dir" )->SetValue ( strDir );
    }

    pConfigFile->Write ();
    delete pConfigFile;
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::GetConnectionPrivateDirectory
//
//
//
///////////////////////////////////////////////////////////////
SString CServerIdManagerImpl::GetConnectionPrivateDirectory ( bool bPreviousVer )
{
    // Get ServerId for this connection
    SString strServerId = g_pCore->GetNetwork ()->GetCurrentServerId ( bPreviousVer );

    // If ServerId is invalid, use the temp dir
    if ( strServerId.length () < 10 )
        return bPreviousVer ? "" : m_strTempErrorDir;

    // Otherwise fetch the server unique dir
    const CServerIdInfo& info = GetServerIdInfo ( strServerId );
    return PathJoin ( m_strServerIdLookupBaseDir, info.strDir );
}


///////////////////////////////////////////////////////////////
//
// CServerIdManagerImpl::GetServerIdInfo
//
//
//
///////////////////////////////////////////////////////////////
const CServerIdInfo& CServerIdManagerImpl::GetServerIdInfo ( const SString& strServerId )
{
    // Find
    CServerIdKey findKey;
    findKey.strId = strServerId;
    CServerIdInfo* pInfo = MapFind ( m_ServerIdMap, findKey );
    if ( !pInfo )
    {
        // Add if missing

        // Use part of serverid for directory name
        SString strDir = strServerId.Left ( 6 );

        // Ensure the item does not exist in the map
        bool bAgain = true;
        while ( bAgain )
        {
            bAgain = false;
            for ( std::map < CServerIdKey, CServerIdInfo >::iterator it = m_ServerIdMap.begin () ; it != m_ServerIdMap.end () ; ++it )
            {
                const CServerIdInfo& info = it->second;
                if ( strDir == info.strDir )
                {
                    strDir = IncrementCounter ( strDir );
                    bAgain = true;
                    break;
                }
            }
        }

        // Ensure the directory does not exist
        while ( DirectoryExists ( PathJoin ( m_strServerIdLookupBaseDir, strDir ) ) )
        {
            strDir = IncrementCounter ( strDir );
        }

        // Add new item
        {
            CServerIdInfo info;
            info.strDir = strDir;
            MapSet ( m_ServerIdMap, findKey, info );

            m_bListChanged = true;
            SaveServerIdMap ();
        }

        pInfo = MapFind ( m_ServerIdMap, findKey ); 
    }

    return *pInfo;
}
