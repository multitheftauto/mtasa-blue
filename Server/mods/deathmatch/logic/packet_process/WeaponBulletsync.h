void CGame::Packet_WeaponBulletsync(CCustomWeaponBulletSyncPacket& Packet)
{
    // Grab the source player
    CPlayer*       pPlayer = Packet.GetSourcePlayer();
    CCustomWeapon* pWeapon = Packet.GetWeapon();
    if (pPlayer && pPlayer->IsJoined() && pPlayer == Packet.GetWeaponOwner())
    {
        // Tell our scripts the player has fired
        CLuaArguments Arguments;
        Arguments.PushElement(pPlayer);

        if (pWeapon->CallEvent("onWeaponFire", Arguments))
        {
            // Relay to other players
            m_pPlayerManager->BroadcastOnlyJoined(Packet, pPlayer);
        }
    }
}