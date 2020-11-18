void CGame::Packet_PedWasted(CPedWastedPacket& Packet)
{
    CPed* pPed = GetElementFromId<CPed>(Packet.m_PedID);
    if (pPed && !pPed->IsDead())
    {
        pPed->SetIsDead(true);
        pPed->SetPosition(Packet.m_vecPosition);
        // Remove him from any occupied vehicle
        pPed->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
        CVehicle* pVehicle = pPed->GetOccupiedVehicle();
        if (pVehicle)
        {
            pVehicle->SetOccupant(NULL, pPed->GetOccupiedVehicleSeat());
            pPed->SetOccupiedVehicle(NULL, 0);
        }

        CElement* pKiller = (Packet.m_Killer != INVALID_ELEMENT_ID) ? CElementIDs::GetElement(Packet.m_Killer) : NULL;

        // Create a new packet to send to everyone
        CPedWastedPacket ReturnWastedPacket(pPed, pKiller, Packet.m_ucKillerWeapon, Packet.m_ucBodyPart, false, Packet.m_AnimGroup, Packet.m_AnimID);
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
        pPed->CallEvent("onPedWasted", Arguments);

        // Reset the weapons list, because a ped loses his weapons on death
        for (unsigned int slot = 0; slot < WEAPON_SLOTS; ++slot)
        {
            pPed->SetWeaponType(0, slot);
            pPed->SetWeaponAmmoInClip(0, slot);
            pPed->SetWeaponTotalAmmo(0, slot);
        }
    }
}