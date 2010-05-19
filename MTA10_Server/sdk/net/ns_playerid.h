/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/ns_playerid.h
*  PURPOSE:     Net server player ID interface
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __NS_PLAYERID_H
#define __NS_PLAYERID_H

#include <MTAPlatform.h>
#include <SString.h>


class NetServerPlayerID
{
public:
    unsigned long   m_uiBinaryAddress;
    unsigned short  m_usPort;
    SString         m_strSerial;
    SString         m_strPlayerVersion;

public:
    NetServerPlayerID ( void )
    {
        m_uiBinaryAddress = 0xFFFFFFFF;
        m_usPort = 0xFFFF;
    };

    ~NetServerPlayerID ( void )
    {
    };

    friend inline int operator == ( const NetServerPlayerID& left, const NetServerPlayerID& right )
    {
        return left.m_uiBinaryAddress == right.m_uiBinaryAddress && left.m_usPort == right.m_usPort;
    };

    friend inline int operator != ( const NetServerPlayerID& left, const NetServerPlayerID& right )
    {
        return ((left.m_uiBinaryAddress != right.m_uiBinaryAddress) || (left.m_usPort != right.m_usPort));
    };

    inline unsigned long    GetBinaryAddress    ( void ) const                      { return m_uiBinaryAddress; };
    inline unsigned short   GetPort             ( void ) const                      { return m_usPort; };
    
    const SString&          GetSerial           ( void ) const                      { return m_strSerial; };
    inline void             SetSerial           ( const SString& strSerial )        { m_strSerial = strSerial; };

    const SString&          GetPlayerVersion    ( void ) const                      { return m_strPlayerVersion; }
    void                    SetPlayerVersion    ( const SString& strPlayerVersion ) { m_strPlayerVersion = strPlayerVersion; }
};

#endif

