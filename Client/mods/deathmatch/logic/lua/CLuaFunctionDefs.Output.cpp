/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Output.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::OutputConsole(lua_State* luaVM)
{
    SString          strText = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strText);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::OutputConsole(strText))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::OutputChatBox(lua_State* luaVM)
{
    SString          strText = "";
    unsigned char    ucRed = 231;
    unsigned char    ucGreen = 217;
    unsigned char    ucBlue = 176;
    bool             bColorCoded = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strText);
    argStream.ReadNumber(ucRed, 231);
    argStream.ReadNumber(ucGreen, 217);
    argStream.ReadNumber(ucBlue, 176);
    argStream.ReadBool(bColorCoded, false);

    if (!argStream.HasErrors())
    {
        // Output to the chatbox
        if (CStaticFunctionDefinitions::OutputChatBox(strText, ucRed, ucGreen, ucBlue, bColorCoded))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::ClearChatBox(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ClearChatBox())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetClipboard(lua_State* luaVM)
{
    SString          strText = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strText);

    if (!argStream.HasErrors())
    {
        // set to clip board
        if (CStaticFunctionDefinitions::SetClipboard(strText))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::OutputClientDebugString(lua_State* luaVM)
{
    SString          strText = "";
    unsigned int     uiLevel = 3;
    unsigned char    ucRed = 255;
    unsigned char    ucGreen = 255;
    unsigned char    ucBlue = 255;
    CScriptArgReader argStream(luaVM);
    argStream.ReadAnyAsString(strText);
    argStream.ReadNumber(uiLevel, 3);

    if (uiLevel == 0 || uiLevel == 4)
    {
        argStream.ReadNumber(ucRed, 255);
        argStream.ReadNumber(ucGreen, 255);
        argStream.ReadNumber(ucBlue, 255);
    }

    // Too big level?
    if (uiLevel > 4)
    {
        argStream.SetCustomError("Bad level argument");
    }

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            switch (uiLevel)
            {
                case 0:
                    m_pScriptDebugging->LogDebug(luaVM, ucRed, ucGreen, ucBlue, "%s", strText.c_str());
                    break;
                case 1:
                    m_pScriptDebugging->LogError(luaVM, "%s", strText.c_str());
                    break;
                case 2:
                    m_pScriptDebugging->LogWarning(luaVM, "%s", strText.c_str());
                    break;
                case 3:
                    m_pScriptDebugging->LogInformation(luaVM, "%s", strText.c_str());
                    break;
                case 4:
                    m_pScriptDebugging->LogCustom(luaVM, ucRed, ucGreen, ucBlue, "%s", strText.c_str());
                    break;
            }

            // Success
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetWindowFlashing(lua_State* luaVM)
{
    //  bool setWindowFlashing ( bool flash [, int count = 10 ] )
    bool flash;
    uint count;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(flash);
    argStream.ReadNumber(count, 10);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::SetWindowFlashing(flash, count));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
