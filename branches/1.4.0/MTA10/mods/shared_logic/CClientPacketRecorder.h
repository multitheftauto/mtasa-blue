/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPacketRecorder.h
*  PURPOSE:     Client packet recorder and "demo" replay class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

class CClientPacketRecorder;

#ifndef __CCLIENTPACKETRECORDER_H
#define __CCLIENTPACKETRECORDER_H

#include "CClientManager.h"
#include <ctime>

class CClientPacketRecorder
{
    friend class CClientManager;

public:
                        CClientPacketRecorder       ( CClientManager* pManager );
                        ~CClientPacketRecorder      ( void );

    void                SetPacketHandler            ( PPACKETHANDLER pfnPacketHandler );

    void                StartPlayback               ( const char* szInput, bool bFrameBased );
    void                StartRecord                 ( const char* szOutput, bool bFrameBased );
    void                Stop                        ( void );

    bool                IsPlaying                   ( void );
    bool                IsRecording                 ( void );
    bool                IsPlayingOrRecording        ( void );
    bool                IsFrameBased                ( void );

    void                SetFrameSkip                ( unsigned int uiFrameSkip );

    void                RecordPacket                ( unsigned char ucPacketID, NetBitStreamInterface& bitStream );
    void                RecordLocalData             ( CClientPlayer* pLocalPlayer );

private:
    void                ReadLocalData               ( FILE* pFile );

    void                DoPulse                     ( void );

    CClientManager*     m_pManager;
    char*               m_szFilename;
    PPACKETHANDLER      m_pfnPacketHandler;

    long                m_lRelative;
    long                m_lFrames;

    bool                m_bPlaying;
    bool                m_bRecording;
    bool                m_bFrameBased;

    unsigned long       m_ulCurrentOffset;
    long                m_lNextPacketTime;

    unsigned int        m_uiFrameSkip;
};

#endif
