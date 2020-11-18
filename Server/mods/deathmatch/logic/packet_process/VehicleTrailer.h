void CGame::Packet_VehicleTrailer(CVehicleTrailerPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Spawned?
        if (pPlayer->IsSpawned())
        {
            // Grab the vehicle with the chosen ID
            ElementID ID = Packet.GetVehicle();
            ElementID TrailerID = Packet.GetAttachedVehicle();
            bool      bAttached = Packet.GetAttached();

            CElement* pVehicleElement = CElementIDs::GetElement(ID);
            if (pVehicleElement && IS_VEHICLE(pVehicleElement))
            {
                CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);

                pVehicleElement = CElementIDs::GetElement(TrailerID);
                if (pVehicleElement && IS_VEHICLE(pVehicleElement))
                {
                    CVehicle* pTrailer = static_cast<CVehicle*>(pVehicleElement);

                    // If we're attaching
                    if (bAttached)
                    {
                        // Do we already have a trailer?
                        CVehicle* pPresentTrailer = pVehicle->GetTowedVehicle();
                        if (pPresentTrailer)
                        {
                            pPresentTrailer->SetTowedByVehicle(NULL);
                            pVehicle->SetTowedVehicle(NULL);

                            // Detach this one
                            CVehicleTrailerPacket DetachPacket(pVehicle, pPresentTrailer, false);
                            DetachPacket.SetSourceElement(pPlayer);
                            m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);
                        }

                        // Is our trailer already attached to something?
                        CVehicle* pPresentVehicle = pTrailer->GetTowedByVehicle();
                        if (pPresentVehicle)
                        {
                            pTrailer->SetTowedByVehicle(NULL);
                            pPresentVehicle->SetTowedVehicle(NULL);

                            // Detach from this one
                            CVehicleTrailerPacket DetachPacket(pPresentVehicle, pTrailer, false);
                            DetachPacket.SetSourceElement(pPlayer);
                            m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);
                        }

                        // Attach them
                        pVehicle->SetTowedVehicle(pTrailer);
                        pTrailer->SetTowedByVehicle(pVehicle);

                        // Make sure the un-occupied syncer of the trailer is this driver
                        m_pUnoccupiedVehicleSync->OverrideSyncer(pTrailer, pPlayer);

                        // Broadcast this packet to everyone else
                        m_pPlayerManager->BroadcastOnlyJoined(Packet, pPlayer);

                        // Execute the attach trailer script function
                        CLuaArguments Arguments;
                        Arguments.PushElement(pVehicle);
                        bool bContinue = pTrailer->CallEvent("onTrailerAttach", Arguments);

                        if (!bContinue)
                        {
                            // Detach them
                            CVehicleTrailerPacket DetachPacket(pVehicle, pTrailer, false);
                            DetachPacket.SetSourceElement(pPlayer);
                            m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);
                        }
                    }
                    else            // If we're detaching
                    {
                        // Make sure they're attached
                        if (pVehicle->GetTowedVehicle() == pTrailer && pTrailer->GetTowedByVehicle() == pVehicle)
                        {
                            // Detach them
                            pVehicle->SetTowedVehicle(NULL);
                            pTrailer->SetTowedByVehicle(NULL);

                            // Tell everyone else to detach them
                            m_pPlayerManager->BroadcastOnlyJoined(Packet, pPlayer);

                            // Execute the detach trailer script function
                            CLuaArguments Arguments;
                            Arguments.PushElement(pVehicle);
                            pTrailer->CallEvent("onTrailerDetach", Arguments);
                        }
                    }
                }
            }
        }
    }
}