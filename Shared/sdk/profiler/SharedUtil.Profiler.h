/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Support for profiler sections
 *
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

//
//
// Do not remove this file or change any usage of DECLARE_PROFILER_SECTION
//
// They are to assist our AQtest plugin identify files when testing the released version
//
//

#if MTASA_VERSION_TYPE >= VERSION_TYPE_UNSTABLE && defined(_DECLARE_PROFILER_SECTION)
    #define DECLARE_PROFILER_SECTION(tag) \
        _DECLARE_PROFILER_SECTION(__FILE__, tag)
#else
    #define DECLARE_PROFILER_SECTION(tag)
#endif
