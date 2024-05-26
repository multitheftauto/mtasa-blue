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
    virtual bool  RequiresSourcePlayer() const noexcept { return false; }
    ePacketID     GetPacketID() const noexcept { return static_cast<ePacketID>(PACKET_ID_PLAYER_JOINDATA); }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;

    std::uint16_t GetNetVersion() const noexcept { return m_usNetVersion; }
    std::uint8_t  GetGameVersion() const noexcept { return m_ucGameVersion; }

    void SetNetVersion(const std::uint16_t usNetVersion) noexcept { m_usNetVersion = usNetVersion; }
    void SetGameVersion(const std::uint8_t ucGameVersion) noexcept { m_ucGameVersion = ucGameVersion; }

    std::uint16_t      GetMTAVersion() const noexcept { return m_usMTAVersion; }
    std::uint16_t      GetBitStreamVersion() const noexcept { return m_usBitStreamVersion; }
    const CMtaVersion& GetPlayerVersion() const noexcept { return m_strPlayerVersion; }

    const char* GetNick() const noexcept { return m_strNick; }
    void        SetNick(const char* szNick) noexcept {
        m_strNick.AssignLeft(szNick, MAX_PLAYER_NICK_LENGTH);
    }

    const MD5& GetPassword() const noexcept { return m_Password; }
    void       SetPassword(const MD5& Password) noexcept { m_Password = Password; }

    const char* GetSerialUser() const noexcept { return m_strSerialUser; }
    void        SetSerialUser(const char* szSerialUser) noexcept {
        m_strSerialUser.AssignLeft(szSerialUser, MAX_SERIAL_LENGTH);
    }

    bool IsOptionalUpdateInfoRequired() const noexcept { return m_bOptionalUpdateInfoRequired; }

private:
    std::uint16_t m_usNetVersion;
    std::uint16_t m_usMTAVersion;
    std::uint16_t m_usBitStreamVersion;
    std::uint8_t  m_ucGameVersion;
    bool          m_bOptionalUpdateInfoRequired;
    SString       m_strNick;
    MD5           m_Password;
    SString       m_strSerialUser;
    CMtaVersion   m_strPlayerVersion;
};
