/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Maps.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <google/dense_hash_map>
#include <hash_map>
using namespace google;

namespace SharedUtil
{
    ////////////////////////////////////////////////
    //
    // Using stdext::hash_map
    //
    template < class K, class V >
    class CHashMap : public stdext::hash_map < K, V >
    {
    public:
    };

    template < class V >
    class CStringHashMap : public CHashMap < SString, V >
    {
    public:
    };


    ////////////////////////////////////////////////
    //
    // Using google::dense_hash_map
    // Requires special keys for empty and deleted items
    //
    template < class K, class V >
    class CFastHashMap : public google::dense_hash_map < K, V >
    {
    public:
        CFastHashMap ( const K& emptyKey, const K& deletedKey )
        {
            SetSpecialKeys ( emptyKey, deletedKey );
        }

        void SetSpecialKeys ( const K& emptyKey, const K& deletedKey )
        {
            set_empty_key ( emptyKey );
            set_deleted_key ( deletedKey );
        }
    };

    template < class V >
    class CFastStringHashMap : public CFastHashMap < SString, V >
    {
    public:
        CFastStringHashMap ( void )
            : CFastHashMap ( std::string ( "\xFF\xFE" ), std::string ( "\xFF\xFF" ) )
        {
        }
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
    // Map helpers for CHashMap
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


    ////////////////////////////////////////////////
    //
    // Map helpers for CFastHashMap
    //

    // Remove key from collection
    template < class T, class V, class T2 >
    bool MapRemove ( CFastHashMap < T, V >& collection, const T2& key )
    {
        typename CFastHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return false;
        collection.erase ( it );
        return true;
    }

    // Find value in collection
    template < class T, class V, class T2 >
    V* MapFind ( CFastHashMap < T, V >& collection, const T2& key )
    {
        typename CFastHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value in const collection
    template < class T, class V, class T2 >
    const V* MapFind ( const CFastHashMap < T, V >& collection, const T2& key )
    {
        typename CFastHashMap < T, V > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value reference in collection
    template < class T, class V, class T2 >
    V MapFindRef ( CFastHashMap < T, V >& collection, const T2& key )
    {
        typename CFastHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return it->second;
    }

    // Find value reference in const collection
    template < class T, class V, class T2 >
    const V MapFindRef ( const CFastHashMap < T, V >& collection, const T2& key )
    {
        typename CFastHashMap < T, V > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return it->second;
    }

    // Find or add a value in collection
    template < class T, class V, class T2 >
    V& MapGet ( CFastHashMap < T, V >& collection, const T2& key )
    {
        typename CFastHashMap < T, V > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
        {
            collection[key] = V ();
            it = collection.find ( key );
        }
        return it->second;
    }
}

// Calculate a hash value for SString
inline size_t hash_value ( const SString& strString )
{
    const char* pBegin = *strString;
    const char* pEnd = pBegin + strString.length ();
    size_t value = 2166136261U;
    while ( pBegin != pEnd )
	    value = 16777619U * value ^ (size_t)*pBegin++;
    return value;
}
