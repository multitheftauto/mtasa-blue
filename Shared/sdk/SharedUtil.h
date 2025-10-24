/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.h
 *  PURPOSE:     Solution wide utility functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "SharedUtil.IntTypes.h"
#include <assert.h>
#include "SharedUtil.Defines.h"
#include "SharedUtil.AllocTracking.h"
#include <list>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <chrono>

// Vendor
#ifndef _
#define _ //Use a dummy localisation define for modules that don't need it
#endif

#include "SString.h"
#include "WString.h"

#define _E(code) SString(" [%s]",code)

#include "SharedUtil.Legacy.h"
#include "SharedUtil.Map.h"
#if defined(SHARED_UTIL_WITH_HASH_MAP) || defined(SHARED_UTIL_WITH_FAST_HASH_MAP)
    #include "SharedUtil.HashMap.h"
#endif
#if defined(SHARED_UTIL_WITH_FAST_HASH_MAP)
    #include "SharedUtil.FastHashMap.h"
    #include "SharedUtil.FastHashSet.h"
#endif
#include "SharedUtil.Misc.h"
#include "SharedUtil.File.h"
#include "SharedUtil.Time.h"
#include "SharedUtil.Buffer.h"
#include "SharedUtil.Game.h"
#include "SharedUtil.Math.h"
#include "SharedUtil.ClassIdent.h"
#include "SharedUtil.Hash.h"
#include "SharedUtil.Crypto.h"
#if defined(SHARED_UTIL_WITH_SYS_INFO)
    #include "SharedUtil.SysInfo.h"
#endif
#include "SharedUtil.Profiling.h"
#include "SharedUtil.Logging.h"
#include "SharedUtil.AsyncTaskScheduler.h"
#include "SharedUtil.ThreadPool.h"
#include "CMtaVersion.h"
#include "CFastList.h"
#include "CDuplicateLineFilter.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#ifndef stricmp
#ifdef _MSC_VER
#define stricmp _stricmp
#else
#define stricmp strcasecmp
#endif
#endif

#ifndef strnicmp
#ifdef _MSC_VER
#define strnicmp _strnicmp
#else
#define strnicmp strncasecmp
#endif
#endif

// Ensure that the C++ version is at least C++20
#if __cplusplus >= 202002L

/*
 The function prevents a compilation error caused by using non-constant format strings and
 correctly forwards the arguments to std::format for formatting
*/
namespace mtasa
{
    template <class... _Types>
    inline std::string format(const std::string_view fmt, _Types&&... _Args)
    {
        return std::vformat(fmt, std::make_format_args(_Args...));
    }
}

#endif
