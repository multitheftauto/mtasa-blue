void CGame::Packet_PlayerWasted(CPlayerWastedPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && !pPlayer->IsDead())
    {
        pPlayer->SetSpawned(false);
        pPlayer->SetIsDead(true);
        pPlayer->SetHealth(0.0f);
        pPlayer->SetArmor(0.0f);
        pPlayer->SetPosition(Packet.m_vecPosition);

        // Remove him from any occupied vehicle
        pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
        if (pVehicle)
        {
            pVehicle->SetOccupant(NULL, pPlayer->GetOccupiedVehicleSeat());
            pPlayer->SetOccupiedVehicle(NULL, 0);
        }

        CElement* pKiller = (Packet.m_Killer != INVALID_ELEMENT_ID) ? CElementIDs::GetElement(Packet.m_Killer) : NULL;

        // Create a new packet to send to everyone
        CPlayerWastedPacket ReturnWastedPacket(pPlayer, pKiller, Packet.m_ucKillerWeapon, Packet.m_ucBodyPart, false, Packet.m_AnimGroup, Packet.m_AnimID);
        // Caz: send this to the local player to avoid issue #8148 - "Desync when calling spawnPlayer from an event handler remotely triggered from within
        // onClientPlayerWasted"
        m_pPlayerManager->BroadcastOnlyJoined(ReturnWastedPacket);

        // Tell our scripts the player has died
        CLuaArguments Arguments;
        Arguments.PushNumber(Packet.m_usAmmo);
        if (pKiller)
            Arguments.PushElement(pKiller);
        else
            Arguments.PushBoolean(false);
        if (Packet.m_ucKillerWeapon != 0xFF)
            Arguments.PushNumber(Packet.m_ucKillerWeapon);
        else
            Arguments.PushBoolean(false);
        if (Packet.m_ucBodyPart != 0xFF)
            Arguments.PushNumber(Packet.m_ucBodyPart);
        else
            Arguments.PushBoolean(false);
        Arguments.PushBoolean(false);
        pPlayer->CallEvent("onPlayerWasted", Arguments);

        // Reset the weapons list, because a player loses his weapons on death
        for (unsigned int slot = 0; slot < WEAPON_SLOTS; ++slot)
        {
            pPlayer->SetWeaponType(0, slot);
            pPlayer->SetWeaponAmmoInClip(0, slot);
            pPlayer->SetWeaponTotalAmmo(0, slot);
        }
    }
}