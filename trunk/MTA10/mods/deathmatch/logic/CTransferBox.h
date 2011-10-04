/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTransferBox.h
*  PURPOSE:     Header for transfer box class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTRANSFERBOX_H
#define __CTRANSFERBOX_H

#define TRANSFERBOX_FRAMES  10
#define TRANSFERBOX_DELAY   50

#include "CClientCommon.h"
#include <gui/CGUI.h>

class CTransferBox
{
public:
                                        CTransferBox                            ( void );
    virtual                             ~CTransferBox                           ( void );

    void                                Show                                    ( void );
    void                                Hide                                    ( void );

    void                                SetInfo                                 ( double dDownloadSizeNow, const char* szMessage = NULL );

    void                                DoPulse                                 ( void );

    bool                                OnCancelClick                           ( CGUIElement* pElement );

    inline bool                         IsVisible                               ( void )            { return m_pWindow->IsVisible (); };

    inline void                         AddToTotalSize                          ( double dSize )    { m_dTotalSize += dSize; };

private:
    CGUIWindow*                         m_pWindow;
    CGUIStaticImage*                    m_pIcon[TRANSFERBOX_FRAMES];
    CGUILabel*                          m_pInfo;
    CGUIProgressBar*                    m_pProgress;

    bool                                m_bMultipleDownloads;

    unsigned int                        m_uiVisible;
    unsigned long                       m_ulTime;
    double                              m_dTotalSize;
};

#endif
