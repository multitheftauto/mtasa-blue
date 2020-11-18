void CGame::Packet_PlayerJoinData(CPlayerJoinDataPacket& Packet)
{
    // Grab the nick
    const char* szNick = Packet.GetNick();
    if (szNick && szNick[0] != 0)
    {
        // Is the server passworded?
        bool bPasswordIsValid = true;
        if (m_pMainConfig->HasPassword())
        {
            // Grab the password hash from the packet
            const MD5& PacketHash = Packet.GetPassword();

            // Hash our password
            const std::string& strPassword = m_pMainConfig->GetPassword();
            MD5                ConfigHash;
            CMD5Hasher         Hasher;
            if (!strPassword.empty() && Hasher.Calculate(strPassword.c_str(), strPassword.length(), ConfigHash))
            {
                // Compare the hashes
                if (memcmp(&ConfigHash, &PacketHash, sizeof(MD5)) != 0)
                {
                    bPasswordIsValid = false;
                }
            }
        }

        // Add the player
        CPlayer* pPlayer = m_pPlayerManager->Create(Packet.GetSourceSocket());

        if (!pPlayer)
            return;

        if (pPlayer->GetID() == INVALID_ELEMENT_ID)
        {
            // Tell the console
            CLogger::LogPrintf("CONNECT: %s failed to connect (Player element could not be created.)\n", szNick);

            // Tell the player the problem
            return DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::ELEMENT_FAILURE);
        }

        // Set the bitstream version number for this connection
        pPlayer->SetBitStreamVersion(Packet.GetBitStreamVersion());
        g_pNetServer->SetClientBitStreamVersion(Packet.GetSourceSocket(), Packet.GetBitStreamVersion());

        // Get the serial number from the packet source
        NetServerPlayerID p = Packet.GetSourceSocket();
        SString           strSerial;
        SString           strExtra;
        CMtaVersion       strPlayerVersion;
        {
            SFixedString<32> strSerialTemp;
            SFixedString<64> strExtraTemp;
            SFixedString<32> strPlayerVersionTemp;
            g_pNetServer->GetClientSerialAndVersion(p, strSerialTemp, strExtraTemp, strPlayerVersionTemp);
            strSerial = SStringX(strSerialTemp);
            strExtra = SStringX(strExtraTemp);
            strPlayerVersion = SStringX(strPlayerVersionTemp);
        }
    #if MTASA_VERSION_TYPE < VERSION_TYPE_UNSTABLE
        if (atoi(ExtractVersionStringBuildNumber(Packet.GetPlayerVersion())) != 0)
        {
            // Use player version from packet if it contains a valid build number
            strPlayerVersion = Packet.GetPlayerVersion();
        }
    #endif

        SString strIP = pPlayer->GetSourceIP();
        SString strIPAndSerial("IP: %s  Serial: %s  Version: %s", strIP.c_str(), strSerial.c_str(), strPlayerVersion.c_str());
        if (!CheckNickProvided(szNick))            // check the nick is valid
        {
            // Tell the console
            CLogger::LogPrintf("CONNECT: %s failed to connect (Invalid nickname) (%s)\n", szNick, strIPAndSerial.c_str());

            // Tell the player the problem
            DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::INVALID_NICKNAME);
            return;
        }

        // Check the size of the nick
        size_t sizeNick = strlen(szNick);
        if (sizeNick >= MIN_PLAYER_NICK_LENGTH && sizeNick <= MAX_PLAYER_NICK_LENGTH)
        {
            // Someone here with the same name?
            CPlayer* pTempPlayer = m_pPlayerManager->Get(szNick);
            if (pTempPlayer)
            {
                // Same person? Quit the first and let this one join
                if (strcmp(pPlayer->GetSourceIP(), pTempPlayer->GetSourceIP()) == 0)
                {
                    // Two players could have the same IP, so see if the old player appears inactive before quitting them
                    if (pTempPlayer->UhOhNetworkTrouble())
                    {
                        pTempPlayer->Send(CPlayerDisconnectedPacket(SString("Supplanted by %s from %s", szNick, pPlayer->GetSourceIP())));
                        // Tell the console
                        CLogger::LogPrintf("DISCONNECT: %s Supplanted by (%s)\n", szNick, pTempPlayer->GetNick());
                        QuitPlayer(*pTempPlayer, CClient::QUIT_QUIT);
                        pTempPlayer = NULL;
                    }
                }
            }
            if (pTempPlayer == NULL)
            {
                // Correct version?
                if (Packet.GetNetVersion() == MTA_DM_NETCODE_VERSION)
                {
                    // If the password is valid
                    if (bPasswordIsValid)
                    {
                        // If he's not join flooding
                        if (!m_pMainConfig->GetJoinFloodProtectionEnabled() || !m_FloodProtect.AddConnect(SString("%x", Packet.GetSourceIP())))
                        {
                            // Set the nick and the game version
                            pPlayer->SetNick(szNick);
                            pPlayer->SetGameVersion(Packet.GetGameVersion());
                            pPlayer->SetMTAVersion(Packet.GetMTAVersion());
                            pPlayer->SetSerialUser(Packet.GetSerialUser());
                            pPlayer->SetSerial(strSerial, 0);
                            pPlayer->SetSerial(strExtra, 1);
                            pPlayer->SetPlayerVersion(strPlayerVersion);
                            pPlayer->SetDiscordJoinSecret(Packet.GetDiscordJoinSecret());

                            // Check if client must update
                            if (IsBelowMinimumClient(pPlayer->GetPlayerVersion()) && !pPlayer->ShouldIgnoreMinClientVersionChecks())
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (Client version is below minimum) (%s)\n", szNick,
                                                    strIPAndSerial.c_str());

                                // Tell the player
                                pPlayer->Send(CUpdateInfoPacket("Mandatory", CalculateMinClientRequirement()));
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::NO_REASON);
                                return;
                            }

                            // Check if client should optionally update
                            if (Packet.IsOptionalUpdateInfoRequired() && IsBelowRecommendedClient(pPlayer->GetPlayerVersion()) &&
                                !pPlayer->ShouldIgnoreMinClientVersionChecks())
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s advised to update (Client version is below recommended) (%s)\n", szNick,
                                                    strIPAndSerial.c_str());

                                // Tell the player
                                pPlayer->Send(CUpdateInfoPacket("Optional", GetConfig()->GetRecommendedClientVersion()));
                                DisconnectPlayer(this, *pPlayer, "");
                                return;
                            }

                            // Check the serial for validity
                            if (CBan* pBan = m_pBanManager->GetBanFromSerial(pPlayer->GetSerial().c_str()))
                            {
                                time_t  Duration = pBan->GetBanTimeRemaining();
                                SString strBanMessage = "Serial is banned";
                                SString strDurationDesc = pBan->GetDurationDesc();
                                if (strDurationDesc.length())
                                    strBanMessage += " (" + strDurationDesc + ")";

                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (%s) (%s)\n", szNick, strBanMessage.c_str(), strIPAndSerial.c_str());

                                // Tell the player he's banned
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::BANNED_SERIAL, Duration, pBan->GetReason().c_str());
                                return;
                            }

                            // Check the ip for banness
                            if (CBan* pBan = m_pBanManager->GetBanFromIP(strIP))
                            {
                                time_t  Duration = pBan->GetBanTimeRemaining();
                                SString strBanMessage;            // = "Serial is banned";
                                SString strDurationDesc = pBan->GetDurationDesc();
                                if (strDurationDesc.length())
                                    strBanMessage += " (" + strDurationDesc + ")";

                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (IP is banned%s) (%s)\n", szNick, strBanMessage.c_str(),
                                                    strIPAndSerial.c_str());

                                // Tell the player he's banned
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::BANNED_IP, Duration, pBan->GetReason().c_str());
                                return;
                            }

                            if (!pPlayer->GetSerialUser().empty() && m_pBanManager->IsAccountBanned(pPlayer->GetSerialUser().c_str()))
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (Account is banned) (%s)\n", szNick, strIPAndSerial.c_str());

                                CBan*   pBan = m_pBanManager->GetBanFromAccount(pPlayer->GetSerialUser().c_str());
                                time_t  Duration = 0;
                                SString strReason;
                                if (pBan)
                                {
                                    strReason = pBan->GetReason();
                                    Duration = pBan->GetBanTimeRemaining();
                                }

                                // Tell the player he's banned
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::BANNED_ACCOUNT, Duration, strReason.c_str());
                                return;
                            }

                        #if MTASA_VERSION_TYPE > VERSION_TYPE_UNSTABLE
                            if (Packet.GetPlayerVersion().length() > 0 && Packet.GetPlayerVersion() != pPlayer->GetPlayerVersion())
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (Version mismatch) (%s)\n", szNick, strIPAndSerial.c_str());

                                // Tell the player
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::VERSION_MISMATCH);
                                return;
                            }
                        #endif

                            PlayerCompleteConnect(pPlayer);
                        }
                        else
                        {
                            // Tell the console
                            CLogger::LogPrintf("CONNECT: %s failed to connect (Join flood) (%s)\n", szNick, strIPAndSerial.c_str());

                            // Tell the player the problem
                            DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::JOIN_FLOOD);
                        }
                    }
                    else
                    {
                        // Tell the console
                        CLogger::LogPrintf("CONNECT: %s failed to connect (Wrong password) (%s)\n", szNick, strIPAndSerial.c_str());

                        // Tell the player the password was wrong
                        DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::INVALID_PASSWORD);
                    }
                }
                else
                {
                    // Tell the console
                    CLogger::LogPrintf("CONNECT: %s failed to connect (Bad version) (%s)\n", szNick, strIPAndSerial.c_str());

                    // Tell the player the problem
                    SString strMessage;
                    ushort  usClientNetVersion = Packet.GetNetVersion();
                    ushort  usServerNetVersion = MTA_DM_NETCODE_VERSION;
                    ushort  usClientBranchId = usClientNetVersion >> 12;
                    ushort  usServerBranchId = usServerNetVersion >> 12;

                    CPlayerDisconnectedPacket::ePlayerDisconnectType eType;

                    if (usClientBranchId != usServerBranchId)
                    {
                        eType = CPlayerDisconnectedPacket::DIFFERENT_BRANCH;
                        strMessage = SString("(client: %X, server: %X)\n", usClientBranchId, usServerBranchId);
                    }
                    else if (MTASA_VERSION_BUILD == 0)
                    {
                        eType = CPlayerDisconnectedPacket::BAD_VERSION;
                        strMessage = SString("(client: %X, server: %X)\n", usClientNetVersion, usServerNetVersion);
                    }
                    else
                    {
                        if (usClientNetVersion < usServerNetVersion)
                        {
                            eType = CPlayerDisconnectedPacket::SERVER_NEWER;
                            strMessage = SString("(%d)\n", MTASA_VERSION_BUILD);
                        }
                        else
                        {
                            eType = CPlayerDisconnectedPacket::SERVER_OLDER;
                            strMessage = SString("(%d)\n", MTASA_VERSION_BUILD);
                        }
                    }
                    DisconnectPlayer(this, *pPlayer, eType, strMessage);
                }
            }
            else
            {
                // Tell the console
                CLogger::LogPrintf("CONNECT: %s failed to connect (Nick already in use) (%s)\n", szNick, strIPAndSerial.c_str());

                // Tell the player the problem
                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::NICK_CLASH);
            }
        }
        else
        {
            // Tell the console
            CLogger::LogPrintf("CONNECT: %s failed to connect (Invalid nickname)\n", pPlayer->GetSourceIP());

            // Tell the player the problem
            DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::INVALID_NICKNAME);
        }
    }
}
