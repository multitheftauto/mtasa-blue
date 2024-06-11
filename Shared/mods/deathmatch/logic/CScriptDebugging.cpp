/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/CScriptDebugging.cpp
 *  PURPOSE:     Script debugging facility class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CScriptDebugging.h"

#ifndef MTA_CLIENT
    #include "CGame.h"
    #include "CMainConfig.h"
    #include "CMapManager.h"
#endif

#define MAX_STRING_LENGTH 2048

// Handle filename/line number in string
void CScriptDebugging::LogPCallError(lua_State* luaVM, const SString& strRes, bool bInitialCall)
{
    std::vector<SString> vecSplit;
    strRes.Split(":", vecSplit);

    if (vecSplit.size() >= 3)
    {
        // File+line info present
        SString strFile = vecSplit[0];
        int     iLine = atoi(vecSplit[1]);

        // Get the message string (we cannot use vecSplit here as the message itself could contain ':')
        auto    pos = strRes.find(':', vecSplit[0].length() + vecSplit[1].length());
        SString strMsg = strRes.SubStr(pos + 2);

        if (iLine == 0 && bInitialCall)
        {
            // Location hint for compiled scripts
            LogError(SLuaDebugInfo(strFile, iLine), "(global scope) %s", *strMsg);
        }
        else
            LogError(SLuaDebugInfo(strFile, iLine), "%s", *strMsg);
    }
    else
    {
        // File+line info not present
        LogError(luaVM, "%s", strRes.c_str());
    }
}

void CScriptDebugging::LogCustom(lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ...)
{
    assert(szFormat);

    // Compose the formatted message
    char    szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start(marker, szFormat);
    VSNPRINTF(szBuffer, MAX_STRING_LENGTH, szFormat, marker);
    va_end(marker);

    LogString("", GetLuaDebugInfo(luaVM), szBuffer, 4, ucRed, ucGreen, ucBlue);
}

void CScriptDebugging::LogDebug(lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ...)
{
    assert(szFormat);

    // Compose the formatted message
    char    szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start(marker, szFormat);
    VSNPRINTF(szBuffer, MAX_STRING_LENGTH, szFormat, marker);
    va_end(marker);

    LogString("", GetLuaDebugInfo(luaVM), szBuffer, 0, ucRed, ucGreen, ucBlue);
}

void CScriptDebugging::LogInformationV(lua_State* luaVM, const char* format, va_list vlist)
{
    assert(format);
    std::array<char, MAX_STRING_LENGTH> buffer{};
    VSNPRINTF(buffer.data(), buffer.size(), format, vlist);
    LogString("INFO: ", GetLuaDebugInfo(luaVM), buffer.data(), 3);
}

void CScriptDebugging::LogInformation(lua_State* luaVM, const char* szFormat, ...)
{
    va_list marker;
    va_start(marker, szFormat);
    LogInformationV(luaVM, szFormat, marker);
    va_end(marker);
}

void CScriptDebugging::LogWarning(lua_State* luaVM, const char* szFormat, ...)
{
    assert(szFormat);

    // Compose the formatted message
    char    szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start(marker, szFormat);
    VSNPRINTF(szBuffer, MAX_STRING_LENGTH, szFormat, marker);
    va_end(marker);

    // Log it
    LogString("WARNING: ", GetLuaDebugInfo(luaVM), szBuffer, 2);
}

void CScriptDebugging::LogError(lua_State* luaVM, const char* szFormat, ...)
{
    assert(szFormat);

    // Compose the formatted message
    char    szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start(marker, szFormat);
    VSNPRINTF(szBuffer, MAX_STRING_LENGTH, szFormat, marker);
    va_end(marker);

    // Log it
    LogString("ERROR: ", GetLuaDebugInfo(luaVM), szBuffer, 1);
}

void CScriptDebugging::LogWarning(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...)
{
    assert(szFormat);

    // Compose the formatted message
    char    szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start(marker, szFormat);
    VSNPRINTF(szBuffer, MAX_STRING_LENGTH, szFormat, marker);
    va_end(marker);

    // Log it
    LogString("WARNING: ", luaDebugInfo, szBuffer, 2);
}

void CScriptDebugging::LogError(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...)
{
    assert(szFormat);

    // Compose the formatted message
    char    szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start(marker, szFormat);
    VSNPRINTF(szBuffer, MAX_STRING_LENGTH, szFormat, marker);
    va_end(marker);

    // Log it
    LogString("ERROR: ", luaDebugInfo, szBuffer, 1);
}

void CScriptDebugging::LogBadPointer(lua_State* luaVM, const char* szArgumentType, unsigned int uiArgument)
{
    assert(szArgumentType);

    // Populate a message to print/send
    LogWarning(luaVM, "Bad '%s' pointer @ '%s'(%u)", szArgumentType, lua_tostring(luaVM, lua_upvalueindex(1)), uiArgument);
}

void CScriptDebugging::LogBadType(lua_State* luaVM)
{
    // Populate a message to print/send
    LogWarning(luaVM, "Bad argument @ '%s'", lua_tostring(luaVM, lua_upvalueindex(1)));
}

void CScriptDebugging::LogCustom(lua_State* luaVM, const char* szMessage)
{
    assert(szMessage);

    // Populate a message to print/send
    LogWarning(luaVM, "%s", szMessage);
}

void CScriptDebugging::LogString(const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage,
    unsigned int uiDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    SString strText = SString("%s%s", szPrePend, szMessage);

    if (luaDebugInfo.infoType != DEBUG_INFO_NONE && uiDebugLevel <= 2)
        strText = ComposeErrorMessage(szPrePend, luaDebugInfo, szMessage);

    switch (uiDebugLevel)
    {
        case 1:
            ucRed = 255, ucGreen = 0, ucBlue = 0;
            break;
        case 2:
            ucRed = 255, ucGreen = 128, ucBlue = 0;
            break;
        case 3:
            ucRed = 0, ucGreen = 255, ucBlue = 0;
            break;
        default:
            break;
    }

    // Check whether on(Client)DebugMessage is currently being triggered
    bool notCancelled = true;
    if (!m_bTriggeringMessageEvent)
    {
        // Make sure the state of on(Client)DebugMessage being triggered can be retrieved later
        m_bTriggeringMessageEvent = true;

        // Prepare onDebugMessage
        CLuaArguments Arguments;
        Arguments.PushString(szMessage);
        Arguments.PushNumber(uiMinimumDebugLevel);

        // Push the file name (if any)
        if (!luaDebugInfo.strFile.empty())
            Arguments.PushString(luaDebugInfo.strFile);
        else
            Arguments.PushNil();

        // Push the line (if any)
        if (luaDebugInfo.iLine != INVALID_LINE_NUMBER)
            Arguments.PushNumber(luaDebugInfo.iLine);
        else
            Arguments.PushNil();

        // Push the colors
        Arguments.PushNumber(ucRed);
        Arguments.PushNumber(ucGreen);
        Arguments.PushNumber(ucBlue);

        // Call on(Client)DebugMessage
#ifdef MTA_CLIENT
        notCancelled = g_pClientGame->GetRootEntity()->CallEvent("onClientDebugMessage", Arguments, false);
#else
        notCancelled = g_pGame->GetMapManager()->GetRootElement()->CallEvent("onDebugMessage", Arguments);
#endif

        // Reset trigger state, so onDebugMessage can be called again at a later moment
        m_bTriggeringMessageEvent = false;
    }

    if (notCancelled)
        m_DuplicateLineFilter.AddLine({
            strText, uiMinimumDebugLevel, ucRed, ucGreen, ucBlue
        });

#ifdef MTA_CLIENT
    if (g_pCore->GetCVars()->GetValue<bool>("filter_duplicate_log_lines") == false)
#else
    if (g_pGame->GetConfig()->GetFilterDuplicateLogLinesEnabled() == false)
#endif
    {
        m_DuplicateLineFilter.Flush();
    }
    UpdateLogOutput();
}

//
// Get best debug info we possibly can from the relevent lua state
//
const SLuaDebugInfo& CScriptDebugging::GetLuaDebugInfo(lua_State* luaVM)
{
    static SLuaDebugInfo scriptDebugInfo;
    scriptDebugInfo = SLuaDebugInfo();

    // Get a VM from somewhere
    if (!luaVM && !m_LuaMainStack.empty())
        luaVM = m_LuaMainStack.back()->GetVM();

    // Lua oop found at level 4 added one just in case it somehow ends up deeper due to nested calls
    for (int level = 1; level <= 5; level++)
    {
        lua_Debug debugInfo;
        if (luaVM && lua_getstack(luaVM, level, &debugInfo))
        {
            lua_getinfo(luaVM, "nlS", &debugInfo);
            // Lua oop handlers get marked as "C", ignore these as the information we want is further up the stack (typically level 4+)
            if (strcmp(debugInfo.source, "=[C]") != 0)
            {
                // Make sure this function isn't defined in a string (eg: from runcode)
                if (debugInfo.source[0] == '@')
                {
                    // Get and store the location of the debug message
                    scriptDebugInfo.strFile = debugInfo.source + 1;
                    scriptDebugInfo.iLine = debugInfo.currentline;
                    scriptDebugInfo.infoType = DEBUG_INFO_FILE_AND_LINE;
                    // Stop here as we now have the best info
                    break;
                }
                else
                {
                    scriptDebugInfo.strShortSrc = debugInfo.short_src;
                    scriptDebugInfo.infoType = DEBUG_INFO_SHORT_SRC;
                    // Try other levels to see if we can get the calling file
                }
            }
        }
        else
        {
            // Use saved info if set
            if (m_SavedLuaDebugInfo.infoType != DEBUG_INFO_NONE)
            {
                scriptDebugInfo = m_SavedLuaDebugInfo;
            }
            // No point in trying other levels as lua_getstack will fail
            break;
        }
    }

    return scriptDebugInfo;
}

//
// Make full log message from components
//
SString CScriptDebugging::ComposeErrorMessage(const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage)
{
    SString strText;

    // 1st part - if not empty, it should have a space at the end
    strText = szPrePend;

    // 2nd part if set
    if (luaDebugInfo.infoType == DEBUG_INFO_FILE_AND_LINE)
    {
        if (luaDebugInfo.iLine == INVALID_LINE_NUMBER)
            strText += SString("%s: ", *luaDebugInfo.strFile);
        else
            strText += SString("%s:%d: ", *luaDebugInfo.strFile, luaDebugInfo.iLine);
    }

    // 3rd part
    strText += szMessage;

    // 4th part - just kidding.
    if (!luaDebugInfo.strShortSrc.empty())
    {
        if (luaDebugInfo.strShortSrc.BeginsWith("["))
            strText += SString(" %s", *luaDebugInfo.strShortSrc);
        else
            strText += SString(" [%s]", *luaDebugInfo.strShortSrc);
    }

    return strText;
}

// Keep a stack of called VMs to give global warnings/errors a context
void CScriptDebugging::PushLuaMain(CLuaMain* pLuaMain)
{
    m_LuaMainStack.push_back(pLuaMain);
}

void CScriptDebugging::PopLuaMain(CLuaMain* pLuaMain)
{
    dassert(!m_LuaMainStack.empty());
    if (!m_LuaMainStack.empty())
    {
        dassert(m_LuaMainStack.back() == pLuaMain);
        m_LuaMainStack.pop_back();
    }
}

void CScriptDebugging::OnLuaMainDestroy(CLuaMain* pLuaMain)
{
    dassert(!ListContains(m_LuaMainStack, pLuaMain));
    ListRemove(m_LuaMainStack, pLuaMain);
}

CLuaMain* CScriptDebugging::GetTopLuaMain()
{
    if (!m_LuaMainStack.empty())
        return m_LuaMainStack.back();
    return NULL;
}
