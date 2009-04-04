/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/CLanBroadcastInterface.h 
*  PURPOSE:     LAN-broadcast server query interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLANBROADCAST_H
#define __CLANBROADCAST_H

class CLanBroadcast
{
public:

	virtual bool				Initialize				( unsigned short usServerPort ) = 0;
	virtual void				DoPulse					( void ) = 0;
	virtual unsigned short		GetPort					( void ) = 0;

};

#endif

