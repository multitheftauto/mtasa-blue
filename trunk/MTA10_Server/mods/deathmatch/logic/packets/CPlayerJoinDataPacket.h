/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerJoinDataPacket.h
*  PURPOSE:     Player join data packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERJOINDATAPACKET_H
#define __PACKETS_CPLAYERJOINDATAPACKET_H

#include "CPacket.h"
#include "../../Config.h"

class CPlayerJoinDataPacket : public CPacket
{
public:
    virtual bool            RequiresSourcePlayer        ( void ) const                      { return false; }
    inline ePacketID        GetPacketID                 ( void ) const                      { return static_cast < ePacketID > ( PACKET_ID_PLAYER_JOINDATA ); };
    inline unsigned long    GetFlags                    ( void ) const                      { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetBitStreamInterface& BitStream );

    inline unsigned short   GetNetVersion               ( void )                            { return m_usNetVersion; };
    inline unsigned char    GetGameVersion              ( void )                            { return m_ucGameVersion; };

    inline void             SetNetVersion               ( unsigned short usNetVersion )     { m_usNetVersion = usNetVersion; };
    inline void             SetGameVersion              ( unsigned char ucGameVersion )     { m_ucGameVersion = ucGameVersion; };

    inline unsigned short   GetMTAVersion               ( void )                            { return m_usMTAVersion; };
    inline unsigned short   GetBitStreamVersion         ( void )                            { return m_usBitStreamVersion; };
    inline const SString&   GetPlayerVersion            ( void )                            { return m_strPlayerVersion; };

    inline const char*      GetNick                     ( void )                            { return m_strNick; };
    inline void             SetNick                     ( const char* szNick )              { m_strNick.AssignLeft( szNick, MAX_NICK_LENGTH ); };

    inline const MD5&       GetPassword                 ( void )                            { return m_Password; };
    inline void             SetPassword                 ( const MD5& Password )             { m_Password = Password; };

    inline const char*      GetSerialUser               ( void )                            { return m_strSerialUser; }
    inline void             SetSerialUser               ( const char* szSerialUser )        { m_strSerialUser.AssignLeft( szSerialUser, MAX_SERIAL_LENGTH ); }

    inline bool             IsOptionalUpdateInfoRequired ( void )                           { return m_bOptionalUpdateInfoRequired; }

private:
    unsigned short          m_usNetVersion;
    unsigned short          m_usMTAVersion;
    unsigned short          m_usBitStreamVersion;
    unsigned char           m_ucGameVersion;
    bool                    m_bOptionalUpdateInfoRequired;
    SString                 m_strNick;
    MD5                     m_Password;
    SString                 m_strSerialUser;
    SString                 m_strPlayerVersion;
};

#endif
