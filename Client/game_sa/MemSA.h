/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/MemSA.h
 *  PURPOSE:     C memory functions in SA
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

namespace MemSA
{
    size_t msize(const void* p)
    {
        using gta_msize = size_t (*__cdecl)(const void*);
        return ((gta_msize)0x828C4A)(p);
    };

    void* malloc(size_t count)
    {
        using gta_malloc = void* (*__cdecl)(size_t a1);
        return ((gta_malloc)0x824257)(count);
    };

    template<typename T>
    T* malloc_struct(size_t count)
    {
        return static_cast<T*>(MemSA::malloc(sizeof(T) * count));
    }

    void free(void* p)
    {
        using gta_free = void (*__cdecl)(void* p);
        return ((gta_free)0x82413F)(p);
    };
}
