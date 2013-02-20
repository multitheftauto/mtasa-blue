/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamerSA.cpp
*  PURPOSE:     GTA:SA entity streamer extensions
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
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

static const DWORD pft = 0x00404180;
static const DWORD pft2 = 0x00404139;

struct streamSectorEntry
{
    CBuildingSAInterface*   entity;
    streamSectorEntry*      next;
};

struct streamRepeatSectorEntry
{
    BYTE                    m_pad[8];   // 0
    streamSectorEntry*      m_entry;
};

static void __cdecl _CBuilding__RemoveReferences( CBuildingSAInterface *building )
{
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

#ifdef _TRY_OTHER_FIX
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
#endif
}

static void __declspec(naked) _CBuilding__Delete()
{
    // We have to remove the building from the internal streamer
    __asm
    {
        push esi
        push edi
        mov edi,ecx
        call pft
        push edi
        call _CBuilding__RemoveReferences
        pop edi
        jmp pft2
    }
}

void Streamer_Init()
{
    // Patch render limitations
    *(unsigned int*)0x00553923 = *(unsigned int*)0x005534F5 = *(unsigned int*)0x00553CB3 = (unsigned int)renderObjects;
    *(unsigned int*)0x00553529 = *(unsigned int*)0x00553944 = *(unsigned int*)0x00553A53 = *(unsigned int*)0x00553B03 = (unsigned int)renderObjects2;
    *(unsigned int*)0x00553965 = (unsigned int)renderObjects3;
    *(unsigned int*)0x00553986 = *(unsigned int*)0x00554AC7 = (unsigned int)renderObjects4;

    // Hook special fixes
    HookInstall( 0x00404130, (DWORD)_CBuilding__Delete, 5 );
}

void Streamer_Shutdown()
{

}