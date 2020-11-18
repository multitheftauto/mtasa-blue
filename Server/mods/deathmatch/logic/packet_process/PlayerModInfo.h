void CGame::Packet_PlayerModInfo(CPlayerModInfoPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Make itemList table
        CLuaArguments resultItemList;
        for (std::vector<SModInfoItem>::iterator iter = Packet.m_ModInfoItemList.begin(); iter != Packet.m_ModInfoItemList.end(); ++iter)
        {
            const SModInfoItem& in = *iter;

            // Make item table
            CLuaArguments resultItem;

            resultItem.PushString("id");
            resultItem.PushNumber(in.usId);

            resultItem.PushString("name");
            resultItem.PushString(in.strName);

            resultItem.PushString("hash");
            resultItem.PushNumber(in.uiHash);

            if (in.bHasSize)
            {
                resultItem.PushString("sizeX");
                resultItem.PushNumber(in.vecSize.fX - fmod((double)in.vecSize.fX, 0.01));

                resultItem.PushString("sizeY");
                resultItem.PushNumber(in.vecSize.fY - fmod((double)in.vecSize.fY, 0.01));

                resultItem.PushString("sizeZ");
                resultItem.PushNumber(in.vecSize.fZ - fmod((double)in.vecSize.fZ, 0.01));

                resultItem.PushString("originalSizeX");
                resultItem.PushNumber(in.vecOriginalSize.fX - fmod((double)in.vecOriginalSize.fX, 0.01));

                resultItem.PushString("originalSizeY");
                resultItem.PushNumber(in.vecOriginalSize.fY - fmod((double)in.vecOriginalSize.fY, 0.01));

                resultItem.PushString("originalSizeZ");
                resultItem.PushNumber(in.vecOriginalSize.fZ - fmod((double)in.vecOriginalSize.fZ, 0.01));
            }

            if (in.bHasHashInfo)
            {
                resultItem.PushString("length");
                resultItem.PushNumber(in.uiShortBytes);

                resultItem.PushString("md5");
                resultItem.PushString(in.strShortMd5);

                resultItem.PushString("sha256");
                resultItem.PushString(in.strShortSha256);

                resultItem.PushString("paddedLength");
                resultItem.PushNumber(in.uiLongBytes);

                resultItem.PushString("paddedMd5");
                resultItem.PushString(in.strLongMd5);

                resultItem.PushString("paddedSha256");
                resultItem.PushString(in.strLongSha256);
            }

            resultItemList.PushNumber(resultItemList.Count() / 2 + 1);
            resultItemList.PushTable(&resultItem);
        }

        CLuaArguments Arguments;
        Arguments.PushString(Packet.m_strInfoType);
        Arguments.PushTable(&resultItemList);
        pPlayer->CallEvent("onPlayerModInfo", Arguments);
    }
}