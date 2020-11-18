void CGame::Packet_ProjectileSync(CProjectileSyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        CVector vecPosition = Packet.m_vecOrigin;
        if (Packet.m_OriginID != INVALID_ELEMENT_ID)
        {
            CElement* pOriginSource = CElementIDs::GetElement(Packet.m_OriginID);
            if (pOriginSource)
                vecPosition += pOriginSource->GetPosition();
        }

        // Make a list of players to send this packet to
        CSendList sendList;

        // Loop through all the players
        std::list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
        for (; iter != m_pPlayerManager->IterEnd(); iter++)
        {
            CPlayer* pSendPlayer = *iter;

            // Not the player we got the packet from?
            if (pSendPlayer != pPlayer)
            {
                // Grab this player's camera position
                CVector vecCameraPosition;
                pSendPlayer->GetCamera()->GetPosition(vecCameraPosition);

                // Is this players camera close enough to send?
                if (IsPointNearPoint3D(vecPosition, vecCameraPosition, MAX_PROJECTILE_SYNC_DISTANCE))
                {
                    // Send the packet to him
                    sendList.push_back(pSendPlayer);
                }
            }
        }
        CPlayerManager::Broadcast(Packet, sendList);
    }
}
