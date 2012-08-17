
#ifdef WIN32
#pragma message("Compiling precompiled header.\n")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <limits.h>
#endif

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

// SDK includes

#include "Common.h"
#include "CXMLAttributeImpl.h"
#include "CXMLAttributesImpl.h"
#include "CXMLFileImpl.h"
#include "CXMLNodeImpl.h"
#include "CXMLImpl.h"
#include "SharedUtil.h"
#include "CXMLArray.h"

// XML includes
#include <xml/CXMLCommon.h>
