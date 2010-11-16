//
// CServerBrowser.MasterServerManager.h
//

class CMasterServerManagerInterface
{
public:
    virtual                 ~CMasterServerManagerInterface  ( void ) {}

    // CMasterServerManagerInterface
    virtual void            Refresh                     ( void ) = 0;
    virtual bool            HasData                     ( void ) = 0;
    virtual bool            ParseList                   ( std::list < CServerListItem* >& itemList ) = 0;
};

CMasterServerManagerInterface* NewMasterServerManager ( void );
