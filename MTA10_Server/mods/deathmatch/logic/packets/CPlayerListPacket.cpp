/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerListPacket.cpp
*  PURPOSE:     Player list packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPlayerListPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // bool                  - show the "X has joined the game" messages?
    // [ following repeats <number of players joined> times ]
    // unsigned char  (1)    - assigned player id
    // unsigned char  (1)    - player nick length
    // unsigned char  (X)    - player nick (X = player nick length)
    // bool                  - is he dead?
    // bool                  - spawned? (following data only if this is TRUE)
    // unsigned char  (1)    - model id
    // unsigned char  (1)    - team id
    // bool                  - in a vehicle?
    // unsigned short (2)    - vehicle id (if vehicle)
    // unsigned char  (1)    - vehicle seat (if vehicle)
    // CVector        (12)   - position (if player)
    // float          (4)    - rotation (if player)
    // bool                  - has a jetpack?
    // unsigned short (2)    - dimension

    // Write the global flags
    BitStream.WriteBit ( m_bShowInChat );

    CPlayer* pPlayer = NULL;
    // Put each player in our list into the packet
    list < CPlayer* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Grab the real pointer
        pPlayer = *iter;

        // Write the player ID
        ElementID PlayerID = pPlayer->GetID ();
        BitStream.Write ( PlayerID );

        // Time sync context
        BitStream.Write ( pPlayer->GetSyncTimeContext () );

        // Write the nick length
        const char* szNickPointer = pPlayer->GetNick ();
        unsigned char ucNickLength = static_cast < unsigned char > ( strlen ( szNickPointer ) );
        if ( ucNickLength < MIN_NICK_LENGTH || ucNickLength > MAX_NICK_LENGTH )
        {
            BitStream.Write ( static_cast < unsigned char > ( 3 ) );
            BitStream.Write ( "???", 3 );
        }
        else
        {
            BitStream.Write ( ucNickLength );
            BitStream.Write ( szNickPointer, ucNickLength );
        }

        // Version info
        if ( BitStream.Version () >= 0x34 )
        {
            BitStream.Write ( pPlayer->GetBitStreamVersion () );
            SString strBuild = pPlayer->GetPlayerVersion ().SubStr ( 8 );
            uint uiBuildNumber = atoi ( strBuild );
            BitStream.Write ( uiBuildNumber );
        }

        // Flags
        bool bIsSpawned = pPlayer->IsSpawned ();
        bool bInVehicle = ( bIsSpawned && ( pPlayer->GetOccupiedVehicle () != NULL ) );
        BitStream.WriteBit ( pPlayer->IsDead () );
        BitStream.WriteBit ( bIsSpawned );
        BitStream.WriteBit ( bInVehicle );
        BitStream.WriteBit ( pPlayer->HasJetPack () );
        BitStream.WriteBit ( pPlayer->IsNametagShowing () );
        BitStream.WriteBit ( pPlayer->IsNametagColorOverridden () );
        BitStream.WriteBit ( pPlayer->IsHeadless() );
        BitStream.WriteBit ( pPlayer->IsFrozen() );

        // Nametag stuff
        unsigned char ucNametagTextLength = 0;
        char* szNametagText = pPlayer->GetNametagText ();
        if ( szNametagText )
            ucNametagTextLength = static_cast < unsigned char > ( strlen ( szNametagText ) );

        BitStream.Write ( ucNametagTextLength );
        if ( ucNametagTextLength > 0 )
            BitStream.Write ( szNametagText, ucNametagTextLength );

        // Write nametag color if it's overridden
        if ( pPlayer->IsNametagColorOverridden () )
        {
            unsigned char ucR, ucG, ucB;
            pPlayer->GetNametagColor ( ucR, ucG, ucB );
            BitStream.Write ( ucR );
            BitStream.Write ( ucG );
            BitStream.Write ( ucB );
        }

        // Move anim
        if ( BitStream.Version() > 0x4B )
        {
            uchar ucMoveAnim = pPlayer->GetMoveAnim();
            BitStream.Write ( ucMoveAnim );
        }

        // **************************************************************************************************************
        // Note: The code below skips various attributes if the player is not spawned.
        // This means joining clients will not receive the current value of these attributes, which could lead to desync.
        // **************************************************************************************************************

        // Write spawn info if he's spawned
        if ( bIsSpawned )
        {
            // Player model ID
            BitStream.WriteCompressed ( pPlayer->GetModel () );

            // Team id
            CTeam* pTeam = pPlayer->GetTeam ();
            if ( pTeam )
            {
                BitStream.WriteBit ( true );
                BitStream.Write ( pTeam->GetID () );
            }
            else
                BitStream.WriteBit ( false );

            if ( bInVehicle )
            {
                // Grab the occupied vehicle
                CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();

                // Vehicle ID and seat
                BitStream.Write ( pVehicle->GetID () );

                SOccupiedSeatSync seat;
                seat.data.ucSeat = pPlayer->GetOccupiedVehicleSeat ();
                BitStream.Write ( &seat );
            }
            else
            {
                // Player position
                SPositionSync position ( false );
                position.data.vecPosition = pPlayer->GetPosition ();
                BitStream.Write ( &position );

                // Player rotation
                SPedRotationSync rotation;
                rotation.data.fRotation = pPlayer->GetRotation ();
                BitStream.Write ( &rotation );
            }

            BitStream.WriteCompressed ( pPlayer->GetDimension () );
            BitStream.Write ( pPlayer->GetFightingStyle () );

            SEntityAlphaSync alpha;
            alpha.data.ucAlpha = pPlayer->GetAlpha ();
            BitStream.Write ( &alpha );

            BitStream.Write ( pPlayer->GetInterior () );

            // Write the weapons of the player weapon slots
            for ( unsigned int i = 0; i < 16; ++i )
            {
                CWeapon* pWeapon = pPlayer->GetWeapon ( i );
                if ( pWeapon && pWeapon->ucType != 0 )
                {
                    BitStream.WriteBit ( true );
                    SWeaponTypeSync weaponType;
                    weaponType.data.ucWeaponType = pWeapon->ucType;
                    BitStream.Write ( &weaponType );
                }
                else
                    BitStream.WriteBit ( false );
            }
        }
    }

    return true;
}
