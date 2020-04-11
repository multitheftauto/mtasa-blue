#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <limits.h>
#endif

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#ifndef WIN32
    #include <unistd.h>
#endif

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

// SDK
#include "Common.h"
#include "../../Server/version.h"
#include "SharedUtil.h"

// Implementation
#include "CXMLArray.h"
#include "CXMLAttributeImpl.h"
#include "CXMLFileImpl.h"
#include "CXMLImpl.h"
#include "CXMLNodeImpl.h"
