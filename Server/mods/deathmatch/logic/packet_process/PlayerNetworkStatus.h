void CGame::Packet_PlayerNetworkStatus(CPlayerNetworkStatusPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        CLuaArguments Arguments;
        Arguments.PushNumber(Packet.m_ucType);             // 0-interruption began  1-interruption end
        Arguments.PushNumber(Packet.m_uiTicks);            // Ticks since interruption start
        pPlayer->CallEvent("onPlayerNetworkStatus", Arguments, NULL);
    }
}