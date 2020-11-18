void CGame::Packet_PlayerNoSocket(CPlayerNoSocketPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // If we are getting 'no socket' warnings from the network layer, and sync has not been received for ages, assume some sort of problem and quit the
        // player
        if (pPlayer->GetTimeSinceReceivedSync() > 20000)
        {
            CLogger::LogPrintf("INFO: Dead connection detected for %s\n", pPlayer->GetNick());
            pPlayer->Send(CPlayerDisconnectedPacket(CPlayerDisconnectedPacket::KICK, "Worrying message"));
            g_pGame->QuitPlayer(*pPlayer, CClient::QUIT_TIMEOUT);
        }
    }
}
