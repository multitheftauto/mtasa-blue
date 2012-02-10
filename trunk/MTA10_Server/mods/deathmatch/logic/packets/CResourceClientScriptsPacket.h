/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.3
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CResourceClientScriptsPacket.h
*  PURPOSE:     Resource client-side scripts packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <vector>
#include "CResource.h"
#include "CResourceClientScriptItem.h"

class CResourceClientScriptsPacket : public CPacket
{
public:
                            CResourceClientScriptsPacket    ( CResource* pResource );

    inline ePacketID        GetPacketID                 ( void ) const      { return PACKET_ID_RESOURCE_CLIENT_SCRIPTS; };
    inline unsigned long    GetFlags                    ( void ) const      { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    void                    AddItem                     ( CResourceClientScriptItem* pItem );

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    std::vector<CResourceClientScriptItem*>     m_vecItems;
    CResource*                                  m_pResource;
};
