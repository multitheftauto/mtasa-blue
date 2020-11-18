void CGame::Packet_PlayerTimeout(CPlayerTimeoutPacket& Packet)
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Quit him
        QuitPlayer(*pPlayer, CClient::QUIT_TIMEOUT);
    }
}
