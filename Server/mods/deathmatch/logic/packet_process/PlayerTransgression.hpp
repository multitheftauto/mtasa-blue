void CGame::Packet_PlayerTransgression(CPlayerTransgressionPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // If ac# not disabled on this server, do a kick
        if (!g_pGame->GetConfig()->IsDisableAC(SString("%d", Packet.m_uiLevel)))
        {
            CStaticFunctionDefinitions::KickPlayer(pPlayer, NULL, Packet.m_strMessage);
        }
    }
}
