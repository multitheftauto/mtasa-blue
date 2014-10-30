/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CBassManager.h
*  PURPOSE:     core based manager class header
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#ifndef __CBASSMANAGER_H
#define __CBASSMANAGER_H

#include "CBassAudio.h"
#include <core/CBassManagerInterface.h>

class CBassManager : public CBassManagerInterface
{
public:
                    CBassManager                ( void );

    void            SetUserAgent                ( SString strConnectedServer );
    
    void            DoPulse                     ( void );

    void            SetCreditsMusicPaused       ( bool bPaused );
    bool            IsCreditsMusicPaused        ( void );

    void            SetCreditsOpen              ( bool bOpen );

private:
    SString         m_strUserAgent;
    CBassAudio *    m_pCreditsStream;
};

#endif