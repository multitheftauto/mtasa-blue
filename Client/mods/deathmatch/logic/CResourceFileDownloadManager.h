/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceFileDownloadManager.h
*  PURPOSE:     Resource object class
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct SHttpServerInfo
{
    bool                bEnabled;
    SString             strUrl;
    EDownloadModeType   downloadChannel;
    uint                uiConnectionAttempts;
    uint                uiConnectTimeoutMs;
};

class CResourceFileDownloadManager
{
public:
    ZERO_ON_NEW
    void                    AddServer                       ( const SString& strUrl, int iMaxConnectionsPerClient, EDownloadModeType downloadChannel, uint uiConnectionAttempts, uint uiConnectTimeoutMs );
    void                    AddPendingFileDownload          ( CDownloadableResource* pDownloadableResource );
    void                    UpdatePendingDownloads          ( void );
    void                    OnRemoveResourceFile            ( CDownloadableResource* pResourceFile );
    void                    DoPulse                         ( void );
    bool                    IsTransferringInitialFiles      ( void )                        { return m_bIsTransferingFiles; }

protected:
    CTransferBox*           GetTransferBox                  ( void )                        { return g_pClientGame->GetTransferBox(); };
    void                    AddDownloadSize                 ( int iSize );
    bool                    DisableHttpServer               ( uint uiHttpServerIndex );
    bool                    BeginResourceFileDownload       ( CDownloadableResource* pDownloadableResource, uint uiHttpServerIndex );
    static void             StaticDownloadFinished          ( char* pCompletedData, size_t completedLength, void *pObj, bool bSuccess, int iErrorCode );
    void                    DownloadFinished                ( char* pCompletedData, size_t completedLength, void *pObj, bool bSuccess, int iErrorCode );
    SString*                MakeDownloadContextString       ( CDownloadableResource* pDownloadableResource );
    CDownloadableResource*  ResolveDownloadContextString    ( SString* pString );

    std::vector < CDownloadableResource* >      m_PendingFileDownloadList;
    std::vector < CDownloadableResource* >      m_ActiveFileDownloadList;
    std::vector < SHttpServerInfo >             m_HttpServerList;
    bool                                        m_bIsTransferingFiles;
    SString                                     m_strLastHTTPError;
};
