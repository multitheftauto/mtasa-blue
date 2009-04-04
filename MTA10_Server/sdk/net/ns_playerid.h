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

#ifndef __NS_PLAYERID_H
#define __NS_PLAYERID_H

#include <Platform.h>
#include <string>

class NetServerPlayerID
{
public:
	ULONG           m_uiBinaryAddress;
	USHORT          m_usPort;
    std::string     m_strSerial;

public:
	NetServerPlayerID ( void )
	{
		m_uiBinaryAddress = 0xFFFFFFFF;
		m_usPort = 0xFFFF;
	};

	NetServerPlayerID ( const NetServerPlayerID& PlayerID )
	{
		m_uiBinaryAddress = PlayerID.m_uiBinaryAddress;
        m_usPort = PlayerID.m_usPort;
        m_strSerial = PlayerID.m_strSerial;
	};

	~NetServerPlayerID ( void )
	{
	};

	NetServerPlayerID& operator = ( const NetServerPlayerID& PlayerID )
	{
		m_uiBinaryAddress = PlayerID.m_uiBinaryAddress;
		m_usPort = PlayerID.m_usPort;
        m_strSerial = PlayerID.m_strSerial;
		return *this;
	};

	friend int operator == ( const NetServerPlayerID& left, const NetServerPlayerID& right )
	{
		return left.m_uiBinaryAddress == right.m_uiBinaryAddress && left.m_usPort == right.m_usPort;
	};

	friend int operator != ( const NetServerPlayerID& left, const NetServerPlayerID& right )
	{
		return ((left.m_uiBinaryAddress != right.m_uiBinaryAddress) || (left.m_usPort != right.m_usPort));
	};

	ULONG       GetBinaryAddress    ( void ) const                      { return m_uiBinaryAddress; };
	USHORT      GetPort             ( void ) const                      { return m_usPort; };
    
    void        GetSerial           ( std::string & strSerial ) const   { strSerial = m_strSerial; };
    void        SetSerial           ( std::string strSerial )           { m_strSerial = strSerial; };
};

#endif

