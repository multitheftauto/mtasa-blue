void CGame::Packet_Command(CCommandPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Tell the console
        m_pConsole->HandleInput(Packet.GetCommand(), pPlayer, pPlayer);
    }
}
