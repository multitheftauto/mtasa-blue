/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CLuaFunctionError.h
 *  PURPOSE:     Exception class for safe Lua error unwinding
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

//
// Exception thrown during Lua function execution to safely unwind C++ stack
// objects before luaL_error is dispatched at outer boundary.
//
class LuaFunctionError
{
protected:
    std::string m_strMessage;
    bool        m_bWarning;

public:
    LuaFunctionError(std::string strMessage, bool bThrowWarning = true) noexcept : m_strMessage(std::move(strMessage)), m_bWarning(bThrowWarning) {}

    const char* what() const noexcept { return m_strMessage.c_str(); }
    bool        IsWarning() const noexcept { return m_bWarning; }
};
