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
#include <string>

class NetServerPlayerID
{
public:
    unsigned long   m_uiBinaryAddress;
    unsigned short  m_usPort;
    char            m_szSerial [ 64 ];

public:
    NetServerPlayerID ( void )
    {
        m_uiBinaryAddress = 0xFFFFFFFF;
        m_usPort = 0xFFFF;
        m_szSerial [ 0 ] = '\0';
    };

    NetServerPlayerID ( const NetServerPlayerID& PlayerID )
    {
        m_uiBinaryAddress = PlayerID.m_uiBinaryAddress;
        m_usPort = PlayerID.m_usPort;
        SetSerial ( PlayerID.m_szSerial );
    };

    ~NetServerPlayerID ( void )
    {
    };

    inline NetServerPlayerID& operator = ( const NetServerPlayerID& PlayerID )
    {
    	m_uiBinaryAddress = PlayerID.m_uiBinaryAddress;
    	m_usPort = PlayerID.m_usPort;
        SetSerial ( PlayerID.m_szSerial );
        return *this;
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
    
    inline void         GetSerial           ( std::string & strSerial ) const   { strSerial = m_szSerial; };
    inline void         SetSerial           ( std::string strSerial )           { SetSerial ( strSerial.c_str() ); };

    inline void         SetSerial           ( const char* szSerial )
    {
        if ( szSerial )
        {
            size_t length = strlen ( szSerial );
            if ( length >= sizeof ( m_szSerial ) )
            {
                length = sizeof ( m_szSerial ) - 1;
            }
            strncpy ( m_szSerial, szSerial, length );
            m_szSerial [ length ] = '\0';
        }
        else
            m_szSerial [ 0 ] = '\0';
    };
};

#endif

