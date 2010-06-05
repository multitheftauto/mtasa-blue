/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPacketTranslator.h
*  PURPOSE:     Network packet translator class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kent Simon <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPacketTranslator;

#ifndef __CPACKETTRANSLATOR_H
#define __CPACKETTRANSLATOR_H

#include "CCommon.h"
#include "CPlayerManager.h"
#include "packets/CPacket.h"

// Left in for dummy purposes
#include "packets/CVoiceDataPacket.h"

class CPacketTranslator
{
public:
                            CPacketTranslator       ( CPlayerManager* pPlayerManager );
                            ~CPacketTranslator      ( void );

    CPacket*                Translate               ( NetServerPlayerID& Socket, ePacketID PacketID, NetBitStreamInterface& BitStream );

private:
    CPlayerManager*         m_pPlayerManager;
};

#endif
