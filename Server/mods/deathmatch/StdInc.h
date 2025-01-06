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
//
// Windows
//
    #include <winsock2.h>
    #include <windows.h>
    #include <mmsystem.h>
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
#include <bochs_internal/bochs_crc32.h>
#include <pcrecpp.h>
#include <pthread.h>
#include "version.h"

extern class CNetServer* g_pRealNetServer;
extern class CGame*      g_pGame;
