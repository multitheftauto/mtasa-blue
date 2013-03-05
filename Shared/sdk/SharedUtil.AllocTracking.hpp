/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.AllocTracking.hpp
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


//
//
// Memory usage tracking
//
// Don't look!
//
//
#if WITH_ALLOC_TRACKING

#undef map
#ifdef WIN32
    #undef malloc
    #undef realloc
    #undef calloc
    #undef free
#endif

using namespace std;
namespace
{
    //#define maptype hash_map
    #define maptype map
    // Update or add a value for a key
    template < class T, class V, class TR, class T2 >
    void xMapSet ( maptype < T, V, TR >& collection, const T2& key, const V& value )
    {
        collection[ key ] = value;
    }

    // Returns true if the item is in the collection
    template < class T, class V, class TR, class T2 >
    bool xMapContains ( const maptype < T, V, TR >& collection, const T2& key )
    {
        return collection.find ( key ) != collection.end ();
    }

    // Remove key from collection
    template < class T, class V, class TR, class T2 >
    void xMapRemove ( maptype < T, V, TR >& collection, const T2& key )
    {
        typename maptype < T, V, TR > ::iterator it = collection.find ( key );
        if ( it != collection.end () )
            collection.erase ( it );
    }

    // Find value in collection
    template < class T, class V, class TR, class T2 >
    V* xMapFind ( maptype < T, V, TR >& collection, const T2& key )
    {
        typename maptype < T, V, TR > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value in const collection
    template < class T, class V, class TR, class T2 >
    const V* xMapFind ( const maptype < T, V, TR >& collection, const T2& key )
    {
        typename maptype < T, V, TR > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }
}

struct CAllocInfo
{
    int elementSize;
    int size;
    int countRealloc;
    int allocIndex;
    int tagId;
};


static int stats_ready = 1;
#ifdef ALLOC_STATS_MODULE_NAME
    #ifndef ALLOC_STATS_PRE_COUNT
        #define ALLOC_STATS_PRE_COUNT 0     // Increase if crash at startup
    #endif
    static int pre_count = ALLOC_STATS_PRE_COUNT;
#else
    #define ALLOC_STATS_MODULE_NAME "none"
    static int pre_count = -1;
#endif

static int no_stuff = 0;    // No tracking when tracker is allocating
#define INVALID_THREAD_ID (-2)

typedef unsigned char BYTE;

struct STagString
{
    char data[24];
    bool operator< ( const STagString& other ) const
    {
        return strcmp( data, other.data ) < 0;
    }
};

typedef SAllocTrackingTagInfo STagInfo;

class CAllocStats
{
public:
    maptype < BYTE*, CAllocInfo >   allocMap;
    int                             allocIndex;
    maptype < STagString, uint >    tagIdMap;
    uint                            tagIdCounter;
    maptype < uint, STagInfo >      tagInfoMap;
    CCriticalSection                cs;
    DWORD                           dwThreadUsing;

    #define SIZE_SORTED_TAG_THRESH 10000
    maptype < uint64, uint >      sizeSortedTagMap;

    unsigned long TotalMem;
    unsigned long TotalMemMax;
    unsigned long ActiveAllocs;
    unsigned long DupeAllocs;
    unsigned long UniqueAllocs;
    unsigned long ReAllocs;
    unsigned long Frees;
    unsigned long UnmatchedFrees;
    unsigned long DupeMem;

    CAllocStats()
    {
        TotalMem = 0;
        TotalMemMax = 0;
        allocIndex = 0;
        tagIdCounter = 0;
        ActiveAllocs = 0;
        DupeAllocs = 0;
        UniqueAllocs = 0;
        ReAllocs = 0;
        Frees = 0;
        UnmatchedFrees = 0;
        DupeAllocs = 0;
        DupeMem = 0;
        dwThreadUsing = INVALID_THREAD_ID;
    }

    uint GetTagId( const char* Tag )
    {
        STagString tagString;
        STRNCPY( tagString.data, Tag, NUMELMS( tagString.data ) );
        uint* pId = xMapFind( tagIdMap, tagString );
        if ( pId )
        {
            return *pId;
        }
        else
        {
            uint tagId = ++tagIdCounter;
            xMapSet( tagIdMap, tagString, tagId );
            return tagId;
        }
    }
 
    void AddSizeSortedTag( uint tagId, int size )
    {
        uint64 key = ((uint64)size << 32) | (uint64)tagId;
        xMapSet( sizeSortedTagMap, key, tagId );
    }

    void RemoveSizeSortedTag( uint tagId, int size )
    {
        uint64 key = ((uint64)size << 32) | (uint64)tagId;
        xMapRemove( sizeSortedTagMap, key );
    }


    void AddTagAlloc( uint tagId, const char* Tag, int size )
    {
        STagInfo* pTagInfo = xMapFind( tagInfoMap, tagId );
        if( pTagInfo )
        {
            if ( pTagInfo->size > SIZE_SORTED_TAG_THRESH )
                RemoveSizeSortedTag( tagId, pTagInfo->size );

            pTagInfo->countAllocs++;
            pTagInfo->size += size;

            if ( pTagInfo->size > SIZE_SORTED_TAG_THRESH )
                AddSizeSortedTag( tagId, pTagInfo->size );
        }
        else
        {
            STagInfo tagInfo;
            tagInfo.countAllocs = 1;
            tagInfo.size = size;
            STRNCPY( tagInfo.tag, Tag, NUMELMS( tagInfo.tag ) );
            xMapSet( tagInfoMap, tagId, tagInfo );

            if ( tagInfo.size > SIZE_SORTED_TAG_THRESH )
                AddSizeSortedTag( tagId, tagInfo.size );
        }
    }

    void RemoveTagAlloc( uint tagId, int size )
    {
        STagInfo* pTagInfo = xMapFind( tagInfoMap, tagId );
        if( pTagInfo )
        {
            if( pTagInfo->countAllocs > 0 )
            {
                if ( pTagInfo->size > SIZE_SORTED_TAG_THRESH )
                    RemoveSizeSortedTag( tagId, pTagInfo->size );

                pTagInfo->countAllocs--;
                pTagInfo->size -= size;
                if( pTagInfo->countAllocs < 1 )
                    pTagInfo->size = 0;

                if ( pTagInfo->size > SIZE_SORTED_TAG_THRESH )
                    AddSizeSortedTag( tagId, pTagInfo->size );
            }
        }
    }

    void AddAllocInfo( BYTE* pData, size_t Count, const char* Tag, int ElementSize )
    {
        Tag = Tag ? Tag : "";
        if( !xMapContains( allocMap, pData ) )
        {
            CAllocInfo info;
            info.elementSize = ElementSize;
            info.size = Count;
            info.countRealloc = 0;
            info.allocIndex = allocIndex++;
            info.tagId = GetTagId( Tag );

            xMapSet( allocMap, pData, info );

            AddTagAlloc( info.tagId, Tag, info.size );

            ActiveAllocs++;
            UniqueAllocs++;
            TotalMem += Count;
            TotalMemMax = Max ( TotalMemMax, TotalMem );
        }
        else
        {
            DupeAllocs++;
            DupeMem += Count;
        }
    }

    void MoveAllocInfo( BYTE* pOrig, BYTE* pNew, size_t Count, const char* Tag, int ElementSize )
    {
        Tag = Tag ? Tag : "";
        CAllocInfo info;
        int SizeDif = Count;
        if( xMapContains( allocMap, pOrig ) )
        {
            info = *xMapFind ( allocMap, pOrig );
            xMapRemove( allocMap, pOrig );
            RemoveTagAlloc( info.tagId, info.size );
            SizeDif = Count - info.size;
        }
        else
            info.countRealloc = 0;

        info.elementSize = ElementSize;
        info.size = Count;
        info.countRealloc++;
        info.allocIndex = allocIndex++;
        info.tagId = GetTagId( Tag );
        ReAllocs++;

        xMapSet( allocMap, pNew, info );
        AddTagAlloc( info.tagId, Tag, info.size );

        TotalMem += SizeDif;
        TotalMemMax = Max ( TotalMemMax, TotalMem );
    }

    void RemoveAllocInfo( BYTE* pOrig )
    {
        if( xMapContains( allocMap, pOrig ) )
        {
            CAllocInfo* pInfo = xMapFind ( allocMap, pOrig );
            TotalMem -= pInfo->size;
            RemoveTagAlloc( pInfo->tagId, pInfo->size );

            xMapRemove( allocMap, pOrig );
            ActiveAllocs--;
            Frees++;
        }
        else
        {
            UnmatchedFrees++;
        }
    }

    // Return false if should not use alloc tracking
    bool Lock( void )
    {
        DWORD dwThreadWanting = GetCurrentThreadId();
        if ( dwThreadWanting == dwThreadUsing )
            return false;   // No tracking when tracker is allocating 
        cs.Lock();
        dwThreadUsing = dwThreadWanting;
        return true;
    }

    void Unlock( void )
    {
        dwThreadUsing = INVALID_THREAD_ID;
        cs.Unlock();
    }
};

static CAllocStats* pAllocStats = NULL;

static CAllocStats* GetAllocStats()
{
    if ( !pAllocStats )
    {
        no_stuff++;
        pAllocStats = new CAllocStats();
        no_stuff--;
    }
    return pAllocStats;
}


typedef unsigned char BYTE;

void* myMalloc ( size_t Count, const char* Tag, int ElementSize )
{
    if ( pre_count > 0 )
        pre_count--;

    if ( pre_count || no_stuff )
        return malloc ( Count );

    CAllocStats* pAllocStats = GetAllocStats();
    if ( !pAllocStats->Lock() )
        return malloc ( Count );

    BYTE* pData = (BYTE*)malloc ( Count );
    pAllocStats->AddAllocInfo( pData, Count, Tag, ElementSize );
    pAllocStats->Unlock();
    return pData;
}

void* myCalloc ( size_t Count, size_t elsize, const char* Tag, int ElementSize )
{
    void* pData = myMalloc ( Count * elsize, Tag, ElementSize );
    memset ( pData, 0, Count * elsize );
    return pData;
}

void* myRealloc ( void* Original, size_t Count, const char* Tag, int ElementSize )
{
    if ( pre_count || no_stuff )
        return realloc ( Original, Count );

    CAllocStats* pAllocStats = GetAllocStats();
    if ( !pAllocStats->Lock() )
        return realloc ( Original, Count );

    BYTE* pData = (BYTE*)realloc ( Original, Count );
    pAllocStats->MoveAllocInfo( (BYTE*)Original, pData, Count, Tag, ElementSize );
    pAllocStats->Unlock();
    return pData;
}

void myFree ( void* Original )
{
    if ( pre_count || no_stuff )
        return free ( Original );

    if ( !Original )
        return;

    CAllocStats* pAllocStats = GetAllocStats();
    if ( !pAllocStats->Lock() )
        return free ( Original );

    pAllocStats->RemoveAllocInfo( (BYTE*)Original );
    free ( Original );
    pAllocStats->Unlock();
}

void* myNew ( std::size_t size, const char* Tag, int ElementSize )
{
    return myMalloc ( size, Tag, ElementSize );
}

void myDelete (void* ptr)
{
    myFree ( ptr );
}

// Set up export type definition for Win32
#ifdef WIN32
    #define MTAEXPORT extern "C" __declspec(dllexport)
#else
    #define MTAEXPORT extern "C"
#endif

//
// Get memory stats from this dll
//
// Returns number of stats copied
//
MTAEXPORT unsigned long GetAllocStats( uint uiType, void* pOutData, unsigned long ulNumStats )
{
    if ( uiType == 0 )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        unsigned long* pOutStats = (unsigned long*)pOutData;

        if ( ulNumStats > 0 )   pOutStats[0] = pAllocStats->TotalMem;
        if ( ulNumStats > 1 )   pOutStats[1] = pAllocStats->TotalMemMax;
        if ( ulNumStats > 2 )   pOutStats[2] = pAllocStats->ActiveAllocs;
        if ( ulNumStats > 3 )   pOutStats[3] = pAllocStats->DupeAllocs;
        if ( ulNumStats > 4 )   pOutStats[4] = pAllocStats->UniqueAllocs;
        if ( ulNumStats > 5 )   pOutStats[5] = pAllocStats->ReAllocs;
        if ( ulNumStats > 6 )   pOutStats[6] = pAllocStats->Frees;
        if ( ulNumStats > 7 )   pOutStats[7] = pAllocStats->UnmatchedFrees;
        if ( ulNumStats > 8 )   pOutStats[8] = pAllocStats->DupeMem;

        return Min < unsigned long > ( ulNumStats, 9 );
    }
    else
    if ( uiType == 1 )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        pAllocStats->cs.Lock();
        STagInfo* pOutStats = (STagInfo*)pOutData;

        uint idx = 0;
        for ( maptype < uint64, uint >::reverse_iterator it = pAllocStats->sizeSortedTagMap.rbegin() ; it != pAllocStats->sizeSortedTagMap.rend() ; ++it )
        {
            if ( idx >= ulNumStats )
                break;

            uint tagId = it->second;
            STagInfo* pTagInfo = xMapFind( pAllocStats->tagInfoMap, tagId );
            if( pTagInfo )
            {
                pOutStats[idx++] = *pTagInfo;
            }
        }

        pAllocStats->cs.Unlock();
        return idx;
    }
    return 0;
}


#define map CMap
#ifdef WIN32
    #define malloc _malloc_
    #define realloc _realloc_
    #define calloc _calloc_
    #define free _free_
#endif

#endif  // WITH_ALLOC_TRACKING
