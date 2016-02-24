/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        StdInc.h
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <string>

#include "MTAPlatform.h"
#ifndef WIN32
    // Linux allocation tracking doesn't work in this module for some reason
    #define WITH_ALLOC_TRACKING 0
#endif
#include "SharedUtil.h"
#include "../version.h"

#ifdef WIN32
    #include "CExceptionInformation_Impl.h"
#endif

void WaitForKey ( int iKey );
void Print ( const char* szFormat, ... );
