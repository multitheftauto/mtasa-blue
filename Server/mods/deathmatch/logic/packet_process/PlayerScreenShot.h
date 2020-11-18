void CGame::Packet_PlayerScreenShot(CPlayerScreenShotPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        if (Packet.m_ucStatus != EPlayerScreenShotResult::SUCCESS)
        {
            // disabled, minimized or error
            if (Packet.m_pResource)
            {
                CLuaArguments Arguments;
                Arguments.PushResource(Packet.m_pResource);
                Arguments.PushString(EnumToString((EPlayerScreenShotResultType)Packet.m_ucStatus));
                Arguments.PushBoolean(false);
                Arguments.PushNumber(static_cast<double>(Packet.m_llServerGrabTime));
                Arguments.PushString(Packet.m_strTag);
                Arguments.PushString(Packet.m_strError);
                pPlayer->CallEvent("onPlayerScreenShot", Arguments);
            }
        }
        else if (Packet.m_ucStatus == EPlayerScreenShotResult::SUCCESS)
        {
            // Get in-progress info
            SScreenShotInfo& info = pPlayer->GetScreenShotInfo();

            // Validate
            if (!info.bInProgress || info.usNextPartNumber != Packet.m_usPartNumber || info.usScreenShotId != Packet.m_usScreenShotId)
            {
                info.bInProgress = false;
                info.buffer.Clear();

                // Check if new start
                if (Packet.m_usPartNumber == 0)
                {
                    info.bInProgress = true;
                    info.usNextPartNumber = 0;
                    info.usScreenShotId = Packet.m_usScreenShotId;

                    info.llTimeStamp = Packet.m_llServerGrabTime;
                    info.uiTotalBytes = Packet.m_uiTotalBytes;
                    info.usTotalParts = Packet.m_usTotalParts;
                    info.usResourceNetId = Packet.m_pResource ? Packet.m_pResource->GetNetID() : INVALID_RESOURCE_NET_ID;
                    info.strTag = Packet.m_strTag;
                }
            }

            // Add data if valid
            if (info.bInProgress)
            {
                info.buffer += Packet.m_buffer;
                info.usNextPartNumber++;

                // Finished?
                if (info.usNextPartNumber == info.usTotalParts)
                {
                    CResource* pResource = g_pGame->GetResourceManager()->GetResourceFromNetID(info.usResourceNetId);
                    if (pResource && info.uiTotalBytes == info.buffer.GetSize())
                    {
                        CLuaArguments Arguments;
                        Arguments.PushResource(pResource);
                        Arguments.PushString("ok");
                        Arguments.PushString(std::string(info.buffer.GetData(), info.buffer.GetSize()));
                        Arguments.PushNumber(static_cast<double>(info.llTimeStamp));
                        Arguments.PushString(info.strTag);
                        pPlayer->CallEvent("onPlayerScreenShot", Arguments);
                    }

                    info.bInProgress = false;
                    info.buffer.Clear();
                }
            }
        }
    }
}