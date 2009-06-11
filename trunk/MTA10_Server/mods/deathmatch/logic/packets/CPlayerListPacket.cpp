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

    // Populate the global flags for all players
    unsigned char ucGlobalFlags = 0;
    ucGlobalFlags |= m_bShowInChat ? 1:0;

    // Write the global flags
    BitStream.Write ( ucGlobalFlags );

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
            BitStream.Write ( const_cast < char* > ( szNickPointer ), ucNickLength );
        }

        // Flags
        bool bIsDead = pPlayer->IsDead ();
        bool bIsSpawned = pPlayer->IsSpawned ();
        bool bInVehicle = bIsSpawned && ( pPlayer->GetOccupiedVehicle () != NULL );
        bool bHasJetpack = pPlayer->HasJetPack ();
        bool bNametagShowing = pPlayer->IsNametagShowing ();
        bool bNametagColorOverridden = pPlayer->IsNametagColorOverridden ();
        bool bIsHeadless = pPlayer->IsHeadless();
        bool bIsFrozen = pPlayer->IsFrozen();

        // Collect the flags into a byte
        unsigned char ucFlags = 0;
        ucFlags |= bIsDead ? 1 : 0;
        ucFlags |= bIsSpawned << 1;
        ucFlags |= bInVehicle << 2;
        ucFlags |= bHasJetpack << 3;
        ucFlags |= bNametagShowing << 4;
        ucFlags |= bNametagColorOverridden << 5;
        ucFlags |= bIsHeadless << 6;
        ucFlags |= bIsFrozen << 7;
        // Write the flags
        BitStream.Write ( ucFlags );

        // Nametag stuff
        unsigned char ucNametagTextLength = 0;
        char* szNametagText = pPlayer->GetNametagText ();
        if ( szNametagText )
            ucNametagTextLength = static_cast < unsigned char > ( strlen ( szNametagText ) );

        BitStream.Write ( ucNametagTextLength );
        if ( ucNametagTextLength > 0 )
            BitStream.Write ( szNametagText, ucNametagTextLength );

        // Write nametag color if it's overridden
        if ( bNametagColorOverridden )
        {
            unsigned char ucR, ucG, ucB;
            pPlayer->GetNametagColor ( ucR, ucG, ucB );
            BitStream.Write ( ucR );
            BitStream.Write ( ucG );
            BitStream.Write ( ucB );
        }
        
        // Write spawn info if he's spawned
        if ( bIsSpawned )
        {
            // Player model ID
            BitStream.Write ( pPlayer->GetModel () );

            // Team id
            CTeam* pTeam = pPlayer->GetTeam ();
            ElementID TeamID = ( pTeam ) ? pTeam->GetID () : INVALID_ELEMENT_ID;
            BitStream.Write ( TeamID );

            // Grab the occupied vehicle
            CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
            if ( pVehicle )
            {
                // Vehicle ID and seat
                BitStream.Write ( pVehicle->GetID () );
                BitStream.Write ( static_cast < unsigned char > ( pPlayer->GetOccupiedVehicleSeat () ) );
            }
            else
            {
                // Player position
                CVector vecTemp = pPlayer->GetPosition ();
                BitStream.Write ( vecTemp.fX );
                BitStream.Write ( vecTemp.fY );
                BitStream.Write ( vecTemp.fZ );

                // Player rotation
                BitStream.Write ( pPlayer->GetRotation () );
            }

            BitStream.Write ( pPlayer->GetDimension () );
            BitStream.Write ( pPlayer->GetFightingStyle () );
            BitStream.Write ( pPlayer->GetAlpha () );
            BitStream.Write ( pPlayer->GetInterior () );

            // Write the weapons of the player weapon slots
            for ( unsigned int i = 0; i < 16; ++i )
            {
                CWeapon* pWeapon = pPlayer->GetWeapon ( i );
                if ( pWeapon && pWeapon->ucType != 0 )
                {
                    BitStream.WriteBit ( true );
                    BitStream.Write ( pWeapon->ucType );
                }
                else
                    BitStream.WriteBit ( false );
            }
        }
    }

    return true;
}
