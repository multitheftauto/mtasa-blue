void CGame::Packet_VehiclePuresync(CVehiclePuresyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        pPlayer->NotifyReceivedSync();

        // Grab the vehicle
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
        if (pVehicle)
        {
            // Send a returnsync packet to the player that sent it
            pPlayer->Send(CReturnSyncPacket(pPlayer));

            // Increment counter to spread out damage info sends
            pVehicle->m_uiDamageInfoSendPhase++;

            // Increment counter to spread out damage info sends
            pVehicle->m_uiDamageInfoSendPhase++;

            CLOCK("VehiclePuresync", "RelayPlayerPuresync");
            // Relay to other players
            RelayPlayerPuresync(Packet);
            UNCLOCK("VehiclePuresync", "RelayPlayerPuresync");

            CVehicle* pTrailer = pVehicle->GetTowedVehicle();

            // Run colpoint checks
            CLOCK("VehiclePuresync", "DoHitDetection");
            m_pColManager->DoHitDetection(pPlayer->GetPosition(), pPlayer);
            m_pColManager->DoHitDetection(pVehicle->GetPosition(), pVehicle);
            while (pTrailer)
            {
                m_pColManager->DoHitDetection(pTrailer->GetPosition(), pTrailer);
                pTrailer = pTrailer->GetTowedVehicle();
            }
            UNCLOCK("VehiclePuresync", "DoHitDetection");
        }
    }
}