void CGame::Packet_PlayerQuit(CPlayerQuitPacket& Packet)
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Quit him
        QuitPlayer(*pPlayer);
    }
}