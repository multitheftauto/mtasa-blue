#pragma message("Compiling precompiled header.\n")

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#endif

#include <errno.h>
#include <stdarg.h>

#ifndef snprintf
#define snprintf _snprintf
#endif

// SDK includes
#include <Common.h>
#include "CXMLArray.h"
#include "CXMLAttributeImpl.h"
#include "CXMLAttributesImpl.h"
#include "CXMLFileImpl.h"
#include "CXMLNodeImpl.h"
#include "CXMLImpl.h"
#include "SharedUtil.h"

// XML includes
#include <xml/CXMLCommon.h>
