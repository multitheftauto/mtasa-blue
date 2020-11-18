void CGame::Packet_PedTask(CPedTaskPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Relay to other players
        RelayNearbyPacket(Packet);
    }
}