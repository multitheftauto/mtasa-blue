/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CConnectManager.h
*  PURPOSE:		Header file for connect manager
*  DEVELOPERS:	Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONNECTMANAGER_H
#define __CCONNECTMANAGER_H

#include <ctime>
#include <gui/CGUI.h>

#define	NET_MTU_MODEM		576
#define NET_MTU_DSL			1400
#define NET_MTU_LAN			1492

class CConnectManager
{
public:
                    CConnectManager     ( void );
                    ~CConnectManager    ( void );

    bool            Connect             ( const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword );
    bool            Abort               ( void );
    inline bool     IsConnecting        ( void )                                                                { return m_bIsConnecting; };

    void            DoPulse             ( void );

    static bool     StaticProcessPacket ( unsigned char ucPacketID, class NetBitStreamInterface& bitStream, unsigned long ulTimeStamp );

	inline void		SetMTUSize			( unsigned short usMTUSize )											{ m_usMTUSize = usMTUSize; };


private:
    bool            Event_OnCancelClick ( CGUIElement * pElement );

	unsigned short	m_usMTUSize;
    char*           m_szHost;
    unsigned short  m_usPort;
    char*           m_szNick;
    char*           m_szPassword;
    bool            m_bIsConnecting;
    time_t          m_tConnectStarted;

    GUI_CALLBACK*   m_pOnCancelClick;

	bool CheckNickProvided ( char* szNick );
};

#endif
