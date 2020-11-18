void CGame::Packet_CameraSync(CCameraSyncPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        pPlayer->NotifyReceivedSync();

        CPlayerCamera* pCamera = pPlayer->GetCamera();

        if (Packet.m_bFixed)
        {
            pCamera->SetMode(CAMERAMODE_FIXED);
            pCamera->SetPosition(Packet.m_vecPosition);
            pCamera->SetLookAt(Packet.m_vecLookAt);
        }
        else
        {
            CPlayer* pTarget = GetElementFromId<CPlayer>(Packet.m_TargetID);
            if (!pTarget)
                pTarget = pPlayer;

            pCamera->SetMode(CAMERAMODE_PLAYER);
            pCamera->SetTarget(pTarget);
        }
    }
}