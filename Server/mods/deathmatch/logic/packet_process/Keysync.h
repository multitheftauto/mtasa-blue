void CGame::Packet_Keysync(CKeysyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Relay to other players
        RelayNearbyPacket(Packet);
    }
}
