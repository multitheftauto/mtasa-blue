/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamerSA.cpp
*  PURPOSE:     GTA:SA entity streamer extensions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define MAX_RENDER_OBJECTS  0x10000

unsigned int *const VAR_renderObjectCount = (unsigned int*)0x00B76840;
unsigned int *const VAR_renderObject2Count = (unsigned int*)0x00B76844;
unsigned int *const VAR_renderObject3Count = (unsigned int*)0x00B76838;
unsigned int *const VAR_renderObject4Count = (unsigned int*)0x00B7683C;
CEntitySAInterface *renderObjects[MAX_RENDER_OBJECTS];
CEntitySAInterface *renderObjects2[MAX_RENDER_OBJECTS];
CEntitySAInterface *renderObjects3[MAX_RENDER_OBJECTS];
CEntitySAInterface *renderObjects4[MAX_RENDER_OBJECTS];

static const DWORD entityDestructor = 0x00404180;
static const DWORD continue_deletion = 0x00404139;

struct streamSectorEntry
{
    CBuildingSAInterface*   entity;
    streamSectorEntry*      next;
};

struct streamRepeatSectorEntry
{
    BYTE                    m_pad[8];   // 0
    streamSectorEntry*      m_entry;    // 8
};

static void __cdecl _CBuilding__RemoveReferences( CBuildingSAInterface *building )
{
    // Scan buildings array
    for ( unsigned int n = 0; n < 2 * NUM_StreamSectorRows * NUM_StreamSectorCols; n++ )
    {
        streamSectorEntry *entry = *((streamSectorEntry**)ARRAY_StreamSectors + n);

        while ( entry )
        {
            if ( entry->entity == building )
            {
                // Deliberate it
                entry->entity = NULL; 
                *((DWORD**)ARRAY_StreamSectors + n) = NULL;
            }

            entry = entry->next;
        }
    }

    // Scan big buildings array
    for ( unsigned int n = 0; n < NUM_StreamRepeatSectorRows * NUM_StreamRepeatSectorCols; n++ )
    {
        streamRepeatSectorEntry *entry = (streamRepeatSectorEntry*)ARRAY_StreamRepeatSectors + n;
        streamSectorEntry *streamEntry = entry->m_entry;

        while ( streamEntry )
        {
            if ( streamEntry->entity == building )
            {
                // Deliberate it
                streamEntry->entity = NULL;
                entry->m_entry = NULL;
            }

            streamEntry = streamEntry->next;
        }
    }
}

static void __declspec(naked) _CBuilding__Delete()
{
    // We have to remove the building from the internal streamer
    __asm
    {
        push esi
        push edi
        mov edi,ecx
        call entityDestructor
        push edi
        call _CBuilding__RemoveReferences
        pop edi
        jmp continue_deletion
    }
}

void Streamer_Init( void )
{
    // Patch render limitations
    MemPut( (unsigned int*)0x00553923, renderObjects );
    MemPut( (unsigned int*)0x005534F5, renderObjects );
    MemPut( (unsigned int*)0x00553CB3, renderObjects );

    MemPut( (unsigned int*)0x00553529, renderObjects2 );
    MemPut( (unsigned int*)0x00553944, renderObjects2 );
    MemPut( (unsigned int*)0x00553A53, renderObjects2 );
    MemPut( (unsigned int*)0x00553B03, renderObjects2 );

    MemPut( (unsigned int*)0x00553965, renderObjects3 );

    MemPut( (unsigned int*)0x00553986, renderObjects4 );
    MemPut( (unsigned int*)0x00554AC7, renderObjects4 );

    // Hook special fixes
    HookInstall( 0x00404130, (DWORD)_CBuilding__Delete, 5 );
}

void Streamer_Shutdown( void )
{

}