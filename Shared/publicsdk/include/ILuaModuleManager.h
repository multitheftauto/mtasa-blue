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

#pragma once

#define MAX_INFO_LENGTH 128

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}
#include <string>
#include <optional>

#ifndef __CChecksum_H
#define __CChecksum_H
struct CChecksum
{
    unsigned long ulCRC;
    unsigned char mD5[16];
};
#endif

#include <CVehicleManager.h>
#include <CMainConfig.h>
#include <CTeamManager.h>

class ILuaModuleManager
{
public:
    virtual unsigned long GetVersion() const noexcept = 0;
    virtual const char*   GetVersionString() const noexcept = 0;
    virtual const char*   GetVersionName() const noexcept = 0;
    virtual unsigned long GetNetcodeVersion() const noexcept = 0;
    virtual const char*   GetOperatingSystemName() const noexcept = 0;

    virtual void ErrorPrintf(const char* szFormat, ...) const noexcept = 0;
    virtual void DebugPrintf(lua_State* luaVM, const char* szFormat, ...) const noexcept = 0;
    virtual void Printf(const char* szFormat, ...) const noexcept = 0;

    virtual bool RegisterFunction(lua_State* luaVM, const char* szFunctionName,
        lua_CFunction Func) = 0;

    virtual lua_State*  GetResourceFromName(const char* szResourceName) const noexcept = 0;
    virtual std::string GetResourceName(lua_State* luaVM) const noexcept = 0;

    virtual std::optional<std::string> GetResourceFilePath(lua_State* luaVM, const char* fileName) const noexcept = 0;
    virtual CChecksum                  GetResourceMetaChecksum(lua_State* luaVM) const noexcept = 0;
    virtual CChecksum                  GetResourceFileChecksum(lua_State* luaVM, const char* szFile) const noexcept = 0;

    virtual CVehicleManager* GetVehicleManager() const noexcept = 0;
    virtual CMainConfig*     GetConfigManager() const noexcept = 0;
    virtual CTeamManager*    GetTeamManager() const noexcept = 0;
};
