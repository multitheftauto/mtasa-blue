/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerJoinCompletePacket.h
*  PURPOSE:     Player join completion packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERJOINCOMPLETEPACKET_H
#define __PACKETS_CPLAYERJOINCOMPLETEPACKET_H

#include "../CCommon.h"
#include "CPacket.h"
#include <string.h>

class CPlayerJoinCompletePacket : public CPacket
{
public:
                            CPlayerJoinCompletePacket       ( void );
                            CPlayerJoinCompletePacket       ( ElementID PlayerID,
                                                              unsigned char ucNumberOfPlayers,
                                                              ElementID RootElementID,
                                                              eHTTPDownloadType ucHTTPDownloadType,
                                                              unsigned short usHTTPDownloadPort,
                                                              const char* szHTTPDownloadURL,
                                                              int iHTTPMaxConnectionsPerClient,
                                                              int iEnableClientChecks, 
                                                              bool bVoiceEnabled, 
                                                              unsigned char ucSampleRate,
                                                              unsigned char ucVoiceQuality,
                                                              unsigned int uiBitrate );

    inline ePacketID        GetPacketID             ( void ) const      { return PACKET_ID_SERVER_JOINEDGAME; };
    inline unsigned long    GetFlags                ( void ) const      { return PACKET_RELIABLE | PACKET_SEQUENCED | PACKET_HIGH_PRIORITY; };

    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;

private:
    ElementID               m_PlayerID;
    unsigned char           m_ucNumberOfPlayers;
    ElementID               m_RootElementID;
    eHTTPDownloadType       m_ucHTTPDownloadType;
    unsigned short          m_usHTTPDownloadPort;
    char                    m_szHTTPDownloadURL [MAX_HTTP_DOWNLOAD_URL + 1];
    int                     m_iHTTPMaxConnectionsPerClient;
    int                     m_iEnableClientChecks;
    bool                    m_bVoiceEnabled;
    unsigned char           m_ucSampleRate;
    unsigned char           m_ucQuality;
    unsigned int            m_uiBitrate;
};

#endif
