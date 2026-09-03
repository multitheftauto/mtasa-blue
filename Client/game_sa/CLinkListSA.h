/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CLinkListSA.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CLinkSA.h"
#include <cstddef>
#include <utility>

template <typename T>
class CLinkListSA
{
public:
    CLinkSA<T>  usedListHead{};
    CLinkSA<T>  usedListTail{};
    CLinkSA<T>  freeListHead{};
    CLinkSA<T>  freeListTail{};
    CLinkSA<T>* entries{};

    void* operator new(std::size_t size) { return ((void*(__cdecl*)(std::size_t))0x821195)(size); }
    void* operator new[](std::size_t size) { return ((void*(__cdecl*)(std::size_t))0x821195)(size); }
    void  operator delete(void* ptr) { ((void(__cdecl*)(void*))0x8213AE)(ptr); }
    void  operator delete[](void* ptr) { ((void(__cdecl*)(void*))0x8213AE)(ptr); }

    void Init(std::size_t count)
    {
        if (count == 0)
            return;

        usedListHead.next = &usedListTail;
        usedListTail.prev = &usedListHead;
        freeListHead.next = &freeListTail;
        freeListTail.prev = &freeListHead;

        entries = new CLinkSA<T>[count];
        for (std::int32_t i = count - 1; i >= 0; i--)
            entries[i].Insert(&freeListHead);
    }

    void Shutdown() { delete[] std::exchange(entries, nullptr); }

    void Insert(CLinkSA<T>& link)
    {
        link.Remove();
        link.Insert(&usedListHead);
    }

    CLinkSA<T>* Insert(T const& data)
    {
        CLinkSA<T>* link = freeListHead.next;
        if (link == &freeListTail)
            return nullptr;

        link->data = data;
        Insert(*link);
        return link;
    }

    CLinkSA<T>* InsertSorted(T const& data)
    {
        CLinkSA<T>* i = nullptr;
        for (i = usedListHead.next; i != &usedListTail; i = i->next)
        {
            if (i->data.distance >= data.distance)
                break;
        }

        CLinkSA<T>* link = freeListHead.next;
        if (link == &freeListTail)
            return nullptr;

        link->data = data;
        link->Remove();
        link->Insert(i->prev);
        return link;
    }

    void Clear()
    {
        for (CLinkSA<T>* link = usedListHead.next; link != &usedListTail; link = usedListHead.next)
            Remove(link);
    }

    auto Remove(CLinkSA<T>* l)
    {
        l->Remove();
        l->Insert(&freeListHead);
        return l;
    }

    auto  GetTail() { return usedListTail.prev; }
    auto& GetTailLink() { return usedListTail; }

    auto  GetHead() { return usedListHead.next; }
    auto& GetHeadLink() { return usedListHead; }
};
static_assert(sizeof(CLinkListSA<int>) == 0x34, "Invalid size for CLinkListSA class!");
