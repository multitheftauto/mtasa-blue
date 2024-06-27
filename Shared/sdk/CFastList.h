/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CFastList.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <map>
#include <vector>
#include <list>
#include "SharedUtil.IntTypes.h"

////////////////////////////////////////////////////////////////
//
// CFastList
//
// list/vector replacement using an ordered map to hold a list of items
//  - The ordered map value is a pointer to the item
//  - The ordered map key is the (relative) index
//
// A second map is used to store data for each item
//      (Using storage in the item class would help speed up inserts a bit)
//
// Pros:
//    1. Fast contains() and remove()
// Cons:
//    1. Slightly slower insert
//    2. Only works with pointers
//    3. Items can only appear in the list once
//    4. ITEMS CAN ONLY APPEAR IN THE LIST ONCE
//
////////////////////////////////////////////////////////////////
template <class Value>
class CFastList
{
public:
    using value_type = Value;

    using MapType = typename std::map<std::uint32_t, value_type>;
    using MapTypePair = typename std::pair<std::uint32_t, value_type>;
    using InfoType = typename std::map<value_type, std::uint32_t>;

    enum class EOperation
    {
        PushBack,
        PushFront,
        Remove,
    };

    struct SSuspendedOperation
    {
        EOperation operation;
        value_type item;
    };

    std::uint32_t                    m_revision;                  // Incremented every time the ordered map changes
    std::uint32_t                    m_nextFrontIndex;            // Next (decrementing) index to use as a map key for items added to the front
    std::uint32_t                    m_nextBackIndex;             // Next (incrementing) index to use as a map key for items added to the back
    MapType                          m_orderedMap;                  // Ordered map of items
    InfoType                         m_infoMap;                     // info for each item
    bool                             m_suspendingModifyOperations;
    std::vector<SSuspendedOperation> m_suspendedOperationList;

    CFastList() noexcept : m_revision(1), m_nextFrontIndex(UINT_MAX / 2 - 1), m_nextBackIndex(UINT_MAX / 2), m_suspendingModifyOperations(false)
    {}

    const value_type& front() const noexcept { return m_orderedMap.begin()->second; }

    void pop_front()
    {
        dassert(!m_suspendingModifyOperations);
        remove(front());
    }

    void push_front(const value_type& item)
    {
        if (m_suspendingModifyOperations)
        {
            m_suspendedOperationList.push_back({EOperation::PushFront, item});
            return;
        }
        // Check if indexing will wrap (and so destroy map order)
        if (m_nextFrontIndex < 5000)
            Reindex();
        dassert(m_orderedMap.find(m_nextFrontIndex) == m_orderedMap.end());
        // Optimized insert at the map beginning
        m_orderedMap.insert(m_orderedMap.begin(), MapTypePair(m_nextFrontIndex, item));
        m_revision++;
        SetItemIndex(item, m_nextFrontIndex);
        m_nextFrontIndex--;
    }

    void push_back(const value_type& item)
    {
        if (m_suspendingModifyOperations)
        {
            m_suspendedOperationList.push_back({EOperation::PushBack, item});
            return;
        }
        // Check if indexing will wrap (and so destroy map order)
        if (m_nextBackIndex > UINT_MAX - 5000)
            Reindex();
        dassert(m_orderedMap.find(m_nextBackIndex) == m_orderedMap.end());
        // Optimized insert at the map end
        m_orderedMap.insert(m_orderedMap.end(), MapTypePair(m_nextBackIndex, item));
        m_revision++;
        SetItemIndex(item, m_nextBackIndex);
        m_nextBackIndex++;
    }

    bool        contains(const value_type& item) const noexcept {
        return m_infoMap.find(item) != m_infoMap.end();
    }
    std::size_t size() const noexcept { return m_orderedMap.size(); }
    bool        empty() const noexcept { return m_orderedMap.empty(); }

    void clear()
    {
        dassert(!m_suspendingModifyOperations);
        m_orderedMap.clear();
        m_revision++;
        m_infoMap.clear();
        m_nextBackIndex = UINT_MAX / 2;
        m_nextFrontIndex = m_nextFrontIndex - 1;
    }

    void remove(const value_type& item)
    {
        if (m_suspendingModifyOperations)
        {
            m_suspendedOperationList.push_back({EOperation::Remove, item});
            return;
        }
        auto i = GetItemIndex(item);
        if (!i)
            return;

        auto it = m_orderedMap.find(i);
        dassert(it != m_orderedMap.end());
        dassert(it->second == item);
        m_orderedMap.erase(it);
        m_revision++;
        RemoveItemIndex(item);
    }

    std::uint32_t GetRevision() const noexcept { return m_revision; }

    // Queue remove/push_back/push_front operations until ResumeModifyOperations is called
    void SuspendModifyOperations()
    {
        dassert(!m_suspendingModifyOperations);
        m_suspendingModifyOperations = true;
    }

    // Replay queued operations
    void ResumeModifyOperations()
    {
        dassert(m_suspendingModifyOperations);
        m_suspendingModifyOperations = false;
        for (const auto& suspendedOperation : m_suspendedOperationList)
        {
            if (suspendedOperation.operation == EOperation::PushBack)
                push_back(suspendedOperation.item);
            else if (suspendedOperation.operation == EOperation::PushFront)
                push_front(suspendedOperation.item);
            else if (suspendedOperation.operation == EOperation::Remove)
                remove(suspendedOperation.item);
        }
        m_suspendedOperationList.clear();
    }

protected:
    // Reset indexing
    void Reindex()
    {
        m_nextBackIndex = UINT_MAX / 2;
        m_nextFrontIndex = m_nextBackIndex - 1;
        MapType newMap;
        for (const auto& [__, item] : m_orderedMap)
        {
            newMap[m_nextBackIndex] = item;
            RemoveItemIndex(item);
            SetItemIndex(item, m_nextBackIndex);
            m_nextBackIndex++;
        }
        m_orderedMap = newMap;
    }

    // Handle storage of the item index
    std::uint32_t GetItemIndex(const value_type& item) const noexcept
    {
        auto it = m_infoMap.find(item);
        if (it == m_infoMap.end())
            return 0;
        return it->second;
    }

    // Item must not exist in the infoMap
    void SetItemIndex(const value_type& item, std::uint32_t index)
    {
        auto it = m_infoMap.find(item);
        assert(it == m_infoMap.end() && index);
        m_infoMap[item] = index;
    }

    // Item must exist in the infoMap
    void RemoveItemIndex(const value_type& item)
    {
        auto it = m_infoMap.find(item);
        assert(it != m_infoMap.end());
        m_infoMap.erase(it);
    }

public:
    //
    // ConstIterator / Iterator
    //
    class ConstIterator
    {
    public:
        typename MapType::const_iterator iter;

        ConstIterator(typename MapType::const_iterator initer) noexcept : iter(initer){}
        bool              operator==(const ConstIterator& other) const noexcept { return iter == other.iter; }
        bool              operator!=(const ConstIterator& other) const noexcept { return iter != other.iter; }
        void              operator++() noexcept { ++iter; }
        void              operator++(int) noexcept { iter++; }
        const value_type& operator*() const noexcept { return iter->second; }
    };

    class Iterator
    {
    public:
        typename MapType::iterator iter;

        Iterator(typename MapType::iterator initer) noexcept : iter(initer){}
        bool              operator==(const Iterator& other) const noexcept { return iter == other.iter; }
        bool              operator!=(const Iterator& other) const noexcept { return iter != other.iter; }
        void              operator++() noexcept { ++iter; }
        void              operator++(int) noexcept { iter++; }
        const value_type& operator*() const noexcept { return iter->second; }
        operator ConstIterator() const noexcept { return ConstIterator(iter); }
    };

    //
    // ConstReverseIterator / ReverseIterator
    //
    class ConstReverseIterator
    {
    public:
        typename MapType::const_reverse_iterator iter;

        ConstReverseIterator(typename MapType::const_reverse_iterator initer) noexcept : iter(initer){}
        bool              operator==(const ConstReverseIterator& other) const noexcept { return iter == other.iter; }
        bool              operator!=(const ConstReverseIterator& other) const noexcept { return iter != other.iter; }
        void              operator++() noexcept { ++iter; }
        void              operator++(int) noexcept { iter++; }
        const value_type& operator*() const noexcept { return iter->second; }
    };

    class ReverseIterator
    {
    public:
        typename MapType::reverse_iterator iter;

        ReverseIterator(typename MapType::reverse_iterator initer) noexcept : iter(initer){}
        bool              operator==(const ReverseIterator& other) const noexcept { return iter == other.iter; }
        bool              operator!=(const ReverseIterator& other) const noexcept { return iter != other.iter; }
        void              operator++() noexcept { ++iter; }
        void              operator++(int) noexcept { iter++; }
        const value_type& operator*() const noexcept { return iter->second; }
        operator ConstReverseIterator() const noexcept { return ConstReverseIterator(iter); }
    };

    ConstIterator begin() const noexcept { return ConstIterator(m_orderedMap.begin()); }
    ConstIterator end() const noexcept { return ConstIterator(m_orderedMap.end()); }
    Iterator      begin() noexcept { return Iterator(m_orderedMap.begin()); }
    Iterator      end() noexcept { return Iterator(m_orderedMap.end()); }

    ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(m_orderedMap.rbegin()); }
    ConstReverseIterator rend() const noexcept { return ConstReverseIterator(m_orderedMap.rend()); }
    ReverseIterator      rbegin() noexcept { return ReverseIterator(m_orderedMap.rbegin()); }
    ReverseIterator      rend() noexcept { return ReverseIterator(m_orderedMap.rend()); }

    // Allow use of std iterator names
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = ReverseIterator;
    using const_reverse_iterator = ConstReverseIterator;

    Iterator erase(Iterator pos) noexcept
    {
        RemoveItemIndex(*pos);
        m_orderedMap.erase(pos.iter++);
        m_revision++;
        return pos;
    }

    Iterator erase(Iterator first, Iterator last) noexcept {
        Iterator outIter;
        for (auto it = first; it != last; it++)
        {
            erase(it);
            outIter = it;
        }
        return outIter;
    }
};

// Returns true if the item is in the itemList
template <class T, class U>
constexpr bool ListContains(const CFastList<T*>& list, const U& item) noexcept
{
    return list.contains(item);
}

// Remove all occurrences of item from itemList (There should never be more than one anyway)
template <class T, class U>
constexpr void ListRemoveAll(CFastList<T*>& list, const U& item) noexcept
{
    list.remove(item);
}
