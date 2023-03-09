/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Legacy.h
 *  PURPOSE:     Home for stuff removed from SharedUtil files.
 *               To prevent breaking other projects which depend on SharedUtil.
 *
 *****************************************************************************/
#pragma once

// NOMINMAX after windows.h has been included
#undef min
#undef max

namespace SharedUtil
{
    template <class T>
    T Min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template <class T>
    T Max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }
}            // namespace SharedUtil

#define PRId64  "lld"
#define PRIx64  "llx"

// Crazy thing
#define LOCAL_FUNCTION_START    struct local {
#define LOCAL_FUNCTION_END      };
#define LOCAL_FUNCTION          local

// Inline callback definition for std::sort
#define sort_inline(a,b,c) \
        { \
            LOCAL_FUNCTION_START \
                static bool SortPredicate c \
            LOCAL_FUNCTION_END \
            std::sort ( a, b, LOCAL_FUNCTION::SortPredicate ); \
        }

// Inline callback definition for std::list::sort
#define sort_list_inline(a,c) \
        { \
            LOCAL_FUNCTION_START \
                static bool SortPredicate c \
            LOCAL_FUNCTION_END \
            a.sort ( LOCAL_FUNCTION::SortPredicate ); \
        }
