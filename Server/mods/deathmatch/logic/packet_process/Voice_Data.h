void CGame::Packet_Voice_Data(CVoiceDataPacket& Packet)
{
    unsigned short usDataLength = 0;

    if (m_pMainConfig->IsVoiceEnabled())            // Shouldn't really be receiving voice packets at all if voice is disabled
    {
        usDataLength = Packet.GetDataLength();

        if (usDataLength > 0)
        {
            CPlayer* pPlayer = Packet.GetSourcePlayer();

            if (pPlayer)
            {
                if (pPlayer->IsVoiceMuted())            // Shouldn't be receiving voice packets, player should be muted client side
                    return;

                // Is it the start of the voice stream?
                if (pPlayer->GetVoiceState() == VOICESTATE_IDLE)
                {
                    // Trigger the related event
                    CLuaArguments Arguments;
                    bool          bEventTriggered = pPlayer->CallEvent("onPlayerVoiceStart", Arguments, pPlayer);

                    if (!bEventTriggered)            // Was the event cancelled?
                    {
                        pPlayer->SetVoiceState(VOICESTATE_TRANSMITTING_IGNORED);
                        return;
                    }

                    // Our voice state has changed
                    pPlayer->SetVoiceState(VOICESTATE_TRANSMITTING);
                }

                if (pPlayer->GetVoiceState() ==
                    VOICESTATE_TRANSMITTING)            // If we reach here, and we're still in idle state, then the event was cancelled
                {
                    const unsigned char* pBuffer = Packet.GetData();
                    CVoiceDataPacket     VoicePacket(pPlayer, pBuffer, usDataLength);

                    // Make list of players to send the voice packet to
                    std::set<CPlayer*> playerSendMap;

                    list<CElement*>::const_iterator iter = pPlayer->IterBroadcastListBegin();
                    for (; iter != pPlayer->IterBroadcastListEnd(); iter++)
                    {
                        CElement* pBroadcastElement = *iter;
                        if (IS_TEAM(pBroadcastElement))
                        {
                            // Add team members
                            CTeam*                         pTeam = static_cast<CTeam*>(pBroadcastElement);
                            list<CPlayer*>::const_iterator iter = pTeam->PlayersBegin();
                            for (; iter != pTeam->PlayersEnd(); iter++)
                                playerSendMap.insert(*iter);
                        }
                        else if (IS_PLAYER(pBroadcastElement))
                        {
                            // Add a player
                            playerSendMap.insert(static_cast<CPlayer*>(pBroadcastElement));
                        }
                        else
                        {
                            // Add element decendants
                            std::vector<CPlayer*> descendantList;
                            pBroadcastElement->GetDescendantsByType(descendantList, CElement::PLAYER);
                            for (std::vector<CPlayer*>::const_iterator iter = descendantList.begin(); iter != descendantList.end(); ++iter)
                            {
                                playerSendMap.insert(*iter);
                            }
                        }
                    }

                    // Filter out ourselves and ignored
                    for (std::set<CPlayer*>::iterator iter = playerSendMap.begin(); iter != playerSendMap.end();)
                    {
                        if (*iter == pPlayer || (*iter)->IsPlayerIgnoringElement(pPlayer))
                            playerSendMap.erase(iter++);
                        else
                            ++iter;
                    }

                    // Send to all players in the send list
                    CPlayerManager::Broadcast(VoicePacket, playerSendMap);
                }
            }
        }
    }
}