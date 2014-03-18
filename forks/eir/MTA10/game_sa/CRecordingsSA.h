/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRecordingsSA.h
*  PURPOSE:     GTA:SA car replay recordings
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RECORDINGS_SA_
#define _RECORDINGS_SA_

#define ARRAY_RecordingEntries  0x0097D880
#define LOC_NumRecordings       0x0097F630

struct recordingEntry
{
    unsigned int    m_id;           // 0
    unsigned int    m_unk;          // 4

    BYTE            m_pad[8];       // 8
};

class CRecordingsSA
{
public:
                            CRecordingsSA       ( void );
                            ~CRecordingsSA      ( void );

    unsigned int            Register            ( const char *name );
};

#endif //_RECORDINGS_SA_