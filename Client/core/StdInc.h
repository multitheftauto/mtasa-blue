
// Pragmas
#pragma warning (disable:4995)
#pragma warning (disable:4244)

// #define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define MTA_CLIENT
#define SHARED_UTIL_WITH_FAST_HASH_MAP
#define SHARED_UTIL_WITH_SYS_INFO
#include "SharedUtil.h"

#include <sys/stat.h>
#include <d3d9.h>
#include <direct.h>
#include <stdio.h>
#include <dbghelp.h>
#include <tchar.h>
#include <windowsx.h>
#include <time.h>
#include <shlwapi.h>
#include <winsock.h>
#include <conio.h>

#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <functional>

// Localization
#include "../../vendor/tinygettext/tinygettext.hpp"
#include "CLocalization.h"

// SDK includes
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <xml/CXMLAttribute.h>
#include <xml/CXMLAttributes.h>
#include <Common.h>

#include <net/CNet.h>
#include <multiplayer/CMultiplayer.h>
#include <game/CGame.h>

// Core-level includes
#include "CQueryReceiver.h"
#include "CrashHandler.h"
#include "CCore.h"
#include "CDebugView.h"
#include "CChat.h"
#include <DXHook/CDirectInputHookManager.h>
#include <DXHook/CDirect3DEvents9.h>
#include <DXHook/CProxyDirectInput8.h>
#include <DXHook/CProxyDirect3D9.h>
#include "CCommandFuncs.h"
#include "CExceptionInformation_Impl.h"
#include "CJoystickManager.h"
#include <Graphics/CVideoModeManager.h>
#include "CCrashDumpWriter.h"
#include "CMemStats.h"
#include "CGraphStats.h"
#include "CNickGen.h"
#include <core/CWebViewInterface.h>
#include "CTrayIcon.h"
