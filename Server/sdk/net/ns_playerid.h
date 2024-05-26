/*************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/net/ns_playerid.h
 *  PURPOSE:     Net server player ID interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *************************************************************************/

#pragma once

#include <MTAPlatform.h>
#include <cstdint>

class SNetExtraInfo : public CRefCountable
{
protected:
    SNetExtraInfo(const SNetExtraInfo&) noexcept;
    ~SNetExtraInfo() noexcept {}

    const SNetExtraInfo& operator=(const SNetExtraInfo&) noexcept;
public:
    ZERO_ON_NEW
    constexpr SNetExtraInfo() noexcept {}

    bool m_bHasPing{ false };
    std::uint32_t m_uiPing{0};
};

class NetServerPlayerID
{
protected:
    std::uint32_t m_uiBinaryAddress;
    std::uint16_t m_usPort;

public:
    constexpr NetServerPlayerID(
        const std::uint32_t uiBinaryAddress = -1,
        const std::uint16_t usPort = -1
    ) noexcept : m_uiBinaryAddress(uiBinaryAddress), m_usPort(usPort) {}

    ~NetServerPlayerID() noexcept {}

    friend constexpr bool operator==(
        const NetServerPlayerID& left,
        const NetServerPlayerID& right
    ) noexcept {
        if (left.m_uiBinaryAddress != right.m_uiBinaryAddress)
            return false;

        if (left.m_usPort != right.m_usPort)
            return false;

        return true;
    }

    friend constexpr bool operator!=(
        const NetServerPlayerID& left,
        const NetServerPlayerID& right
    ) noexcept {
        return !operator==(left, right);
    }

    friend constexpr bool operator<(
        const NetServerPlayerID& left,
        const NetServerPlayerID& right
    ) noexcept {
        if (left.m_uiBinaryAddress >= right.m_uiBinaryAddress)
            return false;

        if (left.m_usPort >= right.m_usPort)
            return false;

        return true;
    }

    friend constexpr bool operator>(
        const NetServerPlayerID& left,
        const NetServerPlayerID& right
    ) noexcept {
        if (left.m_uiBinaryAddress <= right.m_uiBinaryAddress)
            return false;

        if (left.m_usPort <= right.m_usPort)
            return false;

        return true;
    }

    constexpr std::uint32_t GetBinaryAddress() const noexcept {
        return m_uiBinaryAddress;
    }
    constexpr std::uint16_t GetPort() const noexcept {
        return m_usPort;
    }
};
