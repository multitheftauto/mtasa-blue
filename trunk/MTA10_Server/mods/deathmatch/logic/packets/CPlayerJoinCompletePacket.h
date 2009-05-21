/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerJoinCompletePacket.h
*  PURPOSE:     Player join completion packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERJOINCOMPLETEPACKET_H
#define __PACKETS_CPLAYERJOINCOMPLETEPACKET_H

#include "../CCommon.h"
#include "CPacket.h"
#include <string.h>

class CPlayerJoinCompletePacket : public CPacket
{
public:
                            CPlayerJoinCompletePacket       ( void );
                            CPlayerJoinCompletePacket       ( ElementID PlayerID,
                                                      unsigned char ucNumberOfPlayers,
                                                      ElementID RootElementID,
                                                      eHTTPDownloadType ucHTTPDownloadType,
                                                      unsigned short usHTTPDownloadPort,
                                                      const char* szHTTPDownloadURL );

    inline ePacketID        GetPacketID             ( void ) const      { return PACKET_ID_SERVER_JOINEDGAME; };
    inline unsigned long    GetFlags                ( void ) const      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;

    inline ElementID        GetPlayerID                     ( void )                            { return m_PlayerID; };
    inline unsigned char    GetNumberOfPlayers              ( void )                            { return m_ucNumberOfPlayers; };
    inline eHTTPDownloadType GetHTTPDownloadType             ( void )                            { return m_ucHTTPDownloadType; };
    inline unsigned short   GetHTTPDownloadPort             ( void )                            { return m_usHTTPDownloadPort; };
    inline const char*      GetHTTPDownloadURL              ( void )                            { return m_szHTTPDownloadURL; };

    inline void             SetPlayerID                     ( ElementID PlayerID )       { m_PlayerID = PlayerID; };
    inline void             SetNumberOfPlayers              ( unsigned char ucNumberOfPlayers ) { m_ucNumberOfPlayers = ucNumberOfPlayers; };
    inline void             SetRootElementID                ( ElementID ID )                    { m_RootElementID = ID; }
    inline void             SetHTTPDownloadType             ( eHTTPDownloadType ucHTTPDownloadType )       { m_ucHTTPDownloadType = ucHTTPDownloadType; };
    inline void             SetHTTPDownloadPort             ( unsigned short usHTTPDownloadPort ) { m_usHTTPDownloadPort = usHTTPDownloadPort; };
    inline void             SetHTTPDownloadURL              ( const char* szHTTPDownloadURL ) { strncpy ( m_szHTTPDownloadURL, szHTTPDownloadURL, MAX_HTTP_DOWNLOAD_URL ); m_szHTTPDownloadURL [MAX_HTTP_DOWNLOAD_URL] = 0; };

private:
    ElementID               m_PlayerID;
    unsigned char           m_ucNumberOfPlayers;
    ElementID               m_RootElementID;
    eHTTPDownloadType       m_ucHTTPDownloadType;
    unsigned short          m_usHTTPDownloadPort;
    char                    m_szHTTPDownloadURL [MAX_HTTP_DOWNLOAD_URL + 1];
};

#endif
