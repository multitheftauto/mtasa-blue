//
// CRemoteMasterServer.h
//

//
// Communicate with a remote master server and parse the result.
//

class CRemoteMasterServerInterface
{
public:
    virtual                 ~CRemoteMasterServerInterface  ( void ) {}

    // CRemoteMasterServerInterface
    virtual void            Refresh                     ( void ) = 0;
    virtual bool            HasData                     ( void ) = 0;
    virtual bool            ParseList                   ( CServerListItemList& itemList ) = 0;
};

CRemoteMasterServerInterface* NewRemoteMasterServer ( const SString& strURL );

