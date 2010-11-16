//
// CMasterServerManager.cpp
//

#include "StdInc.h"
#include "CServerBrowser.MasterServerManager.h"
#include "CServerBrowser.RemoteMasterServer.h"


///////////////////////////////////////////////////////////////
//
//
// CMasterServerManager class
//
//
///////////////////////////////////////////////////////////////
class CMasterServerManager : public CMasterServerManagerInterface
{
public:
    ZERO_ON_NEW
                            CMasterServerManager        ( void );
                            ~CMasterServerManager       ( void );

    // CMasterServerManagerInterface
    virtual void            Refresh                     ( void );
    virtual bool            HasData                     ( void );
    virtual bool            ParseList                   ( std::list < CServerListItem* >& itemList );

    // CMasterServerManager
protected:

    long long                                       m_llStartTime;
    std::vector < CRemoteMasterServerInterface* >   m_MasterServerList;
};


///////////////////////////////////////////////////////////////
//
// CMasterServerManager instantiation
//
//
//
///////////////////////////////////////////////////////////////
CMasterServerManagerInterface* NewMasterServerManager ( void )
{
    return new CMasterServerManager ();
}


///////////////////////////////////////////////////////////////
//
// CMasterServerManager implementation
//
//
//
///////////////////////////////////////////////////////////////
CMasterServerManager::CMasterServerManager ( void )
{
    m_MasterServerList.push_back ( NewRemoteMasterServer ( "http://nightly.mtasa.com/ase/mta-1.0/" ) );
    //m_MasterServerList.push_back ( NewRemoteMasterServer ( "http://1mgg.com/affil/mta-1.0" ) );
}


///////////////////////////////////////////////////////////////
//
// CMasterServerManager::~CMasterServerManager
//
//
//
///////////////////////////////////////////////////////////////
CMasterServerManager::~CMasterServerManager ( void )
{
    for ( uint i = 0 ; i < m_MasterServerList.size () ; i++ )
        delete m_MasterServerList[i];

    m_MasterServerList.clear ();
}


///////////////////////////////////////////////////////////////
//
// CMasterServerManager::Refresh
//
//
//
///////////////////////////////////////////////////////////////
void CMasterServerManager::Refresh ( void )
{
    // Pass on refresh request
    for ( uint i = 0 ; i < m_MasterServerList.size () ; i++ )
        m_MasterServerList[i]->Refresh ();

    m_llStartTime = GetTickCount64_ ();
}


///////////////////////////////////////////////////////////////
//
// CMasterServerManager::HasData
//
//
//
///////////////////////////////////////////////////////////////
bool CMasterServerManager::HasData ( void )
{
    // Count how many server have responded
    uint uiHasDataCount = 0;

    for ( uint i = 0 ; i < m_MasterServerList.size () ; i++ )
        if ( m_MasterServerList[i]->HasData () )
            uiHasDataCount++;

    // If two servers responded, then success
    if ( uiHasDataCount >= 2 )
        return true;

    // If one server responded, and it's been 4 seconds, then success
    if ( uiHasDataCount >= 1 && GetTickCount64_ () - m_llStartTime > 4000 )
        return true;

    return false;
}


///////////////////////////////////////////////////////////////
//
// CMasterServerManager::ParseList
//
//
//
///////////////////////////////////////////////////////////////
bool CMasterServerManager::ParseList ( std::list < CServerListItem* >& itemList )
{
    uint uiParsedCount = 0;

    for ( uint i = 0 ; i < m_MasterServerList.size () ; i++ )
        if ( m_MasterServerList[i]->HasData () )
            if ( m_MasterServerList[i]->ParseList ( itemList ) )
                uiParsedCount++;

    return uiParsedCount > 0;
}
