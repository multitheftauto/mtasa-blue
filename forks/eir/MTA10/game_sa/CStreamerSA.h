/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamerSA.h
*  PURPOSE:     GTA:SA entity streamer extensions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMER_EXTENSIONS_
#define _STREAMER_EXTENSIONS_

// Streamer variables.
// The streaming sectors are a*a quads, hence rows == cols always true.
#define ARRAY_StreamStaticSectors                           0x00B7D0B8
#define NUM_StreamStaticSectorRows                          120
#define NUM_StreamStaticSectorCols                          120
// The dynamic sector repeats on top of the static sector room.
// Probably an optimization by Rockstar Games.
#define ARRAY_StreamDynamicSectors                          0x00B992B8
#define NUM_StreamDynamicSectorRows                         16
#define NUM_StreamDynamicSectorCols                         16
#define ARRAY_StreamUpdateSectors                           0x00B99EB8
#define NUM_StreamUpdateSectorRows                          30
#define NUM_StreamUpdateSectorCols                          30

// Global game API for streaming entities
namespace Streamer
{
    typedef CPtrNodeSingleSA <CEntitySAInterface> entityLink_t;

    // Structures used to save streamed-in entities in.
    typedef ptrNodeSingleContainer <CEntitySAInterface> streamSectorEntry;

    template <typename callbackType>
    inline void ExecuteChain( streamSectorEntry& sector, callbackType& cb )
    {
        if ( Streamer::entityLink_t *ptrNode = sector.GetList() )
        {
            cb.EntryCallback( sector.m_list );

            while ( ptrNode && ptrNode->m_next )
            {
                cb.EntryCallback( ptrNode->m_next );

                ptrNode = ptrNode->m_next;
            }
        }
    }

    struct streamStaticSectorEntry
    {
        streamSectorEntry       building;   // 0
        streamSectorEntry       dummy;      // 4
    };

    struct streamDynamicSectorEntry
    {
        streamSectorEntry       vehicle;    // 0
        streamSectorEntry       ped;        // 4
        streamSectorEntry       object;     // 8
    };

    // Helper function to scan all entities that are streamed in.
    template <typename callbackType>
    void __forceinline ForAllStreamerSectors( callbackType& cb, bool doBuildings, bool doDummies, bool doVehicles, bool doPeds, bool doObjects )
    {
        if ( doBuildings || doDummies )
        {
            // Scan all static (non moving) entities.
            for ( unsigned int n = 0; n < NUM_StreamStaticSectorRows * NUM_StreamStaticSectorCols; n++ )
            {
                streamStaticSectorEntry *entry = ((streamStaticSectorEntry*)ARRAY_StreamStaticSectors + n);

                // Scan building.
                if ( doBuildings )
                    cb.OnSector( entry->building );

                // Scan dummy.
                if ( doDummies )
                    cb.OnSector( entry->dummy );
            }
        }

        if ( doVehicles || doPeds || doObjects )
        {
            // Now for the dynamic entities entities.
            // ccw has asked whether they are big buildings/he indicated toward it.
            // These are game entitites that can move around, hence they have less sectors to switch around on.
            for ( unsigned int n = 0; n < NUM_StreamDynamicSectorRows * NUM_StreamDynamicSectorCols; n++ )
            {
                streamDynamicSectorEntry *entry = (streamDynamicSectorEntry*)ARRAY_StreamDynamicSectors + n;

                // We have three streaming entries here.
                // Scan vehicle.
                if ( doVehicles )
                    cb.OnSector( entry->vehicle );

                // Scan ped.
                if ( doPeds )
                    cb.OnSector( entry->ped );

                // Scan objects.
                if ( doObjects )
                    cb.OnSector( entry->object );
            }
        }
    }

    // API for utilities.
    void __cdecl ResetScanIndices( void );
    void __cdecl RequestStaticEntitiesOfSector( const streamSectorEntry& entryPtr, unsigned int reqFlags );
    void __cdecl RequestStaticEntities(
        const streamSectorEntry& entryPtr, float x, float y,
        float minX, float minY, float maxX, float maxY,
        float camFarClip, unsigned int reqFlags
    );
    void __cdecl RequestSquaredSectorEntities( const CVector& reqPos, unsigned int reqFlags );
    void __cdecl RequestAdvancedSquaredSectorEntities( const CVector& reqPos, unsigned int reqFlags );
};

// Module management routines.
void    Streamer_Init( void );
void    Streamer_Shutdown( void );

#endif //_STREAMER_EXTENSIONS_