void CGame::Packet_PlayerPuresync(CPlayerPuresyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        pPlayer->NotifyReceivedSync();
        pPlayer->IncrementPuresync();

        // Ignore this packet if he should be in a vehicle
        if (!pPlayer->GetOccupiedVehicle())
        {
            // Send a returnsync packet to the player that sent it
            // Only every 4 packets.
            if ((pPlayer->GetPuresyncCount() % 4) == 0)
                pPlayer->Send(CReturnSyncPacket(pPlayer));

            CLOCK("PlayerPuresync", "RelayPlayerPuresync");
            // Relay to other players
            RelayPlayerPuresync(Packet);
            UNCLOCK("PlayerPuresync", "RelayPlayerPuresync");

            CLOCK("PlayerPuresync", "DoHitDetection");
            // Run colpoint checks
            m_pColManager->DoHitDetection(pPlayer->GetPosition(), pPlayer);
            UNCLOCK("PlayerPuresync", "DoHitDetection");
        }
    }
}
