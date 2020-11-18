void CGame::Packet_Bulletsync(CBulletsyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Relay to other players
        RelayNearbyPacket(Packet);

        // Call event
        CLuaArguments Arguments;
        Arguments.PushNumber(Packet.m_WeaponType);
        Arguments.PushNumber(Packet.m_vecEnd.fX);
        Arguments.PushNumber(Packet.m_vecEnd.fY);
        Arguments.PushNumber(Packet.m_vecEnd.fZ);

        if (Packet.m_DamagedPlayerID == INVALID_ELEMENT_ID)
        {
            Arguments.PushNil();
        }
        else
        {
            Arguments.PushElement(CElementIDs::GetElement(Packet.m_DamagedPlayerID));
        }

        Arguments.PushNumber(Packet.m_vecStart.fX);
        Arguments.PushNumber(Packet.m_vecStart.fY);
        Arguments.PushNumber(Packet.m_vecStart.fZ);
        pPlayer->CallEvent("onPlayerWeaponFire", Arguments);
    }
}