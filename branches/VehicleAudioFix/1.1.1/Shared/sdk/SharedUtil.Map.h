/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Map.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#if defined(WIN32)
    #include <hash_map>
    #define HASH_MAP_TYPE stdext::hash_map
#elif defined(__GNUC__) && (__GNUC__ >= 3)
    #include <ext/hash_map>
    #define HASH_MAP_TYPE __gnu_cxx::hash_map
#endif

namespace SharedUtil
{

    ////////////////////////////////////////////////
    //
    // Using hash_map
    //
    template < class K, class V >
    class CHashMap : public HASH_MAP_TYPE < K, V >
    {
    public:
    };


    ////////////////////////////////////////////////
    //
    // Map helpers for any type
    // 

    // Update or add a value for a key
    template < class T, class T2, class V2 >
    void MapSet ( T& collection, const T2& key, const V2& value )
    {
        collection[ key ] = value;
    }

    // Returns true if the item is in the collection
    template < class T, class T2 >
    bool MapContains ( const T& collection, const T2& key )
    {
        return collection.find ( key ) != collection.end ();
    }


    ////////////////////////////////////////////////
    //
    // std::map helpers
    //

    // Remove key from collection
    template < class T, class V, class TR, class T2 >
    bool MapRemove ( std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return false;
        collection.erase ( it );
        return true;
    }

    // Find value in collection
    template < class T, class V, class TR, class T2 >
    V* MapFind ( std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value in const collection
    template < class T, class V, class TR, class T2 >
    const V* MapFind ( const std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value reference in collection
    template < class T, class V, class TR, class T2 >
    V MapFindRef ( std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return it->second;
    }

    // Find value reference in const collection
    template < class T, class V, class TR, class T2 >
    const V MapFindRef ( const std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return it->second;
    }

    // Find or add a value in collection
    template < class T, class V, class TR, class T2 >
    V& MapGet ( std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
        {
            collection[key] = V ();
            it = collection.find ( key );
        }
        return it->second;
    }


    // Remove value from collection. Returns number of pairs removed
    template < class T, class V, class TR, class V2 >
    uint MapRemoveByValue ( std::map < T, V, TR >& collection, const V2& value )
    {
        uint uiCount = 0;
        typename std::map < T, V, TR > ::iterator it = collection.begin ();
        for ( ; it != collection.end () ; )
        {
            if ( it->second == value )
            {
                uiCount++;
                collection.erase ( it++ );
            }
            else
                ++it;
        }
        return uiCount;
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    // std::multimap helpers
    //

    // Find values in const collection
    template < class T, class V, class TR, class T2 >
    void MultiFind ( const std::multimap < T, V, TR >& collection, const T2& key, std::vector < V >* pResult )
    {
        typedef typename std::multimap < T, V, TR > ::const_iterator const_iter_t;
        std::pair < const_iter_t, const_iter_t > itp = collection.equal_range ( key );
        for ( const_iter_t it = itp.first ; it != itp.second ; ++it )
            pResult->push_back ( it->second );
    }

    // Find first value in const collection
    template < class T, class V, class TR, class T2 >
    const V* MapFind ( const std::multimap < T, V, TR >& collection, const T2& key )
    {
        typename std::multimap < T, V, TR > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Add a value for a key
    template < class T, class V, class TR, class T2, class V2 >
    void MapInsert ( std::multimap < T, V, TR >& collection, const T2& key, const V2& value )
    {
        collection.insert ( std::pair < T2, V > ( key, value ) );
    }

    // Remove first pair
    template < class T, class V, class TR, class T2, class V2 >
    void MapRemovePair ( std::multimap < T, V, TR >& collection, const T2& key, const V2& value )
    {
        typedef typename std::multimap < T, V, TR > ::iterator iter_t;
        std::pair < iter_t, iter_t > itp = collection.equal_range ( key );
        for ( iter_t it = itp.first ; it != itp.second ; ++it )
            if ( it->second == value )
            { 
                collection.erase ( it );
                break;
            }
    }

    // Returns true if the pair is in the collection
    template < class T, class V, class TR, class T2, class V2 >
    bool MapContainsPair ( std::multimap < T, V, TR >& collection, const T2& key, const V2& value )
    {
        typedef typename std::multimap < T, V, TR > ::iterator iter_t;
        std::pair < iter_t, iter_t > itp = collection.equal_range ( key );
        for ( iter_t it = itp.first ; it != itp.second ; ++it )
            if ( it->second == value )
                return true;
        return false;
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    // std::set helpers
    //

    // Update or add an item
    template < class T, class TR, class T2 >
    void MapInsert ( std::set < T, TR >& collection, const T2& item )
    {
        collection.insert ( item );
    }

    // Returns true if the item is in the collection
    template < class T, class TR, class T2 >
    bool MapContains ( const std::set < T, TR >& collection, const T2& item )
    {
        return collection.find ( item ) != collection.end ();
    }

    // Remove item from collection
    template < class T, class TR, class T2 >
    bool MapRemove ( std::set < T, TR >& collection, const T2& item )
    {
        typename std::set < T, TR > ::iterator it = collection.find ( item );
        if ( it == collection.end () )
            return false;
        collection.erase ( it );
        return true;
    }


    ////////////////////////////////////////////////
    //
    // CHashMap helpers
    // 

    // Remove key from collection
    template < class T, class V, class T2 >
    bool MapRemove ( CHashMap < T, V >& collection, const T2& key )
    {
        typename CHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return false;
        collection.erase ( it );
        return true;
    }

    // Find value in collection
    template < class T, class V, class T2 >
    V* MapFind ( CHashMap < T, V >& collection, const T2& key )
    {
        typename CHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value in const collection
    template < class T, class V, class T2 >
    const V* MapFind ( const CHashMap < T, V >& collection, const T2& key )
    {
        typename CHashMap < T, V > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value reference in collection
    template < class T, class V, class T2 >
    V MapFindRef ( CHashMap < T, V >& collection, const T2& key )
    {
        typename CHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return it->second;
    }

    // Find value reference in const collection
    template < class T, class V, class T2 >
    const V MapFindRef ( const CHashMap < T, V >& collection, const T2& key )
    {
        typename CHashMap < T, V > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return it->second;
    }

    // Find or add a value in collection
    template < class T, class V, class T2 >
    V& MapGet ( CHashMap < T, V >& collection, const T2& key )
    {
        typename CHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
        {
            collection[key] = V ();
            it = collection.find ( key );
        }
        return it->second;
    }
}


// Calculate a hash value for SString
#if defined(WIN32)

    inline size_t hash_value ( const SString& strString )
    {
        const char *_Ptr = strString.c_str ();
        return ( stdext::_Hash_value ( _Ptr, _Ptr + strString.size () ) );
    }

#elif defined(__GNUC__) && (__GNUC__ >= 3)

    namespace __gnu_cxx
    {
        template<>
        struct hash < SString >
        {
            size_t operator () ( const SString& strString ) const
            {
                return __stl_hash_string ( strString );
            }
        };
    }

#endif
