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
template <class T>
class CFastList
{
public:
    typedef typename std::map<uint, T>  MapType;
    typedef typename std::pair<uint, T> MapTypePair;
    typedef typename std::map<T, uint>  InfoType;
    enum class EOperation
    {
        PushBack,
        PushFront,
        Remove,
    };
    struct SSuspendedOperation
    {
        EOperation operation;
        T          item;
    };

    uint                             uiRevision;                  // Incremented every time the ordered map changes
    uint                             uiNextFrontIndex;            // Next (decrementing) index to use as a map key for items added to the front
    uint                             uiNextBackIndex;             // Next (incrementing) index to use as a map key for items added to the back
    MapType                          orderedMap;                  // Ordered map of items
    InfoType                         infoMap;                     // info for each item
    bool                             m_bSuspendingModifyOperations;
    std::vector<SSuspendedOperation> m_SuspendedOperationList;

    CFastList() : uiRevision(1), uiNextFrontIndex(UINT_MAX / 2 - 1), uiNextBackIndex(UINT_MAX / 2), m_bSuspendingModifyOperations(false)
    {
        #ifdef MTA_DEBUG
        // T must be a pointer
        void* ptr = (T)NULL;
        ptr = NULL;
        #endif
    }

    const T& front() const { return orderedMap.begin()->second; }

    void pop_front()
    {
        dassert(!m_bSuspendingModifyOperations);
        T item = front();
        remove(item);
    }

    void push_front(const T& item)
    {
        if (m_bSuspendingModifyOperations)
        {
            m_SuspendedOperationList.push_back({EOperation::PushFront, item});
            return;
        }
        // Check if indexing will wrap (and so destroy map order)
        if (uiNextFrontIndex < 5000)
            Reindex();
        dassert(orderedMap.find(uiNextFrontIndex) == orderedMap.end());
        // Optimized insert at the map beginning
        orderedMap.insert(orderedMap.begin(), MapTypePair(uiNextFrontIndex, item));
        uiRevision++;
        SetItemIndex(item, uiNextFrontIndex);
        uiNextFrontIndex--;
    }

    void push_back(const T& item)
    {
        if (m_bSuspendingModifyOperations)
        {
            m_SuspendedOperationList.push_back({EOperation::PushBack, item});
            return;
        }
        // Check if indexing will wrap (and so destroy map order)
        if (uiNextBackIndex > UINT_MAX - 5000)
            Reindex();
        dassert(orderedMap.find(uiNextBackIndex) == orderedMap.end());
        // Optimized insert at the map end
        orderedMap.insert(orderedMap.end(), MapTypePair(uiNextBackIndex, item));
        uiRevision++;
        SetItemIndex(item, uiNextBackIndex);
        uiNextBackIndex++;
    }

    bool contains(const T& item) const { return (infoMap.find(item) != infoMap.end()); }

    size_t size() const { return orderedMap.size(); }

    bool empty() const { return orderedMap.empty(); }

    void clear()
    {
        dassert(!m_bSuspendingModifyOperations);
        orderedMap.clear();
        uiRevision++;
        infoMap.clear();
        uiNextFrontIndex = UINT_MAX / 2 - 1;
        uiNextBackIndex = UINT_MAX / 2;
    }

    void remove(const T& item)
    {
        if (m_bSuspendingModifyOperations)
        {
            m_SuspendedOperationList.push_back({EOperation::Remove, item});
            return;
        }
        if (uint uiIndex = GetItemIndex(item))
        {
            typename MapType::iterator it = orderedMap.find(uiIndex);
            dassert(it != orderedMap.end());
            dassert(it->second == item);
            orderedMap.erase(it);
            uiRevision++;
            RemoveItemIndex(item);
        }
    }

    uint GetRevision() const { return uiRevision; }

    // Queue remove/push_back/push_front operations until ResumeModifyOperations is called
    void SuspendModifyOperations()
    {
        dassert(!m_bSuspendingModifyOperations);
        m_bSuspendingModifyOperations = true;
    }

    // Replay queued operations
    void ResumeModifyOperations()
    {
        dassert(m_bSuspendingModifyOperations);
        m_bSuspendingModifyOperations = false;
        for (const auto& suspendedOperation : m_SuspendedOperationList)
        {
            if (suspendedOperation.operation == EOperation::PushBack)
                push_back(suspendedOperation.item);
            else if (suspendedOperation.operation == EOperation::PushFront)
                push_front(suspendedOperation.item);
            else if (suspendedOperation.operation == EOperation::Remove)
                remove(suspendedOperation.item);
        }
        m_SuspendedOperationList.clear();
    }

protected:
    // Reset indexing
    void Reindex()
    {
        uiNextFrontIndex = UINT_MAX / 2 - 1;
        uiNextBackIndex = UINT_MAX / 2;
        MapType newMap;
        for (typename MapType::iterator iter = orderedMap.begin(); iter != orderedMap.end(); ++iter)
        {
            T item = iter->second;
            newMap[uiNextBackIndex] = item;
            RemoveItemIndex(item);
            SetItemIndex(item, uiNextBackIndex);
            uiNextBackIndex++;
        }
        orderedMap = newMap;
    }

    // Handle storage of the item index
    uint GetItemIndex(const T& item) const
    {
        typename InfoType::const_iterator it = infoMap.find(item);
        if (it == infoMap.end())
            return 0;
        return it->second;
    }

    // Item must not exist in the infoMap
    void SetItemIndex(const T& item, uint uiIndex)
    {
        typename InfoType::const_iterator it = infoMap.find(item);
        assert(it == infoMap.end() && uiIndex);
        infoMap[item] = uiIndex;
    }

    // Item must exist in the infoMap
    void RemoveItemIndex(const T& item)
    {
        typename InfoType::iterator it = infoMap.find(item);
        assert(it != infoMap.end());
        infoMap.erase(it);
    }

public:
    //
    // ConstIterator / Iterator
    //
    class ConstIterator
    {
    public:
        typename MapType::const_iterator iter;

        ConstIterator(typename MapType::const_iterator initer) : iter(initer) {}
        bool     operator==(const ConstIterator& other) const { return iter == other.iter; }
        bool     operator!=(const ConstIterator& other) const { return iter != other.iter; }
        void     operator++() { ++iter; }
        void     operator++(int) { iter++; }
        const T& operator*() const { return iter->second; }
    };

    class Iterator
    {
    public:
        typename MapType::iterator iter;

        Iterator(typename MapType::iterator initer) : iter(initer) {}
        bool     operator==(const Iterator& other) const { return iter == other.iter; }
        bool     operator!=(const Iterator& other) const { return iter != other.iter; }
        void     operator++() { ++iter; }
        void     operator++(int) { iter++; }
        const T& operator*() const { return iter->second; }
                 operator ConstIterator() const { return ConstIterator(iter); }
    };

    //
    // ConstReverseIterator / ReverseIterator
    //
    class ConstReverseIterator
    {
    public:
        typename MapType::const_reverse_iterator iter;

        ConstReverseIterator(typename MapType::const_reverse_iterator initer) : iter(initer) {}
        bool     operator==(const ConstReverseIterator& other) const { return iter == other.iter; }
        bool     operator!=(const ConstReverseIterator& other) const { return iter != other.iter; }
        void     operator++() { ++iter; }
        void     operator++(int) { iter++; }
        const T& operator*() const { return iter->second; }
    };

    class ReverseIterator
    {
    public:
        typename MapType::reverse_iterator iter;

        ReverseIterator(typename MapType::reverse_iterator initer) : iter(initer) {}
        bool     operator==(const ReverseIterator& other) const { return iter == other.iter; }
        bool     operator!=(const ReverseIterator& other) const { return iter != other.iter; }
        void     operator++() { ++iter; }
        void     operator++(int) { iter++; }
        const T& operator*() const { return iter->second; }
                 operator ConstReverseIterator() const { return ConstReverseIterator(iter); }
    };

    ConstIterator begin() const { return ConstIterator(orderedMap.begin()); }
    ConstIterator end() const { return ConstIterator(orderedMap.end()); }
    Iterator      begin() { return Iterator(orderedMap.begin()); }
    Iterator      end() { return Iterator(orderedMap.end()); }

    ConstReverseIterator rbegin() const { return ConstReverseIterator(orderedMap.rbegin()); }
    ConstReverseIterator rend() const { return ConstReverseIterator(orderedMap.rend()); }
    ReverseIterator      rbegin() { return ReverseIterator(orderedMap.rbegin()); }
    ReverseIterator      rend() { return ReverseIterator(orderedMap.rend()); }

    // Allow use of std iterator names
    typedef Iterator             iterator;
    typedef ConstIterator        const_iterator;
    typedef ReverseIterator      reverse_iterator;
    typedef ConstReverseIterator const_reverse_iterator;

    Iterator erase(Iterator iter)
    {
        RemoveItemIndex(*iter);
        orderedMap.erase(iter.iter++);
        uiRevision++;
        return iter;
    }
};

// Returns true if the item is in the itemList
template <class T, class U>
bool ListContains(const CFastList<T*>& itemList, const U& item)
{
    if (itemList.empty())
        return false;
    return itemList.contains(item);
}

// Remove all occurrences of item from itemList (There should never be more than one anyway)
template <class T, class U>
void ListRemove(CFastList<T*>& itemList, const U& item)
{
    itemList.remove(item);
}
