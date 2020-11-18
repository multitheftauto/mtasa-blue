void CGame::Packet_PlayerJoin(const NetServerPlayerID& Source)
{
    // Reply with the mod this server is running
    NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream(0);
    if (pBitStream)
    {
        // Write the mod name to the bitstream
        pBitStream->Write(static_cast<unsigned short>(MTA_DM_BITSTREAM_VERSION));
        pBitStream->WriteString("deathmatch");

        // Send and destroy the bitstream
        g_pNetServer->SendPacket(PACKET_ID_MOD_NAME, Source, pBitStream, false, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
        g_pNetServer->DeallocateNetServerBitStream(pBitStream);
    }
}