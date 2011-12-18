/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPickupHideShowPacket.cpp
*  PURPOSE:     Pickup hide/show packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPickupHideShowPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Got more than zero pickups added?
    if ( m_List.size () > 0 )
    {
        // Write the flags
        BitStream.WriteBit ( m_bShow );

        // Write the pickup ids
        unsigned short usPickupModelID;
        CPickup* pPickup;
        vector < CPickup* > ::const_iterator iter = m_List.begin ();
        for ( ; iter != m_List.end (); iter++ )
        {
            pPickup = *iter;
            usPickupModelID = 0;

            // Grab the pickup type and set the pickup model id using it
            unsigned char ucPickupType = pPickup->GetPickupType ();
            switch ( ucPickupType )
            {
                case CPickup::HEALTH: usPickupModelID = 1240; break;
                case CPickup::ARMOR: usPickupModelID = 1242; break;
                case CPickup::WEAPON: usPickupModelID = CPickupManager::GetWeaponModel ( pPickup->GetWeaponType () ); break;
                case CPickup::CUSTOM: usPickupModelID = pPickup->GetModel (); break;
                default: break;
            }

            // Is the pickup model valid?
            if ( usPickupModelID != 0 )
            {
                // Write the pickup model id and id
                BitStream.Write ( pPickup->GetID () );
                BitStream.WriteCompressed ( usPickupModelID );
            }
        }

        return true;
    }

    return false;
}
