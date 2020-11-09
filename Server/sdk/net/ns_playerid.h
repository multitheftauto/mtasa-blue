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
    unsigned long  m_uiBinaryAddress; // Also called the IP address...
    unsigned short m_usPort;

public:
    NetServerPlayerID()
    {
        m_uiBinaryAddress = 0xFFFFFFFF;
        m_usPort = 0xFFFF;
    };

    NetServerPlayerID(unsigned long uiBinaryAddress, unsigned short usPort)
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
    unsigned short GetPort() const { return m_usPort; };



    // CFastHashMap/Set helpers
    // Must return keys that wont accur naturally. So, dont return a default constructed NetServerPlayerID
    friend NetServerPlayerID GetEmptyMapKey(NetServerPlayerID*)
    {
        // Return something here that isn't GetDeletedMapKey(), or a default constructed NetServerPlayerID
        return { std::numeric_limits<unsigned long>::min() / 3, std::numeric_limits<unsigned short>::min() / 3};
    }
    friend NetServerPlayerID GetDeletedMapKey(NetServerPlayerID*)
    {
        // Return something here that isn't GetEmptyMapKey(), or a default constructed NetServerPlayerID
        return { std::numeric_limits<unsigned long>::min() / 2, std::numeric_limits<unsigned short>::min() / 2 };
    }
};

// std::hash ovherload for NetServerPlayerID - used in CFastHashMap
namespace std
{
    template<>
    struct hash<NetServerPlayerID>
    {
        size_t operator()(const NetServerPlayerID& v) const noexcept
        {
            // Based on https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
            const auto port = v.GetPort();
            return v.GetBinaryAddress() + 0x9e3779b9 + (port << 6) + (port >> 2);
        }
    };
};
