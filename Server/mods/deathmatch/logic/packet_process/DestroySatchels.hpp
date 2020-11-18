void CGame::Packet_DestroySatchels(CDestroySatchelsPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Tell everyone to destroy up this player's satchels
        m_pPlayerManager->BroadcastOnlyJoined(Packet);
        // Take away their detonator
        CStaticFunctionDefinitions::TakeWeapon(pPlayer, 40);
    }
}
