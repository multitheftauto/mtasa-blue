/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*  PURPOSE:
*  DEVELOPERS:
*
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
//
// Do not remove this file or change any usage of DECLARE_PROFILER_SECTION 
//
// They are to assist our AQtest plugin identify files when testing the released version
//
//

#if MTASA_VERSION_TYPE >= VERSION_TYPE_UNSTABLE
    #ifndef _DECLARE_PROFILER_SECTION
        #error Test plugin missing
    #endif
    #define DECLARE_PROFILER_SECTION(tag) \
        _DECLARE_PROFILER_SECTION(__FILE__, tag)
#else
    #define DECLARE_PROFILER_SECTION(tag)
#endif
