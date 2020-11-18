void CGame::Packet_Voice_End(CVoiceEndPacket& Packet)
{
    if (m_pMainConfig->IsVoiceEnabled())            // Shouldn't really be receiving voice packets at all if voice is disabled
    {
        CPlayer* pPlayer = Packet.GetSourcePlayer();

        if (pPlayer)
        {
            CLuaArguments Arguments;
            pPlayer->CallEvent("onPlayerVoiceStop", Arguments, pPlayer);

            // Reset our voice state
            pPlayer->SetVoiceState(VOICESTATE_IDLE);

            CVoiceEndPacket EndPacket(pPlayer);

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
            CPlayerManager::Broadcast(EndPacket, playerSendMap);
        }
    }
}
