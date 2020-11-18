void CGame::Packet_DetonateSatchels(CDetonateSatchelsPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Tell everyone to blow up this guy's satchels
        m_pPlayerManager->BroadcastOnlyJoined(Packet);
        // Take away their detonator
        CStaticFunctionDefinitions::TakeWeapon(pPlayer, 40);
    }
}
