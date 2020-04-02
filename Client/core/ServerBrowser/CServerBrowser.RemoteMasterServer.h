//
// CRemoteMasterServer.h
//

//
// Communicate with a remote master server and parse the result.
//

class CRemoteMasterServerInterface : public CRefCountable
{
protected:
    virtual ~CRemoteMasterServerInterface() {}

public:
    // CRemoteMasterServerInterface
    virtual void Refresh() = 0;
    virtual bool HasData() = 0;
    virtual bool ParseList(CServerListItemList& itemList) = 0;
};

CRemoteMasterServerInterface* NewRemoteMasterServer(const SString& strURL);
