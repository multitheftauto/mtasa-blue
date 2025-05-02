/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerListPacket.cpp
 *  PURPOSE:     Player list packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerListPacket.h"
#include "CPlayer.h"
#include <net/SyncStructures.h>

bool CPlayerListPacket::Write(NetBitStreamInterface& BitStream) const
{
    // string          (X)   - AC info
    // string          (X)   - SD info
    // bool            (1)   - whether these are new players that have just joined
    //                       x ProtocolError(8)
    //
    // [ following repeats <number of players joined> times ]
    //
    // unsigned char   (1)   - assigned player id
    //                       x ProtocolError(8)
    // unsigned char   (1)   - time sync context
    // unsigned char   (1)   - player nick length
    //                       x ProtocolError(9)
    // unsigned char   (X)   - player nick (X = player nick length)
    //
    // if bitStream version >= 0x34
    // | unsigned short(2)  - bitStream version
    // | unsigned int  (1)  - build number
    //
    // bool            (1)   - is he dead?
    // bool            (1)   - spawned? (*)
    // bool            (1)   - in a vehicle? (**)
    // bool            (1)   - has a jetpack?
    // bool            (1)   - nametag showing
    // bool            (1)   - nametag color overridden (***)
    // bool            (1)   - headless?
    // bool            (1)   - frozen?
    // unsigned char   (1)   - nametag text length
    // unsigned char   (X)   - nametag text (X = nametag text length)
    //
    // if (***) nametag color overridden TRUE
    // | unsigned char (3)   - nametag color (RGB)
    //
    // if bitStream version > 0x4B
    // | unsigned char (1)   - movement anim (default = MOVE_DEFAULT)
    //
    // --------------------------------------
    // (*) ALL following data only if SPAWNED
    //     Always true for new server builds
    // --------------------------------------
    //
    // unsigned char   (1)   - model id
    //                       x ProtocolError(10)
    // bool            (1)   - has team
    // if has team
    // | ElementID     (2)   - team id
    // |                      x ProtocolError(10)
    //
    // if (**) inside vehicle
    // | ElementID     (2)    - vehicle id
    // | unsigned char (4)    - vehicle seat
    // |                      x ProtocolError(11)
    // else (on foot)
    // | CVector       (12)   - position
    // | float         (4)    - rotation

    // unsigned short  (2)    - dimension
    // unsigned char   (1)    - fighting style
    // unsigned char   (1)    - alpha
    // unsigned char   (1)    - interior
    //
    // ---------------------------------
    // 16 reads of the following (0..15)
    // ---------------------------------
    // bool            (1)    - if player has this weapon id
    // if player has this weapon
    // | unsigned char   (6)  - weapon type of this id

    // Write the global flags
    BitStream.WriteBit(m_bShowInChat);

    CPlayer* pPlayer = NULL;
    // Put each player in our list into the packet
    std::list<CPlayer*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); ++iter)
    {
        // Grab the real pointer
        pPlayer = *iter;

        // Write the player ID
        ElementID PlayerID = pPlayer->GetID();
        BitStream.Write(PlayerID);

        // Time sync context
        BitStream.Write(pPlayer->GetSyncTimeContext());

        // Write the nick length
        const char*   szNickPointer = pPlayer->GetNick();
        unsigned char ucNickLength = static_cast<unsigned char>(strlen(szNickPointer));
        if (ucNickLength < MIN_PLAYER_NICK_LENGTH || ucNickLength > MAX_PLAYER_NICK_LENGTH)
        {
            BitStream.Write(static_cast<unsigned char>(3));
            BitStream.Write("???", 3);
        }
        else
        {
            BitStream.Write(ucNickLength);
            BitStream.Write(szNickPointer, ucNickLength);
        }

        // Version info
        if (BitStream.Version() >= 0x34)
        {
            BitStream.Write(pPlayer->GetBitStreamVersion());
            BitStream.Write(pPlayer->GetPlayerVersion().GetBuildNumber());
        }

        // Flags
        bool bInVehicle = (pPlayer->GetOccupiedVehicle() != NULL);
        BitStream.WriteBit(pPlayer->IsDead());
        BitStream.WriteBit(true);            // (Was IsSpawned) Used by the client to determine if extra info was sent (in this packet)
        BitStream.WriteBit(bInVehicle);
        BitStream.WriteBit(pPlayer->HasJetPack());
        BitStream.WriteBit(pPlayer->IsNametagShowing());
        BitStream.WriteBit(pPlayer->IsNametagColorOverridden());
        BitStream.WriteBit(pPlayer->IsHeadless());
        BitStream.WriteBit(pPlayer->IsFrozen());

        // Nametag stuff
        unsigned char ucNametagTextLength = 0;
        char*         szNametagText = pPlayer->GetNametagText();
        if (szNametagText)
            ucNametagTextLength = static_cast<unsigned char>(strlen(szNametagText));

        if (!BitStream.Can(eBitStreamVersion::UnicodeNametags))
        {
            // Old client version has a fixed buffer of 22 characters
            ucNametagTextLength = std::min<uchar>(ucNametagTextLength, 22);
        }

        BitStream.Write(ucNametagTextLength);
        if (ucNametagTextLength > 0)
            BitStream.Write(szNametagText, ucNametagTextLength);

        // Write nametag color if it's overridden
        if (pPlayer->IsNametagColorOverridden())
        {
            unsigned char ucR, ucG, ucB;
            pPlayer->GetNametagColor(ucR, ucG, ucB);
            BitStream.Write(ucR);
            BitStream.Write(ucG);
            BitStream.Write(ucB);
        }

        // Move anim
        if (BitStream.Version() > 0x4B)
        {
            uchar ucMoveAnim = pPlayer->GetMoveAnim();
            BitStream.Write(ucMoveAnim);
        }

        // Always send extra info (Was: "Write spawn info if he's spawned")
        if (true)
        {
            // Player model ID
            BitStream.WriteCompressed(pPlayer->GetModel());

            // Team id
            CTeam* pTeam = pPlayer->GetTeam();
            if (pTeam)
            {
                BitStream.WriteBit(true);
                BitStream.Write(pTeam->GetID());
            }
            else
                BitStream.WriteBit(false);

            if (bInVehicle)
            {
                // Grab the occupied vehicle
                CVehicle* pVehicle = pPlayer->GetOccupiedVehicle();

                // Vehicle ID and seat
                BitStream.Write(pVehicle->GetID());

                SOccupiedSeatSync seat;
                seat.data.ucSeat = pPlayer->GetOccupiedVehicleSeat();
                BitStream.Write(&seat);
            }
            else
            {
                // Player position
                SPositionSync position(false);
                position.data.vecPosition = pPlayer->GetPosition();
                BitStream.Write(&position);

                // Player rotation
                SPedRotationSync rotation;
                rotation.data.fRotation = pPlayer->GetRotation();
                BitStream.Write(&rotation);
            }

            BitStream.WriteCompressed(pPlayer->GetDimension());
            BitStream.Write(pPlayer->GetFightingStyle());

            SEntityAlphaSync alpha;
            alpha.data.ucAlpha = pPlayer->GetAlpha();
            BitStream.Write(&alpha);

            BitStream.Write(pPlayer->GetInterior());

            // Write the weapons of the player weapon slots
            for (unsigned int i = 0; i < 16; ++i)
            {
                CWeapon* pWeapon = pPlayer->GetWeapon(i);
                if (pWeapon && pWeapon->ucType != 0)
                {
                    BitStream.WriteBit(true);
                    SWeaponTypeSync weaponType;
                    weaponType.data.ucWeaponType = pWeapon->ucType;
                    BitStream.Write(&weaponType);
                }
                else
                    BitStream.WriteBit(false);
            }
        }
    }

    return true;
}
