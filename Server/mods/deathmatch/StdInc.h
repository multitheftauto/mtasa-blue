/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        StdInc.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <mutex>
#include <unordered_set>
#include <future>

#define SHARED_UTIL_WITH_FAST_HASH_MAP
#include "SharedUtil.h"
#include "MTAPlatform.h"

#ifdef WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
//
// Windows
//
    #include <winsock2.h>
    #include <windows.h>
    #include <mmsystem.h>
#else
//
// Non-Windows: provide the Windows Sleep() spelling used throughout this
// module. This used to be supplied transitively by the EHS vendor headers
// (vendor/ehs/socket.h); now that EHS is gone we define it ourselves, matching
// the equivalent shim in Server/core/StdInc.h.
    #include <unistd.h>
    #ifndef Sleep
        #define Sleep(duration) usleep((duration) * 1000)
    #endif
#endif

#include <net/CSimControl.h>
#include <xml/CXML.h>
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <xml/CXMLAttributes.h>
#include <xml/CXMLAttribute.h>
#include <CVector.h>
#include <CVector4D.h>
#include <CSphere.h>
#include <CBox.h>
#include <CMatrix.h>
#include "CStringName.h"
#include <bochs_internal/bochs_crc32.h>
#include <pcrecpp_compat.h>
#include <pthread.h>
#include "version.h"

extern class CNetServer* g_pRealNetServer;
extern class CGame*      g_pGame;
