/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CNetworkStats.cpp
 *  PURPOSE:     Network stats logger
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CNetworkStats::CNetworkStats(CClientDisplayManager* pDisplayManager)
{
    m_pDisplayManager = pDisplayManager;
    Reset();
}

void CNetworkStats::Reset()
{
    m_ulLastUpdateTime = CClientTime::GetTime();

    NetStatistics stats;
    g_pNet->GetNetworkStatistics(&stats);

    m_fPacketSendRate = 0;
    m_fPacketReceiveRate = 0;
    m_fByteSendRate = 0;
    m_fByteReceiveRate = 0;

    m_uiLastPacketsSent = stats.packetsSent;
    m_uiLastPacketsReceived = stats.packetsReceived;
    m_ullLastBytesSent = stats.bytesSent;
    m_ullLastBytesReceived = stats.bytesReceived;
}

void CNetworkStats::Draw()
{
    // Time to update?
    if (CClientTime::GetTime() >= m_ulLastUpdateTime + 1000)
    {
        Update();
    }

    NetStatistics stats;
    g_pNet->GetNetworkStatistics(&stats);

    // Draw the background
    float fResWidth = static_cast<float>(g_pCore->GetGraphics()->GetViewportWidth());
    float fResHeight = static_cast<float>(g_pCore->GetGraphics()->GetViewportHeight());

    // Grab the bytes sent/recv and datarate in the proper unit
    SString strBytesSent = GetDataUnit(stats.bytesSent);
    SString strBytesRecv = GetDataUnit(stats.bytesReceived);
    SString strRecvRate = GetDataUnit(Round(m_fByteReceiveRate));
    SString strSendRate = GetDataUnit(Round(m_fByteSendRate));

    CClientPlayer*                 pLocalPlayer = g_pClientGame->GetLocalPlayer();
    bool                           bBulletSync = pLocalPlayer && g_pClientGame->GetWeaponTypeUsesBulletSync(pLocalPlayer->GetCurrentWeaponType());
    const SVehExtrapolateSettings& vehExtrapolateSettings = g_pClientGame->GetVehExtrapolateSettings();

    SString strBuffer;

    // Select a string to print
    bool bHoldingCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    if (!bHoldingCtrl)
    {
        strBuffer = SString(
            "Ping: %u ms\n"
            "Messages in send buffer: %u\n"
            "Messages in resend buffer: %u\n"
            "Packets recv: %u\n"
            "Packets sent: %u\n"
            "Overall packet loss: %.2f%%\n"
            "Current packet loss: %.2f%%\n"
            "Bytes recv: %s\n"
            "Bytes sent: %s\n"
            "Datarate in/out: %s/s / %s/s\n"
            "Packet rate in/out: %u / %u\n"
            "BPS limit by CC: %llu\n"
            "BPS limit by OB: %llu\n"
            "Current wep bullet sync: %s\n"
            "Veh. Extrapolate amount: %d%%\n"
            "Veh. Extrapolate max: %dms\n"
            "Alternate pulse order: %s\n"
            "Client: %s\n"
            "Server: %s\n",
            g_pNet->GetPing(), stats.messagesInSendBuffer, stats.messagesInResendBuffer, stats.packetsReceived, stats.packetsSent, stats.packetlossTotal,
            stats.packetlossLastSecond, strBytesRecv.c_str(), strBytesSent.c_str(), strRecvRate.c_str(), strSendRate.c_str(),
            (unsigned int)floor(m_fPacketReceiveRate + 0.5f), (unsigned int)floor(m_fPacketSendRate + 0.5f), stats.isLimitedByCongestionControl ? 1ULL : 0ULL,
            stats.isLimitedByOutgoingBandwidthLimit ? 1ULL : 0ULL, bBulletSync ? "On" : "Off", vehExtrapolateSettings.iScalePercent,
            vehExtrapolateSettings.iMaxMs, g_pClientGame->IsUsingAlternatePulseOrder() ? "Yes" : "No", *CStaticFunctionDefinitions::GetVersionSortable(),
            *g_pClientGame->GetServerVersionSortable());
    }
    else
    {
        NetRawStatistics& r = stats.raw;
        strBuffer += SString("messageSendBuffer {},{},{},{}\n", r.messageSendBuffer[0], r.messageSendBuffer[1], r.messageSendBuffer[2], r.messageSendBuffer[3]);
        strBuffer += SString("messagesSent {},{},{},{}\n", r.messagesSent[0], r.messagesSent[1], r.messagesSent[2], r.messagesSent[3]);
        strBuffer += SString("messageDataBitsSent %lld,%lld,%lld,%lld\n", r.messageDataBitsSent[0], r.messageDataBitsSent[1], r.messageDataBitsSent[2],
                             r.messageDataBitsSent[3]);
        strBuffer += SString("messageTotalBitsSent %lld,%lld,%lld,%lld\n", r.messageTotalBitsSent[0], r.messageTotalBitsSent[1], r.messageTotalBitsSent[2],
                             r.messageTotalBitsSent[3]);
        strBuffer += SString("packetsContainingOnlyAcknowlegements {}\n", r.packetsContainingOnlyAcknowlegements);
        strBuffer += SString("acknowlegementsSent {}\n", r.acknowlegementsSent);
        strBuffer += SString("acknowlegementsPending {}\n", r.acknowlegementsPending);
        strBuffer += SString("acknowlegementBitsSent {}\n", r.acknowlegementBitsSent);
        strBuffer += SString("packetsContainingOnlyAcksAndResends {}\n", r.packetsContainingOnlyAcknowlegementsAndResends);
        strBuffer += SString("messageResends {}\n", r.messageResends);
        strBuffer += SString("messageDataBitsResent {}\n", r.messageDataBitsResent);
        strBuffer += SString("messagesTotalBitsResent {}\n", r.messagesTotalBitsResent);
        strBuffer += SString("messagesOnResendQueue {}\n", r.messagesOnResendQueue);
        strBuffer += SString("numberOfUnsplitMessages {}\n", r.numberOfUnsplitMessages);
        strBuffer += SString("numberOfSplitMessages {}\n", r.numberOfSplitMessages);
        strBuffer += SString("totalSplits {}\n", r.totalSplits);
        strBuffer += SString("packetsSent {}\n", r.packetsSent);
        strBuffer += SString("encryptionBitsSent {}\n", r.encryptionBitsSent);
        strBuffer += SString("totalBitsSent {}\n", r.totalBitsSent);
        strBuffer += SString("sequencedMessagesOutOfOrder {}\n", r.sequencedMessagesOutOfOrder);
        strBuffer += SString("sequencedMessagesInOrder {}\n", r.sequencedMessagesInOrder);
        strBuffer += SString("orderedMessagesOutOfOrder {}\n", r.orderedMessagesOutOfOrder);
        strBuffer += SString("orderedMessagesInOrder {}\n", r.orderedMessagesInOrder);
        strBuffer += SString("packetsReceived {}\n", r.packetsReceived);
        strBuffer += SString("packetsWithBadCRCReceived {}\n", r.packetsWithBadCRCReceived);
        strBuffer += SString("bitsReceived {}\n", r.bitsReceived);
        strBuffer += SString("bitsWithBadCRCReceived {}\n", r.bitsWithBadCRCReceived);
        strBuffer += SString("acknowlegementsReceived {}\n", r.acknowlegementsReceived);
        strBuffer += SString("duplicateAcknowlegementsReceived {}\n", r.duplicateAcknowlegementsReceived);
        strBuffer += SString("messagesReceived {}\n", r.messagesReceived);
        strBuffer += SString("invalidMessagesReceived {}\n", r.invalidMessagesReceived);
        strBuffer += SString("duplicateMessagesReceived {}\n", r.duplicateMessagesReceived);
        strBuffer += SString("messagesWaitingForReassembly {}\n", r.messagesWaitingForReassembly);
        strBuffer += SString("internalOutputQueueSize {}\n", r.internalOutputQueueSize);
        strBuffer += SString("bitsPerSecond {:0.0f}\n", r.bitsPerSecond);
        strBuffer += SString("connectionStartTime {}\n", r.connectionStartTime);
        strBuffer += SString("bandwidthExceeded {}\n", r.bandwidthExceeded);
    }

    int iNumLines = 0;
    for (int i = strBuffer.length() - 1; i >= 0; i--)
        if (strBuffer[i] == '\n')
            iNumLines++;

    float fBackWidth = 310;
    float fBackHeight = (float)(iNumLines * 15);
    float fX = fResWidth - fBackWidth;
    float fY = 0.40f * fResHeight - iNumLines * 4;

    g_pCore->GetGraphics()->DrawRectQueued(fX - 10, fY - 10, fBackWidth + 10, fBackHeight + 20, 0x78000000, true);

    g_pCore->GetGraphics()->DrawStringQueued(fX, fY, fX, fY, 0xFFFFFFFF, strBuffer, 1, 1, DT_NOCLIP, NULL, true);
}

void CNetworkStats::Update()
{
    NetStatistics stats;
    g_pNet->GetNetworkStatistics(&stats);

    float delay = static_cast<float>(CClientTime::GetTime() - m_ulLastUpdateTime);
    m_ulLastUpdateTime = CClientTime::GetTime();

    m_fPacketSendRate = (stats.packetsSent - m_uiLastPacketsSent) * 1000 / delay;
    m_fPacketReceiveRate = (stats.packetsReceived - m_uiLastPacketsReceived) * 1000 / delay;
    m_fByteSendRate = (stats.bytesSent - m_ullLastBytesSent) * 1000 / delay;
    m_fByteReceiveRate = (stats.bytesReceived - m_ullLastBytesReceived) * 1000 / delay;

    m_uiLastPacketsSent = stats.packetsSent;
    m_uiLastPacketsReceived = stats.packetsReceived;
    m_ullLastBytesSent = stats.bytesSent;
    m_ullLastBytesReceived = stats.bytesReceived;
}
