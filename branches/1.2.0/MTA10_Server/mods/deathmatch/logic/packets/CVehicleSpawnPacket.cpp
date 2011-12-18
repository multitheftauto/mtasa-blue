/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleSpawnPacket.cpp
*  PURPOSE:     Vehicle spawn packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CVehicleSpawnPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // unsigned short   (2)     - vehicle id
    // CVector          (12)    - position
    // CVector          (12)    - rotation in degrees
    // unsigned char    (1)     - color 1
    // unsigned char    (1)     - color 2
    // unsigned char    (1)     - color 3
    // unsigned char    (1)     - color 4
    // Repeats ...

    // Got any vehicles to send?
    if ( m_List.size () > 0 )
    {
        // Write each vehicle to the bitstream
        CVehicle* pVehicle;
        vector < CVehicle* > ::const_iterator iter = m_List.begin ();
        for ( ; iter != m_List.end (); iter++ )
        {
             pVehicle = *iter;

            // Vehicle ID
            BitStream.Write ( pVehicle->GetID () );

            // Generate a time context for it and write it
            BitStream.Write ( pVehicle->GenerateSyncTimeContext () );

            // Write the vehicle position
            const CVector& vecPosition = pVehicle->GetPosition ();
            BitStream.Write ( vecPosition.fX );
            BitStream.Write ( vecPosition.fY );
            BitStream.Write ( vecPosition.fZ );

            // Write the vehicle rotation in degrees
            CVector vecRotationDegrees;
            pVehicle->GetRotationDegrees ( vecRotationDegrees );
            BitStream.Write ( vecRotationDegrees.fX );
            BitStream.Write ( vecRotationDegrees.fY );
            BitStream.Write ( vecRotationDegrees.fZ );

             // Vehicle color
            CVehicleColor& vehColor = pVehicle->GetColor ();
            uchar ucNumColors = vehColor.GetNumColorsUsed () - 1;
            BitStream.WriteBits ( &ucNumColors, 2 );
            for ( uint i = 0 ; i <= ucNumColors ; i++ )
            {
                SColor RGBColor = vehColor.GetRGBColor ( i );
                BitStream.Write ( RGBColor.R );
                BitStream.Write ( RGBColor.G );
                BitStream.Write ( RGBColor.B );
            }
        }

        return true;
    }

    return false;
}
