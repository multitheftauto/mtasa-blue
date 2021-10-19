/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        publicsdk/include/ILuaModuleManager.h
 *  PURPOSE:     Lua dynamic module interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// INTERFACE for Lua dynamic modules

/* Interface for modules until DP2.3 */
class ILuaModuleManager
{
public:
    virtual void ErrorPrintf(const char* szFormat, ...) = 0;
    virtual void DebugPrintf(lua_State* luaVM, const char* szFormat, ...) = 0;
    virtual void Printf(const char* szFormat, ...) = 0;

    virtual bool RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func) = 0;
    virtual bool GetResourceName(
        lua_State* luaVM, std::string& strName) = 0;            // This function might not work if module and MTA were compiled with different compiler versions
    virtual CChecksum GetResourceMetaChecksum(lua_State* luaVM) = 0;
    virtual CChecksum GetResourceFileChecksum(lua_State* luaVM, const char* szFile) = 0;
};

/* Interface for modules until 1.0 */
class ILuaModuleManager10 : public ILuaModuleManager
{
public:
    virtual unsigned long GetVersion() = 0;
    virtual const char*   GetVersionString() = 0;
    virtual const char*   GetVersionName() = 0;
    virtual unsigned long GetNetcodeVersion() = 0;
    virtual const char*   GetOperatingSystemName() = 0;

    virtual lua_State* GetResourceFromName(const char* szResourceName) = 0;

    // GetResourceName above doesn't work if module and MTA were compiled with different compiler versions
    virtual bool GetResourceName(lua_State* luaVM, char* szName, size_t length) = 0;
    virtual bool GetResourceFilePath(lua_State* luaVM, const char* fileName, char* path, size_t length) = 0;
};

class ILuaModuleManager20 : public ILuaModuleManager10
{
public:
    virtual IResource* GetResourceFromNameV2(const char* szResourceName) = 0;
    virtual IResource* GetResourceFromLuaState(lua_State* luaVM) = 0;
};

// Wants to add new methods? create new class that inherits from ILuaModuleManager20, call it eg: ILuaModuleManager21
// Add your methods and update version in MTA_API_VERSION and MTA_API_REVISION in CLuaModule.cpp
// where 21 - 2 version, 1 - revision. Remember to update CLuaModule to inherit from new class you created
// Wants to update existing function? Bump version and add method with `VX` where X is next version
