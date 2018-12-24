/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerListPacket.cpp
 *  PURPOSE:     Player list packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

bool CPlayerListPacket::Write(NetBitStreamInterface& BitStream) const
{
    // bool                                 - show the "X has joined the game" messages?
    // [ following repeats <number of players joined> times ]
    // ElementID             (4)            - assigned player id(I'm not sure it's 4 bytes, but -- Pirulax)
    // unsigned int          (4)            - Time sync context
    // unsigned char         (1)            - player rp name length
    // unsigned char         (X)            - player rp name (X = player rp name length)
    // unsigned short        (1)            - player bitstream version
    // unsigned int          (4)            - bitstream build number
    // bool                                 - is he dead? (UNUSED)
    // bool                                 - spawned? (unused as well, but here because of bckward compatiblity)
    // bool                                 - Is in vehicle
    // bool                                 - has a jetpack?
    // bool                                 - is his nametag showing?
    // bool                                 - is his nametag color overridden?
    // bool                                 - is headless?
    // bool                                 - is frozen?
    // unsigned char         (1)            - nametag text length
    // char*                 (x)            - nametag text(x = nametag text length)

    // Following only if nametag color is overridden:
    //    unsigned char      (1)            - nametag R
    //    unsigned char      (1)            - nametag G
    //    unsigned char      (1)            - nametag B

    // unsigned char         (1)            - Move anim(dafaqs that ? -Pirulax)
    // unsigned char         (1)            - model id
    // bool                                 - is in team
    // unsigned char         (1)            - team id (only if in team)

    // Following if in vehicle: 
    //    ElementID(4)                      - Vehicle elementid
    //    SOccupiedSeatSync  (1)            - SeatID
    // If not in vehicle:
    //    CVector            (12)           - Player position
    //    CVector            (12)           - Player rotation
    
    // unsigned short        (2)            - dimension
    // unsigned char         (1)            - Fighting style   
    // SEntityAlphaSyn       (1)            - Alpha

// 16x times:
    // bool                                 - true / false (for more info go to the comment in this file called: 
                                              //"Write the weapons of the player weapon slots")
    // SWeaponTypeSync     (6 bit)          - Take a look at it if you want to buddy

    // Write the global flags
    BitStream.WriteBit(m_bShowInChat);

    CPlayer* pPlayer = NULL;
    // Put each player in our list into the packet
    list<CPlayer*>::const_iterator iter = m_List.begin();
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
            SString strBuild = pPlayer->GetPlayerVersion().SubStr(8);
            uint    uiBuildNumber = atoi(strBuild);
            BitStream.Write(uiBuildNumber);
        }

        // Flags
        bool bInVehicle = (pPlayer->GetOccupiedVehicle() != NULL);
        BitStream.WriteBit(pPlayer->IsDead());            // Currently unused by the client
        BitStream.WriteBit(true);                         // (Was IsSpawned) Used by the client to determine if extra info was sent (in this packet)
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

        if (BitStream.Version() < 0x67)
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

    return true;
}
