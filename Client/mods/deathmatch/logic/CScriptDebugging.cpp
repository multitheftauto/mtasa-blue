/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CScriptDebugging.cpp
 *  PURPOSE:     Script debugging class
 *
 *****************************************************************************/

#include <StdInc.h>
#include <SharedUtil.DebugScript.h>

FILE* CScriptDebugging::m_pLogFile;

CScriptDebugging::CScriptDebugging(CLuaManager* pLuaManager)
{
    m_pLuaManager = pLuaManager;
    m_LogFileLevels = {};
    m_pLogFile = nullptr;
    m_bTriggeringMessageEvent = false;
    m_flushTimerHandle = nullptr;
}

CScriptDebugging::~CScriptDebugging()
{
    // Flush any pending duplicate loggings
    m_DuplicateLineFilter.Flush();
    UpdateLogOutput();

    // Close the previously loaded file
    if (m_pLogFile)
    {
        fprintf(m_pLogFile, "INFO: Logging to this file ended\n");

        // if we have a flush timer
        if (m_flushTimerHandle != NULL)
        {
            // delete our flush timer
            DeleteTimerQueueTimer(NULL, m_flushTimerHandle, INVALID_HANDLE_VALUE);            // INVALID_HANDLE_VALUE = wait for running callbacks to finish
        }
        fclose(m_pLogFile);
        m_pLogFile = NULL;
    }
}

void CScriptDebugging::LogBadLevel(lua_State* luaVM, unsigned int uiRequiredLevel)
{
    // Populate a message to print/send
    LogWarning(luaVM, "Requires level '%d' @ '%s", uiRequiredLevel, lua_tostring(luaVM, lua_upvalueindex(1)));
}

void CALLBACK TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired)
{
    // Got a logfile?
    if (CScriptDebugging::m_pLogFile != NULL)
    {
        // flush our log file
        fflush((FILE*)CScriptDebugging::m_pLogFile);
    }
}

bool CScriptDebugging::SetLogfile(const char* szFilename, std::set<DebugScriptLevel> debugLevels)
{
    assert(szFilename);

    // Close the previously loaded file
    if (m_pLogFile)
    {
        fprintf(m_pLogFile, "INFO: Logging to this file ended\n");
        // if we have a flush timer
        if (m_flushTimerHandle)
        {
            // delete our flush timer
            // INVALID_HANDLE_VALUE = wait for running callbacks to finish
            DeleteTimerQueueTimer(nullptr, m_flushTimerHandle, INVALID_HANDLE_VALUE);
        }
        fclose(m_pLogFile);
        m_pLogFile = nullptr;
    }

    // Apply log size limit
    std::uint32_t uiMaxSizeKB = 0;
    g_pCore->GetCVars()->Get("max_clientscript_log_kb", uiMaxSizeKB);
    CycleFile(szFilename, uiMaxSizeKB);

    // Try to load the new file
    FILE* pFile = File::Fopen(szFilename, "a+");
    if (!pFile)
        return false;

    // [2014-07-09 14:39:31] WARNING: Bad argument @ 'setElementPosition' [Expected element at argument 1, got nil] [string "return
    // addEventHandler("onClientRender", ro..."] length = 158

    // set our buffer size

    // assumed message length of 158

    // if we pulse at 150FPS (unrealistic but whatever)
    // that's 1 update every 6.66666666ms
    // pulse rate is 50 so 50 / 6.6666666 = 7.5 (close enough)
    // if we are doing 5 error messages a pulse that's 5 * 7.5
    // 5 * 7.5 = 37.5
    // we need room for at least 37.5 messages in this buffer
    // round 37.5 to 38 because we can't have half a message
    // 8 * 256 bytes = 6004B
    // round 6004 up to the nearest divisible by 1024 = 6144
    // we have our buffer size.
    setvbuf(pFile, nullptr, _IOFBF, 6144);

    // Set the new pointer and level and return true
    m_LogFileLevels = debugLevels;
    m_pLogFile = pFile;

    // Create a timer
    ::CreateTimerQueueTimer(&m_flushTimerHandle, nullptr, TimerProc, nullptr,
        50, 50, WT_EXECUTEINTIMERTHREAD);
    return true;
}

void CScriptDebugging::UpdateLogOutput()
{
    SLogLine line;
    while (m_DuplicateLineFilter.PopOutputLine(line))
    {
        for (const auto& level : m_LogFileLevels)
        {
            // Log it to the file if enough level
            if (level == line.uiMinimumDebugLevel)
            {
                PrintLog(line.strText);
                break;
            }
        }
    #ifdef MTA_DEBUG
        if (!g_pCore->IsDebugVisible())
            return;
    #endif
        g_pCore->DebugEchoColor(line.strText, line.ucRed, line.ucGreen, line.ucBlue);
    }
}

void CScriptDebugging::PrintLog(const char* szText)
{
    // Got a logfile?
    if (m_pLogFile)
    {
        // Log it, timestamped
        SString strInput("[%s] %s\n", *GetLocalTimeString(true), szText);
        fwrite(strInput.c_str(), strInput.length(), 1, m_pLogFile);
    }
}
