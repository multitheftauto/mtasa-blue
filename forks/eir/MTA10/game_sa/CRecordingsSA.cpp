/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRecordingsSA.cpp
*  PURPOSE:     GTA:SA car replay recordings
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static recordingEntry *VAR_RecordingEntries = (recordingEntry*)ARRAY_RecordingEntries;
static unsigned int *VAR_NumRecordings = (unsigned int*)LOC_NumRecordings;

CRecordingsSA::CRecordingsSA( void )
{
}

CRecordingsSA::~CRecordingsSA( void )
{
}

unsigned int CRecordingsSA::Register( const char *name )
{
    unsigned int id = 850;

    if ( !sscanf( name, "carrec%d", &id ) )
    {
        sscanf( name, "CARREC%d", &id );
    }

    unsigned int index = *VAR_NumRecordings;

    recordingEntry& entry = VAR_RecordingEntries[index];
    entry.m_id = id;
    entry.m_unk = 0;

    *VAR_NumRecordings = index + 1;
    return index;
}