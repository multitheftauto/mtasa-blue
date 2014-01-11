/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.FastHashSet.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#if WITH_ALLOC_TRACKING
    #define CFastHashSet std::CSet
#else

#ifdef WIN32
    #pragma push_macro("assert")
#endif

#include <google/dense_hash_set>
using namespace google;

#ifdef WIN32
    #pragma pop_macro("assert")
#endif

namespace SharedUtil
{

    ////////////////////////////////////////////////
    //
    // Using google::dense_hash_set
    // Requires special keys for empty and deleted items
    //
    template < class K >
    class CFastHashSet : public google::dense_hash_set < K >
    {
    public:
        CFastHashSet ( void )
        {
            set_empty_key ( GetEmptyMapKey ( (K*)NULL ) );
            set_deleted_key ( GetDeletedMapKey ( (K*)NULL ) );
        }
    };


    ////////////////////////////////////////////////
    //
    // CFastHashSet helpers
    //
    template < class T, class T2 >
    void MapInsert ( CFastHashSet < T >& collection, const T2& item )
    {
        collection.insert ( item );
    }

    // Remove key from collection
    template < class T, class T2 >
    bool MapRemove ( CFastHashSet < T >& collection, const T2& key )
    {
        typename CFastHashSet < T > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return false;
        collection.erase ( it );
        return true;
    }

}

#endif  // WITH_ALLOC_TRACKING
