void CGame::Packet_DiscordJoin(CDiscordJoinPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        CLuaArguments Arguments;
        Arguments.PushBoolean(false);
        Arguments.PushString(Packet.GetSecret());
        pPlayer->CallEvent("onPlayerDiscordJoin", Arguments, NULL);
    }
}