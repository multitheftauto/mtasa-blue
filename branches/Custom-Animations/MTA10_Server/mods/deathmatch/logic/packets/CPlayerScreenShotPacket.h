/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerScreenShotPacket.h
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerScreenShotPacket : public CPacket
{
public:

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_SCREENSHOT; };
    unsigned long                           GetFlags                    ( void ) const                  { return 0; };  // Not used

    bool                                    Read                        ( NetBitStreamInterface& BitStream );

    uchar                                   m_ucStatus;                 // 1 = has image, 2 = minimized, 3 = disabled

    // With every packet if status is 1
    ushort                                  m_usScreenShotId;
    ushort                                  m_usPartNumber;
    CBuffer                                 m_buffer;

    // When uiPartNumber is 0:
    long long                               m_llServerGrabTime;
    uint                                    m_uiTotalBytes;
    ushort                                  m_usTotalParts;
    SString                                 m_strResourceName;
    SString                                 m_strTag;
};
