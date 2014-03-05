/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.cache.cpp
*  PURPOSE:     Resource cache for fast access
*               This is a MTA extension (derived from CMultiplayerSA)
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// This functionality can be used to load .IMG files into memory.
// It is more advanced than the fast clothes loading, as it supports all .IMG containers.

/*=========================================================
    StreamingCache::GetCachedIMGData

    Arguments:
        imgId - .IMG container id which the request belongs to
        blockOffset - offset in blocks (2048) of the request
        blockCount - size in blocks (2048) of the request
        dataPtr - output pointer to write the load source from if successful
    Purpose:
        Looks up the cache registry and returns a data pointer
        if a cache representation of a requested .IMG data has
        been found.
=========================================================*/
struct IMGFileCache
{
    IMGFileCache( void )
    {
        isCached = false;
    }

    bool isCached;
    std::vector <char> buffer;
};

static IMGFileCache cachedIMGFiles[MAX_GTA_IMG_ARCHIVES];

bool StreamingCache::GetCachedIMGData( unsigned int imgId, unsigned int blockOffset, unsigned int blockCount, void*& dataPtr )
{
    IMGFileCache& cacheEntry = cachedIMGFiles[imgId];

    if ( cacheEntry.isCached )
    {
        dataPtr = ( &cacheEntry.buffer[0] + blockOffset * 2048 );
        return true;
    }

    return false;
}

/*=========================================================
    StreamingCache::CacheIMGFile

    Arguments:
        name - name of the .IMG container to cache
    Purpose:
        Attempts to cache an .IMG container by name.
        Returns whether the operation was successful.
=========================================================*/
bool StreamingCache::CacheIMGFile( const char *name )
{
    unsigned int imgID;

    if ( !GetIMGFileByName( name, imgID ) )
        return false;

    // Reset the cache data.
    IMGFileCache& cacheEntry = cachedIMGFiles[imgID]; 
    cacheEntry.buffer.clear();

    bool success = gameFileRoot->ReadToBuffer( name, cachedIMGFiles[imgID].buffer );

    cacheEntry.isCached = success;
    return success;
}

void CStreamingSA::CacheIMGFile( const char *name )     { StreamingCache::CacheIMGFile( name ); }

/*=========================================================
    StreamingCache::IsIMGFileCached

    Arguments:
        name - name of the .IMG container to check
    Purpose:
        Returns whether the given .IMG container is cached.
=========================================================*/
bool StreamingCache::IsIMGFileCached( const char *name )
{
    unsigned int imgID;

    if ( !GetIMGFileByName( name, imgID ) )
        return false;

    return cachedIMGFiles[imgID].isCached;
}

bool CStreamingSA::IsIMGFileCached( const char *name ) const    { return StreamingCache::IsIMGFileCached( name ); }

/*=========================================================
    StreamingCache::FreeIMGFileCache

    Arguments:
        name - name of the .IMG container
    Purpose:
        Frees the cache resources associated with the
        named container (if available).
=========================================================*/
void StreamingCache::FreeIMGFileCache( const char *name )
{
    unsigned int imgID;

    if ( !GetIMGFileByName( name, imgID ) )
        return;

    cachedIMGFiles[imgID].buffer.clear();
}

void CStreamingSA::FreeIMGFileCache( const char *name ) { return StreamingCache::FreeIMGFileCache( name ); }

void StreamingCache_Init( void )
{
}

void StreamingCache_Shutdown( void )
{
}