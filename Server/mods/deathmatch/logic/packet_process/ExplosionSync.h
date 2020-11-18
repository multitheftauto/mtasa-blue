void CGame::Packet_ExplosionSync(CExplosionSyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        bool          bBroadcast = true;
        ElementID     OriginID = Packet.m_OriginID;
        unsigned char ucType = Packet.m_ucType;
        CVector       vecPosition = Packet.m_vecPosition;
        if (OriginID != INVALID_ELEMENT_ID)
        {
            CElement* pOrigin = CElementIDs::GetElement(OriginID);
            // Do we have an origin source?
            if (pOrigin)
            {
                // Is the source of the explosion a vehicle?
                switch (pOrigin->GetType())
                {
                    case CElement::PLAYER:
                    {
                        // Correct our position vector
                        CVehicle* pVehicle = static_cast<CPlayer*>(pOrigin)->GetOccupiedVehicle();
                        if (pVehicle)
                        {
                            // Use the vehicle's position?
                            vecPosition += pVehicle->GetPosition();
                        }
                        else
                        {
                            // Use the player's position
                            vecPosition += pOrigin->GetPosition();
                        }
                        break;
                    }
                    case CElement::VEHICLE:
                    {
                        // Correct our position vector
                        vecPosition += pOrigin->GetPosition();

                        // Has the vehicle blown up?
                        switch (ucType)
                        {
                            case 4:             // EXP_TYPE_CAR
                            case 5:             // EXP_TYPE_CAR_QUICK
                            case 6:             // EXP_TYPE_BOAT
                            case 7:             // EXP_TYPE_HELI
                            case 12:            // EXP_TYPE_TINY - RC Vehicles
                            {
                                CVehicle* pVehicle = static_cast<CVehicle*>(pOrigin);
                                // Is this vehicle not already blown?
                                if (pVehicle->GetIsBlown() == false)
                                {
                                    pVehicle->SetIsBlown(true);

                                    // Call the onVehicleExplode event
                                    CLuaArguments Arguments;
                                    pVehicle->CallEvent("onVehicleExplode", Arguments);
                                    // Update our engine State
                                    pVehicle->SetEngineOn(false);
                                }
                                else
                                {
                                    bBroadcast = false;
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        if (bBroadcast)
        {
            // Make a list of players to send this packet to
            CSendList sendList;

            // Loop through all the players
            std::list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
            for (; iter != m_pPlayerManager->IterEnd(); iter++)
            {
                CPlayer* pSendPlayer = *iter;

                // We tell the reporter to create the explosion too
                // Grab this player's camera position
                CVector vecCameraPosition;
                pSendPlayer->GetCamera()->GetPosition(vecCameraPosition);

                // Is this players camera close enough to send?
                if (IsPointNearPoint3D(vecPosition, vecCameraPosition, MAX_EXPLOSION_SYNC_DISTANCE))
                {
                    // Send the packet to him
                    sendList.push_back(pSendPlayer);
                }
            }

            CPlayerManager::Broadcast(Packet, sendList);
        }
    }
}