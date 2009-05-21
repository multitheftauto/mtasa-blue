/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/net/cnet.h
*  PURPOSE:		Network subsystem interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNET_H
#define __CNET_H

#include <string>
#include "net_common.h"
#include "net/bitstream.h"
#include "CNetHTTPDownloadManagerInterface.h"

class CNet
{
public:
    virtual void                        ResetNetwork                ( void ) = 0;
    virtual bool                        StartNetwork                ( const char* szServerHost, unsigned short usServerPort, const char* szServerPassword = NULL ) = 0;
    virtual void                        StopNetwork                 ( void ) = 0;

    virtual void                        SetFakeLag                  ( unsigned short usMaxPacketsOnWire, unsigned short usMinExtraPing, unsigned short usExtraPingVariance ) = 0;

    virtual bool                        IsConnected                 ( void ) = 0;

    virtual void                        DoPulse                     ( void ) = 0;

    virtual void                        RegisterPacketHandler       ( PPACKETHANDLER pfnPacketHandler, bool bIsPrimaryPacketHandler = true ) = 0;

    virtual NetBitStreamInterface*      AllocateNetBitStream        ( void ) = 0;
    virtual void                        DeallocateNetBitStream      ( NetBitStreamInterface* bitStream ) = 0;
    virtual bool                        SendPacket                  ( unsigned char ucPacketID, NetBitStreamInterface* bitStream, NetPacketPriority packetPriority = PACKET_PRIORITY_HIGH, NetPacketReliability packetReliability = PACKET_RELIABILITY_RELIABLE, NetPacketOrdering packetOrdering = PACKET_ORDERING_GAME ) = 0;

    virtual void                        SetClientPort               ( unsigned short usClientPort ) = 0;
    virtual char *                      GetConnectedServer          ( void )=0;

    virtual unsigned int                GetMessagesInSendBuffer     ( void ) = 0;
    virtual unsigned int                GetMessagesSent             ( void ) = 0;
    virtual unsigned int                GetMessagesWaitingAck       ( void ) = 0;
    virtual unsigned int                GetMessagesResent           ( void ) = 0;
    virtual unsigned int                GetAcknowledgesSent         ( void ) = 0;
    virtual unsigned int                GetAcknowledgesPending      ( void ) = 0;
    virtual unsigned int                GetAcknowledgesReceived     ( void ) = 0;
    virtual unsigned int                GetPacketsSent              ( void ) = 0;
    virtual float                       GetPacketLoss               ( void ) = 0;
    virtual unsigned int                GetGoodPacketsReceived      ( void ) = 0;
    virtual unsigned int                GetBadPacketsReceived       ( void ) = 0;
    virtual unsigned int                GetBitsSent                 ( void ) = 0;
    virtual unsigned int                GetBitsReceived             ( void ) = 0;
    virtual float                       GetCompressionRatio         ( void ) = 0;
    virtual float                       GetDecompressionRatio       ( void ) = 0;
    virtual void                        GetPacketLogData            ( unsigned long* ulBytes, unsigned long* ulCount ) = 0;

    virtual int                         GetPing                     ( void ) = 0;
    virtual unsigned long		        GetTime			            ( void ) = 0;

    virtual unsigned int                GetMTUSize                  ( void ) = 0;
    virtual bool                        SetMTUSize                  ( unsigned int uiSize ) = 0;

	virtual const char *				GetLocalIP					( void ) = 0;
    virtual void                        GetSerial                   ( char* szSerial, size_t maxLength ) = 0;

    virtual unsigned char               GetConnectionError          ( void ) = 0;
    virtual void                        SetConnectionError          ( unsigned char ucConnectionError ) = 0;

    virtual unsigned char               GetImmediateError           ( void ) = 0;
    virtual void                        SetImmediateError           ( unsigned char ucImmediateError ) = 0;

	virtual void                        Reset                       ( void ) = 0;

    virtual CNetHTTPDownloadManagerInterface*   GetHTTPDownloadManager          ( void ) = 0;
};

#endif
