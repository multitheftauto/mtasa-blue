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
    CMasterServerManager();
    ~CMasterServerManager();

    // CMasterServerManagerInterface
    virtual void Refresh();
    virtual bool HasData();
    virtual bool ParseList(CServerListItemList& itemList);

    // CMasterServerManager
protected:
    CElapsedTime                               m_ElapsedTime;
    std::vector<CRemoteMasterServerInterface*> m_MasterServerList;
    uint                                       m_iActiveAmount;
};

///////////////////////////////////////////////////////////////
//
// CMasterServerManager instantiation
//
//
//
///////////////////////////////////////////////////////////////
CMasterServerManagerInterface* NewMasterServerManager()
{
    return new CMasterServerManager();
}

///////////////////////////////////////////////////////////////
//
// CMasterServerManager implementation
//
//
//
///////////////////////////////////////////////////////////////
CMasterServerManager::CMasterServerManager()
{
    m_ElapsedTime.SetMaxIncrement(500);
}

///////////////////////////////////////////////////////////////
//
// CMasterServerManager::~CMasterServerManager
//
//
//
///////////////////////////////////////////////////////////////
CMasterServerManager::~CMasterServerManager()
{
    for (uint i = 0; i < m_MasterServerList.size(); i++)
        SAFE_RELEASE(m_MasterServerList[i]);

    m_MasterServerList.clear();
}

///////////////////////////////////////////////////////////////
//
// CMasterServerManager::Refresh
//
//
//
///////////////////////////////////////////////////////////////
void CMasterServerManager::Refresh()
{
    // Create master server list if required
    if (m_MasterServerList.empty())
    {
        std::vector<SString> resultList;
        GetVersionUpdater()->GetAseServerList(resultList);

        for (uint i = 0; i < resultList.size(); i++)
            m_MasterServerList.push_back(NewRemoteMasterServer(resultList[i]));
    }

    // Pass on refresh request to first two servers
    m_iActiveAmount = std::min<uint>(2, m_MasterServerList.size());
    for (uint i = 0; i < m_MasterServerList.size() && i < m_iActiveAmount; i++)
        m_MasterServerList[i]->Refresh();

    m_ElapsedTime.Reset();
}

///////////////////////////////////////////////////////////////
//
// CMasterServerManager::HasData
//
//
//
///////////////////////////////////////////////////////////////
bool CMasterServerManager::HasData()
{
    // Count how many server have responded
    uint uiHasDataCount = 0;

    for (uint i = 0; i < m_MasterServerList.size() && i < m_iActiveAmount; i++)
        if (m_MasterServerList[i]->HasData())
            uiHasDataCount++;

    // If two servers responded, then success
    if (uiHasDataCount >= 2)
        return true;

    // If less than 2 servers responded, and it's been 2.5 seconds, try to add a new server
    if (uiHasDataCount < 2 && m_ElapsedTime.Get() > 2500)
    {
        if (m_iActiveAmount <= 2 && m_MasterServerList.size() > m_iActiveAmount)
        {
            m_MasterServerList[m_iActiveAmount++]->Refresh();
        }
    }

    // If one server responded, and it's been 5 seconds, then success
    if (uiHasDataCount >= 1 && m_ElapsedTime.Get() > 5000)
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
bool CMasterServerManager::ParseList(CServerListItemList& itemList)
{
    uint uiParsedCount = 0;

    for (uint i = 0; i < m_MasterServerList.size() && i < m_iActiveAmount; i++)
        if (m_MasterServerList[i]->HasData())
            if (m_MasterServerList[i]->ParseList(itemList))
                uiParsedCount++;

    return uiParsedCount > 0;
}
