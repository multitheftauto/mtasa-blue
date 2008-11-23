/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleSpawnPacket.h
*  PURPOSE:     Vehicle spawn packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CVehicleSpawnPacket;

#ifndef __CVEHICLESPAWNPACKET_H
#define __CVEHICLESPAWNPACKET_H

#include "CPacket.h"
#include <vector>

using namespace std;
class CVehicle;

class CVehicleSpawnPacket : public CPacket
{
public:
    inline ePacketID            GetPacketID             ( void ) const                  { return PACKET_ID_VEHICLE_SPAWN; };
    inline unsigned long        GetFlags                ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                        Write                   ( NetServerBitStreamInterface& BitStream ) const;

    inline void                 Add                     ( CVehicle* pVehicle )          { m_List.push_back ( pVehicle ); };
    inline void                 Clear                   ( void )                        { m_List.clear (); };

private:
    vector <CVehicle* >         m_List;
};

#endif
