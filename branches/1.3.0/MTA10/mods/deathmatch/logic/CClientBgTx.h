/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

typedef uint STransferHandle;

/*
namespace ETransferTag
{
    enum ETransferTagType
    {
        DATA,
    };
}
using ETransferTag::ETransferTagType;
*/

struct SClientTx
{
    uint uiId;
    uchar ucPacketId;
    CBuffer buffer;
    uint uiRate;
    ushort usCategory;
    uint uiReadPosition;
    bool bTransferStarted;
    bool bTransferFinishing;
};


class CClientBgTx
{
public:
    ZERO_ON_NEW
                        CClientBgTx                 ( void );
                        ~CClientBgTx                ( void );
    void                DoPulse                     ( void );
    STransferHandle     AddTransfer                 ( uchar ucPacketID, NetBitStreamInterface* pBitStream, uint uiRate, ushort usCategory );
    bool                CancelTransfer              ( STransferHandle handle );
    void                CancelAllTransfers          ( void );
    bool                GetTransferStatus           ( STransferHandle handle, uint* puiTickCountStart, uint* puiTickCountEnd );
    uint                GetlAllTransferStatus       ( void );
    void                Pause                       ( bool bPause );

protected:
    void                PostQueueRemove             ( void );

    std::list < SClientTx >     m_TxQueue;
    uint                        m_uiCurrentRate;
    CTickCount                  m_NextSentTime;
    const uint                  m_uiMaxPacketSize;
    uint                        m_uiNextTransferId;
};
