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

//#define TEST_URL ""

class CBassManager
{
public:
                    CBassManager    ( );

    void            SetUserAgent    ( SString strConnectedServer );
    
    void            DoPulse         ( );
private:
    SString         m_strUserAgent;
    CBassAudio *    m_pTestStream;
};

#endif