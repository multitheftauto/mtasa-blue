/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CLinkSA.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

template <typename T>
class CLinkSA
{
public:
    T           data;
    CLinkSA<T>* prev;
    CLinkSA<T>* next;

    void* operator new(std::size_t size) { return ((void*(__cdecl*)(std::size_t))0x821195)(size); }
    void* operator new[](std::size_t size) { return ((void*(__cdecl*)(std::size_t))0x821195)(size); }
    void  operator delete(void* ptr) { ((void(__cdecl*)(void*))0x8213AE)(ptr); }
    void  operator delete[](void* ptr) { ((void(__cdecl*)(void*))0x8213AE)(ptr); }

    void Remove()
    {
        next->prev = prev;
        prev->next = next;
    }

    // If `this` is already in another list, `Remove()` must first be called! (Not doing so will result in the list (`this` is in) getting corrupted)
    void Insert(CLinkSA<T>* after)
    {
        next = after->next;
        next->prev = this;

        prev = after;
        prev->next = this;
    }
};
static_assert(sizeof(CLinkSA<int>) == 0xC, "Invalid size for CLinkSA class!");
