/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CNetworkStats.h
*  PURPOSE:     Header for network stats class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNETWORKSTATS_H
#define __CNETWORKSTATS_H

#include "CClientDisplayManager.h"

class CNetworkStats
{
public:
                                    CNetworkStats               ( CClientDisplayManager* pDisplayManager );

    void                            Draw                        ( void );
    void                            Reset                       ( void );

private:
    void                            Update                      ( void );

    CClientDisplayManager*          m_pDisplayManager;

    unsigned long                   m_ulLastUpdateTime;
    unsigned int                    m_uiPacketsSent;
    unsigned int                    m_uiPacketsReceived;
    unsigned int                    m_uiBitsReceived;
    unsigned int                    m_uiBitsSent;

    unsigned int                    m_uiLastPacketsSent;
    unsigned int                    m_uiLastPacketsReceived;
    unsigned int                    m_uiLastBitsReceived;
    unsigned int                    m_uiLastBitsSent;
};

#endif
