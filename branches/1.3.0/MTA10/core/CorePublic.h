/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#define DLL_IMPORT	__declspec(dllimport)	/* Import function from DLL */
#define DLL_EXPORT  __declspec(dllexport)	/* Export function to DLL */

// API definition.
#ifndef CORE_API
#define CORE_API DLL_IMPORT
#endif

#include "SharedUtil.Profiling.h"

CORE_API extern CStatEvents g_StatEvents;

#include "CRealtimeStats.h"
