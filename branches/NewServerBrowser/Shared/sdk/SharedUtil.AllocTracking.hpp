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
    char tag[28];
};


static int stats_ready = 1;
#ifdef ALLOC_STATS_MODULE_NAME
    #ifndef ALLOC_STATS_PRE_COUNT
        #define ALLOC_STATS_PRE_COUNT 200     // Increase if crash at startup
    #endif
    static int pre_count = ALLOC_STATS_PRE_COUNT;
#else
    #define ALLOC_STATS_MODULE_NAME "none"
    static int pre_count = -1;
#endif

static int no_stuff = 0;

typedef unsigned char BYTE;

class CAllocStats
{
public:
    maptype < BYTE*, CAllocInfo >   allocMap;
    int                             allocIndex;
    CCriticalSection                cs;

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
        ActiveAllocs = 0;
        DupeAllocs = 0;
        UniqueAllocs = 0;
        ReAllocs = 0;
        Frees = 0;
        UnmatchedFrees = 0;
        DupeAllocs = 0;
        DupeMem = 0;
    }

    void AddAllocInfo( BYTE* pData, size_t Count, const char* Tag, int ElementSize )
    {
        cs.Lock();
        no_stuff++;
        if( !xMapContains( allocMap, pData ) )
        {
            CAllocInfo info;
            info.elementSize = ElementSize;
            info.size = Count;
            info.countRealloc = 0;
            info.allocIndex = allocIndex++;
            STRNCPY( info.tag, Tag ? Tag : "", 28 );

            xMapSet( allocMap, pData, info );

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
        no_stuff--;
        cs.Unlock();
    }

    void MoveAllocInfo( BYTE* pOrig, BYTE* pNew, size_t Count, const char* Tag, int ElementSize )
    {
        cs.Lock();
        no_stuff++;
        CAllocInfo info;
        int SizeDif = Count;
        if( xMapContains( allocMap, pOrig ) )
        {
            info = *xMapFind ( allocMap, pOrig );
            xMapRemove( allocMap, pOrig );
            SizeDif = Count - info.size;
        }

        info.elementSize = ElementSize;
        info.size = Count;
        info.countRealloc++;
        info.allocIndex = allocIndex++;
        ReAllocs++;

        xMapSet( allocMap, pNew, info );

        TotalMem += SizeDif;
        TotalMemMax = Max ( TotalMemMax, TotalMem );
        no_stuff--;
        cs.Unlock();
    }

    void RemoveAllocInfo( BYTE* pOrig )
    {
        cs.Lock();
        no_stuff++;
        if( xMapContains( allocMap, pOrig ) )
        {
            CAllocInfo* pInfo = xMapFind ( allocMap, pOrig );
            TotalMem -= pInfo->size;
            xMapRemove( allocMap, pOrig );
            ActiveAllocs--;
            Frees++;
        }
        else
        {
            UnmatchedFrees++;
        }
        no_stuff--;
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

    if ( no_stuff || pre_count )
        return malloc ( Count );

    BYTE* pData = (BYTE*)malloc ( Count );

    if ( stats_ready )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        pAllocStats->AddAllocInfo( pData, Count, Tag, ElementSize );
    }

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
    if ( no_stuff || pre_count )
        return realloc ( Original, Count );

    BYTE* pData = (BYTE*)realloc ( Original, Count );

    if ( stats_ready )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        pAllocStats->MoveAllocInfo( (BYTE*)Original, pData, Count, Tag, ElementSize );
    }

    return pData;
}

void myFree ( void* Original )
{
    if ( no_stuff || pre_count )
        return free ( Original );

    if ( !Original )
        return;

    if ( stats_ready )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        pAllocStats->RemoveAllocInfo( (BYTE*)Original );
    }

    free ( Original );
}

void* myNew ( std::size_t size, const char* Tag, int ElementSize )
{
    return myMalloc ( size, Tag, ElementSize );
}

void myDelete (void* ptr)
{
    myFree ( ptr );
}

#define map CMap
#ifdef WIN32
    #define malloc _malloc_
    #define realloc _realloc_
    #define calloc _calloc_
    #define free _free_
#endif

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
MTAEXPORT unsigned long GetAllocStats( unsigned long* pOutStats, unsigned long ulNumStats )
{
    CAllocStats* pAllocStats = GetAllocStats();

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

#endif  // WITH_ALLOC_TRACKING
