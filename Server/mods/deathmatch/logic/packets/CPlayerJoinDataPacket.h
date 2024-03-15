/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerJoinDataPacket.h
 *  PURPOSE:     Player join data packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../../Config.h"

class CPlayerJoinDataPacket final : public CPacket
{
public:
    virtual bool  RequiresSourcePlayer() const { return false; }
    ePacketID     GetPacketID() const { return static_cast<ePacketID>(PACKET_ID_PLAYER_JOINDATA); };
    std::uint32_t GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);

    std::uint16_t GetNetVersion() { return m_usNetVersion; };
    std::uint8_t  GetGameVersion() { return m_ucGameVersion; };

    void SetNetVersion(std::uint16_t usNetVersion) { m_usNetVersion = usNetVersion; };
    void SetGameVersion(std::uint8_t ucGameVersion) { m_ucGameVersion = ucGameVersion; };

    std::uint16_t     GetMTAVersion() { return m_usMTAVersion; };
    std::uint16_t     GetBitStreamVersion() { return m_usBitStreamVersion; };
    const CMtaVersion& GetPlayerVersion() { return m_strPlayerVersion; };

    const char* GetNick() { return m_strNick; };
    void        SetNick(const char* szNick) { m_strNick.AssignLeft(szNick, MAX_PLAYER_NICK_LENGTH); };

    const MD5& GetPassword() { return m_Password; };
    void       SetPassword(const MD5& Password) { m_Password = Password; };

    const char* GetSerialUser() { return m_strSerialUser; }
    void        SetSerialUser(const char* szSerialUser) { m_strSerialUser.AssignLeft(szSerialUser, MAX_SERIAL_LENGTH); }

    bool IsOptionalUpdateInfoRequired() { return m_bOptionalUpdateInfoRequired; }

private:
    std::uint16_t m_usNetVersion;
    std::uint16_t m_usMTAVersion;
    std::uint16_t m_usBitStreamVersion;
    std::uint8_t  m_ucGameVersion;
    bool           m_bOptionalUpdateInfoRequired;
    SString        m_strNick;
    MD5            m_Password;
    SString        m_strSerialUser;
    CMtaVersion    m_strPlayerVersion;
};
