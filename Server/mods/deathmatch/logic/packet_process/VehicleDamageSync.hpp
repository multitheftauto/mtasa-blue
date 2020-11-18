void CGame::Packet_VehicleDamageSync(CVehicleDamageSyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Grab the vehicle
        CElement* pVehicleElement = CElementIDs::GetElement(Packet.m_Vehicle);
        if (pVehicleElement && IS_VEHICLE(pVehicleElement))
        {
            CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);

            // Is this guy the driver or syncer?
            if (pVehicle->GetSyncer() == pPlayer || pVehicle->GetOccupant(0) == pPlayer)
            {
                // Set the new damage model
                for (unsigned int i = 0; i < MAX_DOORS; ++i)
                {
                    if (Packet.m_damage.data.bDoorStatesChanged[i])
                        pVehicle->m_ucDoorStates[i] = Packet.m_damage.data.ucDoorStates[i];
                }
                for (unsigned int i = 0; i < MAX_WHEELS; ++i)
                {
                    if (Packet.m_damage.data.bWheelStatesChanged[i])
                        pVehicle->m_ucWheelStates[i] = Packet.m_damage.data.ucWheelStates[i];
                }
                for (unsigned int i = 0; i < MAX_PANELS; ++i)
                {
                    if (Packet.m_damage.data.bPanelStatesChanged[i])
                        pVehicle->m_ucPanelStates[i] = Packet.m_damage.data.ucPanelStates[i];
                }
                for (unsigned int i = 0; i < MAX_LIGHTS; ++i)
                {
                    if (Packet.m_damage.data.bLightStatesChanged[i])
                        pVehicle->m_ucLightStates[i] = Packet.m_damage.data.ucLightStates[i];
                }

                // Make a list of players to relay this packet to
                CSendList                      sendList;
                list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
                for (; iter != m_pPlayerManager->IterEnd(); iter++)
                {
                    CPlayer* pOther = *iter;
                    if (pOther != pPlayer && pOther->IsJoined())
                    {
                        if (pOther->GetDimension() == pPlayer->GetDimension())
                        {
                            // Newer clients only need sync if vehicle has no driver
                            if (pOther->GetBitStreamVersion() < 0x5D || pVehicle->GetOccupant(0) == NULL)
                            {
                                sendList.push_back(pOther);
                            }
                        }
                    }
                }

                CPlayerManager::Broadcast(Packet, sendList);
            }
        }
    }
}
