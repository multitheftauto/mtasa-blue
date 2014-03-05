/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.cache.h
*  PURPOSE:     Resource cache for fast access
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_RESOURCE_CACHE_
#define _STREAMING_RESOURCE_CACHE_

namespace StreamingCache
{
    bool    GetCachedIMGData( unsigned int imgId, unsigned int blockOffset, unsigned int blockCount, void*& dataPtr );
    bool    CacheIMGFile( const char *name );
    bool    IsIMGFileCached( const char *name );
    void    FreeIMGFileCache( const char *name );
};

// Module initialization.
void StreamingCache_Init( void );
void StreamingCache_Shutdown( void );

#endif //_STREAMING_RESOURCE_CACHE_