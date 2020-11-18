void CGame::Packet_PlayerDiagnostic(CPlayerDiagnosticPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        if (Packet.m_uiLevel == 236)
        {
            // Handle special info
            std::vector<SString> parts;
            Packet.m_strMessage.Split(",", parts);
            if (parts.size() > 3)
            {
                pPlayer->m_strDetectedAC = parts[0].Replace("|", ",");
                pPlayer->m_uiD3d9Size = atoi(parts[1]);
                pPlayer->m_strD3d9Md5 = parts[2];
                pPlayer->m_strD3d9Sha256 = parts[3];
            }
        }
        else if (Packet.m_uiLevel >= 1000 || g_pGame->GetConfig()->IsEnableDiagnostic(SString("%d", Packet.m_uiLevel)))
        {
            // If diagnosticis enabled on this server, log it
            SString strMessageCombo("DIAGNOSTIC: %s #%d %s\n", pPlayer->GetNick(), Packet.m_uiLevel, Packet.m_strMessage.c_str());
            CLogger::LogPrint(strMessageCombo);
        }
    }
}
