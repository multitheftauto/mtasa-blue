/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerDisconnectedPacket.h
 *  PURPOSE:     Player disconnected packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerDisconnectedPacket final : public CPacket
{
public:
    enum ePlayerDisconnectType
    {
        NO_REASON,
        INVALID_PASSWORD,
        INVALID_NICKNAME,
        BANNED_SERIAL,
        BANNED_IP,
        BANNED_ACCOUNT,
        VERSION_MISMATCH,
        JOIN_FLOOD,
        INCORRECT_PASSWORD,
        DIFFERENT_BRANCH,
        BAD_VERSION,
        SERVER_NEWER,
        SERVER_OLDER,
        NICK_CLASH,
        ELEMENT_FAILURE,
        GENERAL_REFUSED,
        SERIAL_VERIFICATION,
        CONNECTION_DESYNC,
        BAN,
        KICK,
        CUSTOM,
        SHUTDOWN,
        SERIAL_DUPLICATE
    };

    CPlayerDisconnectedPacket(const char* szReason);
    CPlayerDisconnectedPacket(CPlayerDisconnectedPacket::ePlayerDisconnectType eType, const char* szReason = "");
    CPlayerDisconnectedPacket(CPlayerDisconnectedPacket::ePlayerDisconnectType eType, time_t BanDuration = 0, const char* szReason = "");

    ePacketID     GetPacketID() const { return PACKET_ID_SERVER_DISCONNECTED; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    const char* GetReason() { return m_strReason; }
    void        SetReason(const char* szReason) { m_strReason = szReason; }

private:
    SString               m_strReason;
    ePlayerDisconnectType m_eType;
    time_t                m_Duration;
};
