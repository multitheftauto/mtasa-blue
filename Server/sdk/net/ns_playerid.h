/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/net/ns_playerid.h
 *  PURPOSE:     Net server player ID interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <MTAPlatform.h>

class SNetExtraInfo : public CRefCountable
{
protected:
    SNetExtraInfo(const SNetExtraInfo&);
    const SNetExtraInfo& operator=(const SNetExtraInfo&);
    ~SNetExtraInfo() {}

public:
    ZERO_ON_NEW
    SNetExtraInfo() {}

    bool m_bHasPing;
    uint m_uiPing;
};

class NetServerPlayerID
{
protected:
    unsigned long  m_uiBinaryAddress;
    std::uint16_t m_usPort;

public:
    NetServerPlayerID()
    {
        m_uiBinaryAddress = 0xFFFFFFFF;
        m_usPort = 0xFFFF;
    };

    NetServerPlayerID(unsigned long uiBinaryAddress, std::uint16_t usPort)
    {
        m_uiBinaryAddress = uiBinaryAddress;
        m_usPort = usPort;
    };

    ~NetServerPlayerID(){};

    friend inline int operator==(const NetServerPlayerID& left, const NetServerPlayerID& right)
    {
        return left.m_uiBinaryAddress == right.m_uiBinaryAddress && left.m_usPort == right.m_usPort;
    };

    friend inline int operator!=(const NetServerPlayerID& left, const NetServerPlayerID& right)
    {
        return ((left.m_uiBinaryAddress != right.m_uiBinaryAddress) || (left.m_usPort != right.m_usPort));
    };

    friend inline bool operator<(const NetServerPlayerID& left, const NetServerPlayerID& right)
    {
        return left.m_uiBinaryAddress < right.m_uiBinaryAddress || (left.m_uiBinaryAddress == right.m_uiBinaryAddress && left.m_usPort < right.m_usPort);
    }

    unsigned long  GetBinaryAddress() const { return m_uiBinaryAddress; };
    std::uint16_t GetPort() const { return m_usPort; };
};
