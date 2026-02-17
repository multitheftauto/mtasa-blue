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
    inline size_t msize(const void* p)
    {
        using gta_msize = size_t (*)(const void*);
        return reinterpret_cast<gta_msize>(0x828C4A)(p);
    };

    inline void* malloc(size_t count)
    {
        using gta_malloc = void* (*)(size_t);
        return reinterpret_cast<gta_malloc>(0x824257)(count);
    };

    template <typename T>
    T* malloc_struct(size_t count)
    {
        return static_cast<T*>(MemSA::malloc(sizeof(T) * count));
    }

    inline void free(void* p)
    {
        using gta_free = void (*)(void*);
        return reinterpret_cast<gta_free>(0x82413F)(p);
    };
}
