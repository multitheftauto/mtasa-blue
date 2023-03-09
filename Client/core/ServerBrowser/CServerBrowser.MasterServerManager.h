//
// CServerBrowser.MasterServerManager.h
//

class CMasterServerManagerInterface
{
public:
    virtual ~CMasterServerManagerInterface() {}

    // CMasterServerManagerInterface
    virtual void Refresh() = 0;
    virtual bool HasData() = 0;
    virtual bool ParseList(CServerListItemList& itemList) = 0;
};

CMasterServerManagerInterface* NewMasterServerManager();
