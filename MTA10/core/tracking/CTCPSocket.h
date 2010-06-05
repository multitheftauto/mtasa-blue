/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/tracking/CTCPSocket.h
*  PURPOSE:		Interface for TCP socket
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPSOCKET_H
#define __CTCPSOCKET_H

#include <windows.h>

// Custom async-related window message define
#define WM_ASYNCTRAP	WM_USER

typedef void (*PFNEVENT) ( void *, void * );

class CTCPSocket
{
protected:
    virtual					~CTCPSocket         ( void ) {};
public:
    virtual void            AddRef              ( void ) = 0;
    virtual void            Release             ( void ) = 0;

    virtual int				ReadBuffer          ( void* pOutput, int iSize ) = 0;
    virtual int				WriteBuffer         ( const void* pInput, int iSize ) = 0;

    virtual bool            Initialize			( unsigned int uiID ) = 0;

    virtual const char*     GetLastError		( void ) = 0;
	virtual void			FireEvent			( LPARAM lType ) = 0;

	virtual void			SetEventClass		( void* pClass ) = 0;
	virtual void			SetEventRead		( PFNEVENT pEvent ) = 0;
	virtual void			SetEventWrite		( PFNEVENT pEvent ) = 0;
	virtual void			SetEventConnect		( PFNEVENT pEvent ) = 0;
	virtual void			SetEventClose		( PFNEVENT pEvent ) = 0;
};

#endif
