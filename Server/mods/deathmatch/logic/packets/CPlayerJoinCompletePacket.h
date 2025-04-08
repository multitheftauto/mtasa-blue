/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerJoinCompletePacket.h
 *  PURPOSE:     Player join completion packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"
#include <string.h>

class CPlayerJoinCompletePacket final : public CPacket
{
public:
    CPlayerJoinCompletePacket();
    CPlayerJoinCompletePacket(ElementID PlayerID, ElementID RootElementID, eHTTPDownloadType ucHTTPDownloadType, unsigned short usHTTPDownloadPort,
                              const char* szHTTPDownloadURL, int iHTTPMaxConnectionsPerClient, int iEnableClientChecks, bool bVoiceEnabled,
                              unsigned char ucSampleRate, unsigned char ucVoiceQuality, unsigned int uiBitrate, const char* szServerName);

    ePacketID     GetPacketID() const { return PACKET_ID_SERVER_JOINEDGAME; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    ElementID         m_PlayerID;
    ElementID         m_RootElementID;
    eHTTPDownloadType m_ucHTTPDownloadType;
    unsigned short    m_usHTTPDownloadPort;
    SString           m_strHTTPDownloadURL;
    int               m_iHTTPMaxConnectionsPerClient;
    int               m_iEnableClientChecks;
    bool              m_bVoiceEnabled;
    unsigned char     m_ucSampleRate;
    unsigned char     m_ucQuality;
    unsigned int      m_uiBitrate;
    const char*       m_szServerName;
};
