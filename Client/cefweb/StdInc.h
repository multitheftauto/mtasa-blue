
#include <winsock.h>
#define MTA_CLIENT
#define SHARED_UTIL_WITH_FAST_HASH_MAP
#include "SharedUtil.h"

// SDK includes
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <xml/CXMLAttribute.h>
#include <xml/CXMLAttributes.h>
#include <Common.h>

#include <net/CNet.h>
#include <core/CClientBase.h>
#include <core/CClientEntityBase.h>
#include <core/CCoreInterface.h>
#include <core/CLocalizationInterface.h>
#include "version.h"

#include "CWebCore.h"
#include "CWebView.h"

extern CCoreInterface* g_pCore;
extern CLocalizationInterface* g_pLocalization;
