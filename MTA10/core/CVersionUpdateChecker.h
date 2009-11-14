/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVersionUpdateChecker.h
*  PURPOSE:     Header file for version update check and message dialog
*  DEVELOPERS:  Jesus
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVERSIONUPDATECHECKER_H
#define __CVERSIONUPDATECHECKER_H

class CVersionUpdateChecker
{
public:
                            CVersionUpdateChecker   ( void );
    void                    DoPulse                 ( void );

private:
    void                    StartVersionCheck       ( void );

    int                     m_iStage;
    CHTTPClient             m_HTTP;
};

#endif
