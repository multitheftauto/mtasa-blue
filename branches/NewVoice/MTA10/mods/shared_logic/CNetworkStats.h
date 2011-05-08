/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CNetworkStats.h
*  PURPOSE:     Network stats class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#pragma error("Deprecated\n");

#ifndef __CNETWORKSTATUS_H
#define __CNETWORKSTATUS_H

#include <net/cnet.h>

class CNetworkStats
{
    public:
                            CNetworkStats                   ( void );
                            ~CNetworkStats                  ( void );

    void                    ReadStatusPacket                ( NetBitStreamInterface& bitStream );
    void                    WriteStatusPacket               ( NetBitStreamInterface& bitStream );

    float                   GetAveragePacketLoss            ( void );
    unsigned int            GetBytesReceived                ( void );
    unsigned int            GetBytesSent                    ( void );
    unsigned int            GetRemotelyBytesReceived        ( void );
    unsigned int            GetRemotelyBytesSent            ( void );

    private:
    unsigned int            m_ullBytesReceived;
    unsigned int            m_ullBytesSent;
    unsigned int            m_ullRemoteBytesReceived;
    unsigned int            m_ullRemoteBytesSent;
};

#endif
