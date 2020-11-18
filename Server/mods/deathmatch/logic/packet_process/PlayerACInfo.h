void CGame::Packet_PlayerACInfo(CPlayerACInfoPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        CLuaArguments acList;
        for (uint i = 0; i < Packet.m_IdList.size(); i++)
        {
            acList.PushNumber(i + 1);
            acList.PushNumber(Packet.m_IdList[i]);
        }

        CLuaArguments Arguments;
        Arguments.PushTable(&acList);
        Arguments.PushNumber(Packet.m_uiD3d9Size);
        Arguments.PushString(Packet.m_strD3d9MD5);
        Arguments.PushString(Packet.m_strD3d9SHA256);
        pPlayer->CallEvent("onPlayerACInfo", Arguments);
    }
}