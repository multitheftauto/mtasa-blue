/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Map.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <map>
#include <set>
#include <vector>
#include "SharedUtil.IntTypes.h"

namespace SharedUtil
{
    ////////////////////////////////////////////////
    //
    // Map helpers for several types
    //

    // Update or add a value for a key
    template <class T, class T2, class V2>
    void MapSet(T& collection, const T2& key, const V2& value)
    {
        collection[key] = value;
    }

    // Returns true if the item is in the collection
    template <class T, class T2>
    bool MapContains(const T& collection, const T2& key)
    {
        return collection.find(key) != collection.end();
    }

    // Remove key from collection
    template <class T, class T2>
    bool MapRemove(T& collection, const T2& key)
    {
        typename T::iterator it = collection.find(key);
        if (it == collection.end())
            return false;
        collection.erase(it);
        return true;
    }

    ////////////////////////////////////////////////
    //
    // std::map helpers
    //

    // Find value in collection
    template <class T, class V, class TR, class T2>
    V* MapFind(std::map<T, V, TR>& collection, const T2& key)
    {
        typename std::map<T, V, TR>::iterator it = collection.find(key);
        if (it == collection.end())
            return NULL;
        return &it->second;
    }

    // Find value in const collection
    template <class T, class V, class TR, class T2>
    const V* MapFind(const std::map<T, V, TR>& collection, const T2& key)
    {
        typename std::map<T, V, TR>::const_iterator it = collection.find(key);
        if (it == collection.end())
            return NULL;
        return &it->second;
    }

    // Find value reference in collection
    template <class T, class V, class TR, class T2>
    V MapFindRef(std::map<T, V, TR>& collection, const T2& key)
    {
        typename std::map<T, V, TR>::iterator it = collection.find(key);
        if (it == collection.end())
            return NULL;
        return it->second;
    }

    // Find value reference in const collection
    template <class T, class V, class TR, class T2>
    const V MapFindRef(const std::map<T, V, TR>& collection, const T2& key)
    {
        typename std::map<T, V, TR>::const_iterator it = collection.find(key);
        if (it == collection.end())
            return NULL;
        return it->second;
    }

    // Find or add a value in collection
    template <class T, class V, class TR, class T2>
    V& MapGet(std::map<T, V, TR>& collection, const T2& key)
    {
        typename std::map<T, V, TR>::iterator it = collection.find(key);
        if (it == collection.end())
        {
            collection[key] = V();
            it = collection.find(key);
        }
        return it->second;
    }

    // Remove value from collection. Returns number of pairs removed
    template <class T, class V2>
    uint MapRemoveByValue(T& collection, const V2& value)
    {
        uint                 uiCount = 0;
        typename T::iterator it = collection.begin();
        for (; it != collection.end();)
        {
            if (it->second == value)
            {
                uiCount++;
                collection.erase(it++);
            }
            else
                ++it;
        }
        return uiCount;
    }

    // Find key in const collection
    template <class T, class V, class TR, class V2>
    const T* MapFindByValue(const std::map<T, V, TR>& collection, const V2& value)
    {
        typename std::map<T, V, TR>::const_iterator it = collection.begin();
        for (; it != collection.end(); ++it)
        {
            if (it->second == value)
            {
                return &it->first;
            }
        }
        return nullptr;
    }

    // Returns true if the value is in the collection
    template <class T, class V, class TR, class V2>
    bool MapContainsValue(const std::map<T, V, TR>& collection, const V2& value)
    {
        return MapFindByValue(collection, value) != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // std::multimap helpers
    //

    // Find values in const collection
    template <class T, class V, class TR, class T2>
    void MultiFind(const std::multimap<T, V, TR>& collection, const T2& key, std::vector<V>* pResult)
    {
        typedef typename std::multimap<T, V, TR>::const_iterator const_iter_t;
        std::pair<const_iter_t, const_iter_t>                    itp = collection.equal_range(key);
        for (const_iter_t it = itp.first; it != itp.second; ++it)
            pResult->push_back(it->second);
    }

    // Find first value in const collection
    template <class T, class V, class TR, class T2>
    const V* MapFind(const std::multimap<T, V, TR>& collection, const T2& key)
    {
        typename std::multimap<T, V, TR>::const_iterator it = collection.find(key);
        if (it == collection.end())
            return NULL;
        return &it->second;
    }

    // Add a value for a key
    template <class T, class V, class TR, class T2, class V2>
    void MapInsert(std::multimap<T, V, TR>& collection, const T2& key, const V2& value)
    {
        collection.insert(std::pair<T2, V>(key, value));
    }

    // Remove first pair
    template <class T, class V, class TR, class T2, class V2>
    void MapRemovePair(std::multimap<T, V, TR>& collection, const T2& key, const V2& value)
    {
        typedef typename std::multimap<T, V, TR>::iterator iter_t;
        std::pair<iter_t, iter_t>                          itp = collection.equal_range(key);
        for (iter_t it = itp.first; it != itp.second; ++it)
            if (it->second == value)
            {
                collection.erase(it);
                break;
            }
    }

    // Returns true if the pair is in the collection
    template <class T, class V, class TR, class T2, class V2>
    bool MapContainsPair(std::multimap<T, V, TR>& collection, const T2& key, const V2& value)
    {
        typedef typename std::multimap<T, V, TR>::iterator iter_t;
        std::pair<iter_t, iter_t>                          itp = collection.equal_range(key);
        for (iter_t it = itp.first; it != itp.second; ++it)
            if (it->second == value)
                return true;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // std::set helpers
    //

    // Return true if the insertion took place
    template <class T, class TR, class T2>
    bool MapInsert(std::set<T, TR>& collection, const T2& item)
    {
        return std::get<bool>(collection.insert(item));
    }

}            // namespace SharedUtil
