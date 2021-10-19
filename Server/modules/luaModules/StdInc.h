class CLuaModuleManager;

#define MAX_INFO_LENGTH 128

class CChecksum
{
public:
    unsigned long ulCRC;
    unsigned char mD5[16];
};


extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#ifdef WIN32
    #include <windows.h>
#endif

#include "string"
#include "vector"

//#include "sdk/SharedUtil.File.hpp"
//#include "sdk/std::string.hpp"
#include "publicsdk/IModule.h"
#include "publicsdk/include/ILuaModuleManager.h"
#include "CLuaModule.h"
#include "CLuaModuleManager.h"
#include "CLuaModulesModule.h"
