/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVersionUpdater.h
*  PURPOSE:     Header file for version update check and message dialog
*  DEVELOPERS:  ccccccccccccccccccccccccccccw
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVERSIONUPDATER_H
#define __CVERSIONUPDATER_H


class CVersionUpdater
{
public:
                            CVersionUpdater         ( void );
    void                    DoPulse                 ( void );

protected:
    void                    ResetCheckTimer         ( void );
    bool                    IsDownloadMandatory     ( void );
    CQuestionBox&           GetQuestionBox          ( void );

    void                    StartVersionCheck       ( void );
    void                    PollVersionCheck        ( void );
    void                    StartUpdateQuestion     ( void );
    void                    PollUpdateQuestion      ( void );
    void                    StartDownload           ( void );
    void                    PollDownload            ( void );
    void                    PollPostDownloadError   ( void );
    void                    PollPostDownloadOk      ( void );

    SString                 m_strStage;
    CHTTPClient             m_HTTP;
    long long               m_llTimeStart;


    struct CCheckInfo
    {
                                CCheckInfo() : iCurrent ( -1 ), iRetryCount ( 0 ) {}
        int                     iCurrent;
        int                     iRetryCount;
        std::vector < SString > serverList;
    } m_CheckInfo;

    struct CDownloadInfo
    {
                                CDownloadInfo() : iCurrent ( -1 ), iRetryCount ( 0 ), uiFilesize ( 0 ), uiBytesDownloaded ( 0 ) {}
        int                     iCurrent;
        int                     iRetryCount;
        SString                 strStatus;
        SString                 strTitle;
        SString                 strMsg;
        SString                 strMsg2;
        SString                 strYes;
        SString                 strNo;
        SString                 strPriority;
        SString                 strFilename;
        unsigned int            uiFilesize;
        std::vector < SString > serverList;
        SString                 strSHA1;
        SString                 strEnd;
        unsigned int            uiBytesDownloaded;
    } m_DownloadInfo;

};

#endif
