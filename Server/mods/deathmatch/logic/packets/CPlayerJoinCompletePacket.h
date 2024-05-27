/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerJoinCompletePacket.h
 *  PURPOSE:     Player join completion packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"
#include <string.h>

class CPlayerJoinCompletePacket final : public CPacket
{
public:
    CPlayerJoinCompletePacket() noexcept;
    CPlayerJoinCompletePacket(ElementID PlayerID, ElementID RootElementID, eHTTPDownloadType ucHTTPDownloadType, std::uint16_t usHTTPDownloadPort,
                              const char* szHTTPDownloadURL, int iHTTPMaxConnectionsPerClient, int iEnableClientChecks, bool bVoiceEnabled,
                              std::uint8_t ucSampleRate, std::uint8_t ucVoiceQuality, std::uint32_t uiBitrate, const char* szServerName) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_SERVER_JOINEDGAME; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    ElementID         m_PlayerID;
    ElementID         m_RootElementID;
    eHTTPDownloadType m_ucHTTPDownloadType;
    std::uint16_t    m_usHTTPDownloadPort;
    SString           m_strHTTPDownloadURL;
    int               m_iHTTPMaxConnectionsPerClient;
    int               m_iEnableClientChecks;
    bool              m_bVoiceEnabled;
    std::uint8_t     m_ucSampleRate;
    std::uint8_t     m_ucQuality;
    std::uint32_t      m_uiBitrate;
    const char*       m_szServerName;
};
