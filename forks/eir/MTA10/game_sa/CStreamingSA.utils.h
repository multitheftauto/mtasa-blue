/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.utils.h
*  PURPOSE:     Data streamer utilities
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_UTILITIES_
#define _STREAMING_UTILITIES_

#define ARRAY_PEDSPECMODEL      0x008E4C00
#define VAR_PEDSPECMODEL        0x008E4BB0

#define     VAR_currArea                    0xB72914 // ##SA##

namespace Streaming
{
    // Check whether the entitiy is in a valid streaming area.
    inline bool IsValidStreamingArea( unsigned char area )
    {
        return ( *(unsigned int*)VAR_currArea == area ) || ( area == 13 );
    }

    bool __cdecl IsCurrentHourPeriod( unsigned char fromHour, unsigned char toHour );

    // The best way to maintain R* source code is to split recurring logic into functions like these.
    // Definitions of certain parts are easily changed globally this way.
    inline bool IsTimeRightForStreaming( ModelInfo::timeInfo *timeInfo )
    {
        // Check the GTA clock.
        return Streaming::IsCurrentHourPeriod( timeInfo->m_fromHour, timeInfo->m_toHour );
    }

    void __cdecl StreamPedsAndVehicles( const CVector& reqPos );
    bool __cdecl IsStreamingBusy( void );

    extern streamingLoadCallback_t  streamingLoadCallback;
};

// Global arrays for collision replacement.
extern class CColModelSA *g_colReplacement[MAX_MODELS];
extern class CColModelSAInterface *g_originalCollision[MAX_MODELS];

// This is an optimization because the computer binary system works that way.
// Removes the need for additional clock cycles by avoiding conversion to integer.
#define idOffset( num, off )                ((modelId_t)( (modelId_t)(num) - (modelId_t)(off) ))
#define idRangeCheck( num, off, range )     (idOffset((num),(off)) < (modelId_t)(range))
#define idRangeCheckEx( num, range )        ((modelId_t)(num) < (modelId_t)(range))

// Streaming utility exports.

void StreamingUtils_Init( void );
void StreamingUtils_Shutdown( void );

#endif //_STREAMING_UTILITIES_