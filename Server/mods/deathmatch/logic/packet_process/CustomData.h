void CGame::Packet_CustomData(CCustomDataPacket& Packet)
{
    // Got a valid source?
    CPlayer* pSourcePlayer = Packet.GetSourcePlayer();
    if (pSourcePlayer)
    {
        // Grab the element
        ElementID ID = Packet.GetElementID();
        CElement* pElement = CElementIDs::GetElement(ID);
        if (pElement)
        {
            // Change the data
            const char*   szName = Packet.GetName();
            CLuaArgument& Value = Packet.GetValue();

            // Ignore if the wrong length
            if (strlen(szName) > MAX_CUSTOMDATA_NAME_LENGTH)
            {
                CLogger::ErrorPrintf("Received oversized custom data name from %s (%s)", Packet.GetSourcePlayer()->GetNick(),
                                     *SStringX(szName).Left(MAX_CUSTOMDATA_NAME_LENGTH + 1));
                return;
            }

            ESyncType lastSyncType = ESyncType::BROADCAST;
            pElement->GetCustomData(szName, false, &lastSyncType);

            if (lastSyncType != ESyncType::LOCAL)
            {
                // Tell our clients to update their data. Send to everyone but the one we got this packet from.
                unsigned short usNameLength = static_cast<unsigned short>(strlen(szName));
                CBitStream     BitStream;
                BitStream.pBitStream->WriteCompressed(usNameLength);
                BitStream.pBitStream->Write(szName, usNameLength);
                Value.WriteToBitStream(*BitStream.pBitStream);
                if (lastSyncType == ESyncType::BROADCAST)
                    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(pElement, SET_ELEMENT_DATA, *BitStream.pBitStream), pSourcePlayer);
                else
                    m_pPlayerManager->BroadcastOnlySubscribed(CElementRPCPacket(pElement, SET_ELEMENT_DATA, *BitStream.pBitStream), pElement, szName,
                                                              pSourcePlayer);

                CPerfStatEventPacketUsage::GetSingleton()->UpdateElementDataUsageRelayed(szName, m_pPlayerManager->Count(),
                                                                                         BitStream.pBitStream->GetNumberOfBytesUsed());
            }

            pElement->SetCustomData(szName, Value, lastSyncType, pSourcePlayer);
        }
    }
}
