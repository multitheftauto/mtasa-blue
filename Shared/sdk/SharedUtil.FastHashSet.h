/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.FastHashSet.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <tuple>

#if WITH_ALLOC_TRACKING
    #define CFastHashSet std::CSet
#else

#ifdef WIN32
    #pragma push_macro("assert")
#endif

#include <google/dense_hash_set>

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
    template <class K>
    class CFastHashSet : public google::dense_hash_set<K>
    {
    public:
        CFastHashSet()
        {
            this->set_empty_key(GetEmptyMapKey((K*)NULL));
            this->set_deleted_key(GetDeletedMapKey((K*)NULL));
        }
    };

    ////////////////////////////////////////////////
    //
    // CFastHashSet helpers
    //

    // Return true if the insertion took place
    template <class T, class T2>
    bool MapInsert(CFastHashSet<T>& collection, const T2& item)
    {
        return std::get<bool>(collection.insert(item));
    }

    // Return true if the remove took place
    template <class T, class T2>
    bool MapRemove(CFastHashSet<T>& collection, const T2& key)
    {
        typename CFastHashSet<T>::iterator it = collection.find(key);
        if (it == collection.end())
            return false;
        collection.erase(it);
        return true;
    }

}            // namespace SharedUtil

#endif  // WITH_ALLOC_TRACKING
