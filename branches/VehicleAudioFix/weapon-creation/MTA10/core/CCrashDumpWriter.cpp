/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCrashDumpWriter.cpp
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

#define LOG_EVENT_SIZE 200

struct SLogEventInfo
{
    uint uiTickCount;
    SString strType;
    SString strContext;
    SString strBody;
};

struct SCrashAvertedInfo
{
    uint uiTickCount;
    int uiUsageCount;
};

static std::list < SLogEventInfo >              ms_LogEventList;
static std::map < int, SCrashAvertedInfo >      ms_CrashAvertedMap;
static uint                                     ms_uiTickCountBase = 0;


typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                    );


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::OnCrashAverted
//
// Static function. Called everytime a crash is averted
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::OnCrashAverted ( uint uiId )
{
    SCrashAvertedInfo* pInfo = MapFind ( ms_CrashAvertedMap, uiId );
    if ( !pInfo )
    {
        MapSet ( ms_CrashAvertedMap, uiId, SCrashAvertedInfo () );
        pInfo = MapFind ( ms_CrashAvertedMap, uiId );
        pInfo->uiUsageCount = 0;
    }
    pInfo->uiTickCount = GetTickCount32 ();
    pInfo->uiUsageCount++;
}


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::LogEvent
//
// Static function.
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::LogEvent ( const char* szType, const char* szContext, const char* szBody )
{
    SLogEventInfo info;
    info.uiTickCount = GetTickCount32 ();
    info.strType = szType;
    info.strContext = szContext;
    info.strBody = szBody;
    ms_LogEventList.push_front ( info );

    while ( ms_LogEventList.size () > LOG_EVENT_SIZE )
        ms_LogEventList.pop_back ();
}


long WINAPI CCrashDumpWriter::HandleExceptionGlobal ( _EXCEPTION_POINTERS* pException )
{
    // Create the exception information class
    CExceptionInformation_Impl* pExceptionInformation = new CExceptionInformation_Impl;
    pExceptionInformation->Set ( pException->ExceptionRecord->ExceptionCode, pException );

    // Grab the mod manager
    CModManager* pModManager = CModManager::GetSingletonPtr ();
    if ( pModManager )
    {
        // Got a client?
        if ( pModManager->GetCurrentMod () )
        {
            // Protect us from "double-faults"
            try
            {
                // Let the client handle it. If it could, continue the execution
                if ( pModManager->GetCurrentMod ()->HandleException ( pExceptionInformation ) )
                {
                    // Delete the exception record and continue to search the exception stack
                    delete pExceptionInformation;
                    return EXCEPTION_CONTINUE_SEARCH;
                }

                // Save tick count now
                ms_uiTickCountBase = GetTickCount32 ();

                // The client wants us to terminate the process
                DumpCoreLog ( pExceptionInformation );
                DumpMiniDump ( pException, pExceptionInformation );
                RunErrorTool ( pExceptionInformation );
                TerminateProcess ( GetCurrentProcess (), 1 );
            }
            catch ( ... )
            {
                // Double-fault, terminate the process
                DumpCoreLog ( pExceptionInformation );
                DumpMiniDump ( pException, pExceptionInformation );
                RunErrorTool ( pExceptionInformation );
                TerminateProcess ( GetCurrentProcess (), 1 );
            }
        }
        else
        {
            // Continue if we're in debug mode, if not terminate
            #ifdef MTA_DEBUG
                return EXCEPTION_CONTINUE_SEARCH;
            #endif
        }
    }

    // Terminate the process
    DumpCoreLog ( pExceptionInformation );
    DumpMiniDump ( pException, pExceptionInformation );
    RunErrorTool ( pExceptionInformation );
    TerminateProcess ( GetCurrentProcess (), 1 );
    return EXCEPTION_CONTINUE_SEARCH;
}


void CCrashDumpWriter::DumpCoreLog ( CExceptionInformation* pExceptionInformation )
{
    // Write a log with the generic exception information
    FILE* pFile = fopen ( CalcMTASAPath ( "mta\\core.log" ), "a+" );
    if ( pFile )
    {
        // Header
        fprintf ( pFile, "%s", "** -- Unhandled exception -- **\n\n" );

        // Write the time
        time_t timeTemp;
        time ( &timeTemp );

        SString strMTAVersionFull = SString ( "%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting ( "mta-version-ext" ).SplitRight ( ".", NULL, -2 ) );

        SString strInfo;
        strInfo += SString ( "Version = %s\n", strMTAVersionFull.c_str () );
        strInfo += SString ( "Time = %s", ctime ( &timeTemp ) );

        strInfo += SString ( "Module = %s\n", pExceptionInformation->GetModulePathName () );

        // Write the basic exception information
        strInfo += SString ( "Code = 0x%08X\n", pExceptionInformation->GetCode () );
        strInfo += SString ( "Offset = 0x%08X\n\n", pExceptionInformation->GetAddressModuleOffset () );

        // Write the register info
        strInfo += SString ( "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n" \
                         "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\n" \
                         "CS=%04X   DS=%04X  SS=%04X  ES=%04X   " \
                         "FS=%04X  GS=%04X\n\n",
                         pExceptionInformation->GetEAX (),
                         pExceptionInformation->GetEBX (),
                         pExceptionInformation->GetECX (),
                         pExceptionInformation->GetEDX (),
                         pExceptionInformation->GetESI (),
                         pExceptionInformation->GetEDI (),
                         pExceptionInformation->GetEBP (),
                         pExceptionInformation->GetESP (),
                         pExceptionInformation->GetEIP (),
                         pExceptionInformation->GetEFlags (),
                         pExceptionInformation->GetCS (),
                         pExceptionInformation->GetDS (),
                         pExceptionInformation->GetSS (),
                         pExceptionInformation->GetES (),
                         pExceptionInformation->GetFS (),
                         pExceptionInformation->GetGS () );


        fprintf ( pFile, "%s", strInfo.c_str () );

        // End of unhandled exception
        fprintf ( pFile, "%s", "** -- End of unhandled exception -- **\n\n\n" );
        
        // Close the file
        fclose ( pFile );

        // For the crash dialog
        SetApplicationSetting ( "diagnostics", "last-crash-info", strInfo );
    }
}


void CCrashDumpWriter::DumpMiniDump ( _EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation )
{
    // Try to load the DLL in our directory
    HMODULE hDll = NULL;
    char szDbgHelpPath [MAX_PATH];
    if ( GetModuleFileName ( NULL, szDbgHelpPath, MAX_PATH ) )
    {
        char* pSlash = _tcsrchr ( szDbgHelpPath, '\\' );
        if ( pSlash )
        {
            _tcscpy ( pSlash + 1, "DBGHELP.DLL" );
            hDll = LoadLibrary ( szDbgHelpPath );
        }
    }

    // If we couldn't load the one in our dir, load any version available
    if ( !hDll )
    {
        hDll = LoadLibrary( "DBGHELP.DLL" );
    }

    // We could load a dll?
    if ( hDll )
    {
        // Grab the MiniDumpWriteDump proc address
        MINIDUMPWRITEDUMP pDump = reinterpret_cast < MINIDUMPWRITEDUMP > ( GetProcAddress( hDll, "MiniDumpWriteDump" ) );
        if ( pDump )
        {
            // Create the file
            HANDLE hFile = CreateFile ( CalcMTASAPath ( "mta\\core.dmp" ), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            if ( hFile != INVALID_HANDLE_VALUE )
            {
                // Create an exception information struct
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
                ExInfo.ThreadId = GetCurrentThreadId ();
                ExInfo.ExceptionPointers = pException;
                ExInfo.ClientPointers = FALSE;

                // Write the dump
                pDump ( GetCurrentProcess(), GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)( MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory ), &ExInfo, NULL, NULL );

                // Close the dumpfile
                CloseHandle ( hFile );

                // Grab the current time
                // Ask windows for the system time.
                SYSTEMTIME SystemTime;
                GetLocalTime ( &SystemTime );

                // Create the dump directory
                CreateDirectory ( CalcMTASAPath ( "mta\\dumps" ), 0 );

                SString strModuleName = pExceptionInformation->GetModuleBaseName ();
                strModuleName = strModuleName.ReplaceI ( ".dll", "" ).Replace ( ".exe", "" ).Replace ( "_", "" ).Replace ( ".", "" ).Replace ( "-", "" );
                if ( strModuleName.length () == 0 )
                    strModuleName = "unknown";

                SString strMTAVersionFull = SString ( "%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting ( "mta-version-ext" ).SplitRight ( ".", NULL, -2 ) );
                SString strSerialPart = GetApplicationSetting ( "serial" ).substr ( 0, 5 );
                uint uiServerIP = GetApplicationSettingInt ( "last-server-ip" );
                uint uiServerPort = GetApplicationSettingInt ( "last-server-port" );
                int uiServerTime = GetApplicationSettingInt ( "last-server-time" );
                int uiServerDuration = _time32 ( NULL ) - uiServerTime;
                uiServerDuration = Clamp ( 0, uiServerDuration + 1, 0xfff );

                // Get path to mta dir
                SString strPathCode;
                {
                    std::vector < SString > parts;
                    PathConform ( CalcMTASAPath ( "" ) ).Split ( PATH_SEPERATOR, parts );
                    for ( uint i = 0 ; i < parts.size () ; i++ )
                    {
                        if ( parts[i].CompareI ( "Program Files" ) )
                            strPathCode += "Pr";
                        else
                        if ( parts[i].CompareI ( "Program Files (x86)" ) )
                            strPathCode += "Px";
                        else
                        if ( parts[i].CompareI ( "MTA San Andreas" ) )
                            strPathCode += "Mt";
                        else
                        if ( parts[i].BeginsWithI ( "MTA San Andreas" ) )
                            strPathCode += "Mb";
                        else
                            strPathCode += parts[i].Left ( 1 ).ToUpper ();
                    }
                }

                // Ensure filename parts match up with EDumpFileNameParts
                SString strFilename ( "mta\\dumps\\client_%s_%s_%08x_%x_%s_%08X_%04X_%03X_%s_%04d%02d%02d_%02d%02d.dmp",
                                             strMTAVersionFull.c_str (),
                                             strModuleName.c_str (),
                                             pExceptionInformation->GetAddressModuleOffset (),
                                             pExceptionInformation->GetCode () & 0xffff,
                                             strPathCode.c_str (),
                                             uiServerIP,
                                             uiServerPort,
                                             uiServerDuration,
                                             strSerialPart.c_str (),
                                             SystemTime.wYear,
                                             SystemTime.wMonth,
                                             SystemTime.wDay,
                                             SystemTime.wHour,
                                             SystemTime.wMinute
                                           );

                SString strPathFilename = CalcMTASAPath ( strFilename );

                // Copy the file
                CopyFile ( CalcMTASAPath ( "mta\\core.dmp" ), strPathFilename, false );

                // For the dump uploader
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "none" );
                SetApplicationSetting ( "diagnostics", "last-dump-save", strPathFilename );

                // Try to append pool sizes info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-pools" );
                CBuffer poolInfo;
                GetPoolInfo ( poolInfo );
                AppendToDumpFile ( strPathFilename, poolInfo, 'POLs', 'POLe' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-pools" );

                // Try to append d3d state info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-d3d" );
                CBuffer d3dInfo;
                GetD3DInfo ( d3dInfo );
                AppendToDumpFile ( strPathFilename, d3dInfo, 'D3Ds', 'D3De' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-d3d" );

                // Try to append crash averted stats to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-crash-averted" );
                CBuffer crashAvertedStats;
                GetCrashAvertedStats ( crashAvertedStats );
                AppendToDumpFile ( strPathFilename, crashAvertedStats, 'CASs', 'CASe' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-crash-averted" );

                // Try to append log info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-log" );
                CBuffer logInfo;
                GetLogInfo ( logInfo );
                AppendToDumpFile ( strPathFilename, logInfo, 'LOGs', 'LOGe' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-log" );

                // Try to append dx info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-misc" );
                CBuffer dxInfo;
                GetDxInfo ( dxInfo );
                AppendToDumpFile ( strPathFilename, dxInfo, 'DXIs', 'DXIe' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-misc" );

                // Try to append misc info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-misc" );
                CBuffer miscInfo;
                GetMiscInfo ( miscInfo );
                AppendToDumpFile ( strPathFilename, miscInfo, 'MSCs', 'MSCe' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-misc" );

                // Try to append memory info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-mem" );
                CBuffer memInfo;
                GetMemoryInfo ( memInfo );
                AppendToDumpFile ( strPathFilename, memInfo, 'MEMs', 'MEMe' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-mem" );
            }
        }

        // Free the DLL again
        FreeLibrary ( hDll );
    }
}


void CCrashDumpWriter::RunErrorTool ( CExceptionInformation* pExceptionInformation )
{
// MTA Error Reporter is now integrated into the launcher

    // Only do once
    static bool bDoneReport = false;
    if ( bDoneReport )
        return;
    bDoneReport = false;

    // Log the basic exception information
    SString strMessage ( "Crash 0x%08X 0x%08X %s"
                         " EAX=%08X EBX=%08X ECX=%08X EDX=%08X ESI=%08X"
                         " EDI=%08X EBP=%08X ESP=%08X EIP=%08X FLG=%08X"
                         " CS=%04X DS=%04X SS=%04X ES=%04X"
                         " FS=%04X GS=%04X",
                         pExceptionInformation->GetCode (),
                         pExceptionInformation->GetAddressModuleOffset (),
                         pExceptionInformation->GetModulePathName (),
                         pExceptionInformation->GetEAX (),
                         pExceptionInformation->GetEBX (),
                         pExceptionInformation->GetECX (),
                         pExceptionInformation->GetEDX (),
                         pExceptionInformation->GetESI (),
                         pExceptionInformation->GetEDI (),
                         pExceptionInformation->GetEBP (),
                         pExceptionInformation->GetESP (),
                         pExceptionInformation->GetEIP (),
                         pExceptionInformation->GetEFlags (),
                         pExceptionInformation->GetCS (),
                         pExceptionInformation->GetDS (),
                         pExceptionInformation->GetSS (),
                         pExceptionInformation->GetES (),
                         pExceptionInformation->GetFS (),
                         pExceptionInformation->GetGS ()
                        );

    AddReportLog ( 3120, strMessage );

    // Try relaunch with crashed flag
    SString strMTASAPath = GetMTASABaseDir ();
    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

#ifdef MTA_DEBUG
    #define MTA_EXE_NAME            "Multi Theft Auto_d.exe"
#else
    #define MTA_EXE_NAME            "Multi Theft Auto.exe"
#endif
    SString strFile = strMTASAPath + "\\" + MTA_EXE_NAME;
    ShellExecute( NULL, "open", strFile, "install_stage=crashed", NULL, SW_SHOWNORMAL );
}



//
// Add extra data to the dump file and hope visual studio doesn't mind
//
void CCrashDumpWriter::AppendToDumpFile ( const SString& strPathFilename, const CBuffer& dataBuffer, DWORD dwMagicStart, DWORD dwMagicEnd )
{
    CBuffer output;
    CBufferWriteStream stream ( output );
    // 4 bytes zero
    stream.Write ( (DWORD)0 );
    // 4 bytes magic
    stream.Write ( dwMagicStart );
    // 4 bytes size of data
    stream.Write ( dataBuffer.GetSize () );
    // n bytes data
    stream.WriteBytes ( dataBuffer.GetData (), dataBuffer.GetSize () );
    // 4 bytes size of data
    stream.Write ( dataBuffer.GetSize () );
    // 4 bytes magic
    stream.Write ( dwMagicEnd );
    // 4 bytes zero
    stream.Write ( (DWORD)0 );
    FileAppend ( strPathFilename, output.GetData (), output.GetSize () );
}


//
// Helper crap for GetPoolInfo
//
namespace
{
    #define CLASS_CBuildingPool                 0xb74498
    #define CLASS_CPedPool                      0xb74490
    #define CLASS_CObjectPool                   0xb7449c
    #define CLASS_CDummyPool                    0xb744a0
    #define CLASS_CVehiclePool                  0xb74494
    #define CLASS_CColModelPool                 0xb744a4
    #define CLASS_CTaskPool                     0xb744a8
    #define CLASS_CEventPool                    0xb744ac
    #define CLASS_CTaskAllocatorPool            0xb744bc
    #define CLASS_CPedIntelligencePool          0xb744c0
    #define CLASS_CPedAttractorPool             0xb744c4
    #define CLASS_CEntryInfoNodePool            0xb7448c
    #define CLASS_CNodeRoutePool                0xb744b8
    #define CLASS_CPatrolRoutePool              0xb744b4
    #define CLASS_CPointRoutePool               0xb744b0
    #define CLASS_CPtrNodeDoubleLinkPool        0xB74488
    #define CLASS_CPtrNodeSingleLinkPool        0xB74484

    #define FUNC_CBuildingPool_GetNoOfUsedSpaces                0x550620
    #define FUNC_CPedPool_GetNoOfUsedSpaces                     0x5504A0
    #define FUNC_CObjectPool_GetNoOfUsedSpaces                  0x54F6B0
    #define FUNC_CDummyPool_GetNoOfUsedSpaces                   0x5507A0
    #define FUNC_CVehiclePool_GetNoOfUsedSpaces                 0x42CCF0
    #define FUNC_CColModelPool_GetNoOfUsedSpaces                0x550870
    #define FUNC_CTaskPool_GetNoOfUsedSpaces                    0x550940
    #define FUNC_CEventPool_GetNoOfUsedSpaces                   0x550A10
    #define FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces           0x550d50
    #define FUNC_CPedIntelligencePool_GetNoOfUsedSpaces         0x550E20
    #define FUNC_CPedAttractorPool_GetNoOfUsedSpaces            0x550ef0
    #define FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces           0x5503d0
    #define FUNC_CNodeRoutePool_GetNoOfUsedSpaces               0x550c80
    #define FUNC_CPatrolRoutePool_GetNoOfUsedSpaces             0x550bb0
    #define FUNC_CPointRoutePool_GetNoOfUsedSpaces              0x550ae0
    #define FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces       0x550230
    #define FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces       0x550300

    int GetPoolCapacity ( ePools pool )
    {
        DWORD iPtr = NULL;
        DWORD cPtr = NULL;
        switch ( pool )
        {
            case BUILDING_POOL:             iPtr = 0x55105F; break;
            case PED_POOL:                  iPtr = 0x550FF2; break;
            case OBJECT_POOL:               iPtr = 0x551097; break;
            case DUMMY_POOL:                iPtr = 0x5510CF; break;
            case VEHICLE_POOL:              cPtr = 0x55102A; break;
            case COL_MODEL_POOL:            iPtr = 0x551107; break;
            case TASK_POOL:                 iPtr = 0x55113F; break;
            case EVENT_POOL:                iPtr = 0x551177; break;
            case TASK_ALLOCATOR_POOL:       cPtr = 0x55124E; break;
            case PED_INTELLIGENCE_POOL:     iPtr = 0x551283; break;
            case PED_ATTRACTOR_POOL:        cPtr = 0x5512BC; break;
            case ENTRY_INFO_NODE_POOL:      iPtr = 0x550FBA; break;
            case NODE_ROUTE_POOL:           cPtr = 0x551219; break;
            case PATROL_ROUTE_POOL:         cPtr = 0x5511E4; break;
            case POINT_ROUTE_POOL:          cPtr = 0x5511AF; break;
            case POINTER_DOUBLE_LINK_POOL:  iPtr = 0x550F82; break;
            case POINTER_SINGLE_LINK_POOL:  iPtr = 0x550F46; break;
        }
        if ( iPtr )
            return *(int*)iPtr;

        if ( cPtr )
            return *(char*)cPtr;

        return 0;
    }

    int GetNumberOfUsedSpaces ( ePools pool )
    {
        DWORD dwFunc = NULL;
        DWORD dwThis = NULL;
        switch ( pool )
        {
            case BUILDING_POOL: dwFunc = FUNC_CBuildingPool_GetNoOfUsedSpaces; dwThis = CLASS_CBuildingPool; break;
            case PED_POOL: dwFunc = FUNC_CPedPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedPool; break;
            case OBJECT_POOL: dwFunc = FUNC_CObjectPool_GetNoOfUsedSpaces; dwThis = CLASS_CObjectPool; break;
            case DUMMY_POOL: dwFunc = FUNC_CDummyPool_GetNoOfUsedSpaces; dwThis = CLASS_CDummyPool; break;
            case VEHICLE_POOL: dwFunc = FUNC_CVehiclePool_GetNoOfUsedSpaces; dwThis = CLASS_CVehiclePool; break;
            case COL_MODEL_POOL: dwFunc = FUNC_CColModelPool_GetNoOfUsedSpaces; dwThis = CLASS_CColModelPool; break;
            case TASK_POOL: dwFunc = FUNC_CTaskPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskPool; break;
            case EVENT_POOL: dwFunc = FUNC_CEventPool_GetNoOfUsedSpaces; dwThis = CLASS_CEventPool; break;
            case TASK_ALLOCATOR_POOL: dwFunc = FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskAllocatorPool; break;
            case PED_INTELLIGENCE_POOL: dwFunc = FUNC_CPedIntelligencePool_GetNoOfUsedSpaces; dwThis = CLASS_CPedIntelligencePool; break;
            case PED_ATTRACTOR_POOL: dwFunc = FUNC_CPedAttractorPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedAttractorPool; break;
            case ENTRY_INFO_NODE_POOL: dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces; dwThis = CLASS_CEntryInfoNodePool; break;
            case NODE_ROUTE_POOL: dwFunc = FUNC_CNodeRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CNodeRoutePool; break;
            case PATROL_ROUTE_POOL: dwFunc = FUNC_CPatrolRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPatrolRoutePool; break;
            case POINT_ROUTE_POOL: dwFunc = FUNC_CPointRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPointRoutePool; break;
            case POINTER_DOUBLE_LINK_POOL: dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeDoubleLinkPool; break;
            case POINTER_SINGLE_LINK_POOL: dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeSingleLinkPool; break;
            default: return -1;
        }

        int iOut = -2;
        if ( *(DWORD *)dwThis != NULL )
        {
            _asm
            {
                mov     ecx, dwThis
                mov     ecx, [ecx]
                call    dwFunc
                mov     iOut, eax

            }
        }

        return iOut;
    }
}


//
// Grab the state of the memory pools
//
void CCrashDumpWriter::GetPoolInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write PoolInfo version
    stream.Write ( 1 );

    // Write number of pools we have info on
    stream.Write ( MAX_POOLS );

    // For each pool
    for ( int i = 0; i < MAX_POOLS ; i++ )
    {
        int iCapacity = GetPoolCapacity ( (ePools)i );
        int iUsedSpaces = GetNumberOfUsedSpaces ( (ePools)i );
        // Write pool info
        stream.Write ( i );
        stream.Write ( iCapacity );
        stream.Write ( iUsedSpaces );
    }

    // TODO - Get usage info for other arrays such as CMatrixLinkList
}


//
// Grab the state of D3D
//
void CCrashDumpWriter::GetD3DInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write D3DInfo version
    stream.Write ( 2 );

    // Quit if device state pointer is not valid
    if ( !g_pDeviceState )
        return;

    // Write D3D call type
    stream.Write ( g_pDeviceState->CallState.callType );

    // Only record state if crash was inside D3D
    if ( g_pDeviceState->CallState.callType == CProxyDirect3DDevice9::SCallState::NONE )
        return;

    // Write D3D call args
    stream.Write ( g_pDeviceState->CallState.uiNumArgs );
    for ( uint i = 0; i < g_pDeviceState->CallState.uiNumArgs ; i++ )
        stream.Write ( g_pDeviceState->CallState.args[i] );

    // Try to get CRenderWare pointer
    CCore* pCore = CCore::GetSingletonPtr ();
    CGame* pGame = pCore ? pCore->GetGame () : NULL;
    CRenderWare* pRenderWare = pGame->GetRenderWare ();
    // Write on how we got on with doing that
    stream.Write ( (uchar)( pCore ? 1 : 0 ) );
    stream.Write ( (uchar)( pGame ? 1 : 0 ) );
    stream.Write ( (uchar)( pRenderWare ? 1 : 0 ) );

    // Write last used texture D3D pointer
    stream.Write ( (uint)g_pDeviceState->TextureState[0].Texture );

    // Write last used texture name
    SString strTextureName = "no name";
    if ( pRenderWare )
        strTextureName = pRenderWare->GetTextureName ( (CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture );
    stream.WriteString ( strTextureName );

    // Write shader name if being used
    stream.WriteString ( (const char*)g_pDeviceState->CallState.strShaderName );
    stream.Write ( g_pDeviceState->CallState.bShaderRequiresNormals );

    // TODO - Vertex state and vertex/index buffer if readable
}


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetCrashAvertedStats
//
// Static function
// Grab the crash averted stats
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetCrashAvertedStats ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write info version
    stream.Write ( 2 );

    // Write number of stats
    stream.Write ( ms_CrashAvertedMap.size () );

    // Write stats
    for ( std::map < int, SCrashAvertedInfo >::iterator iter = ms_CrashAvertedMap.begin () ; iter != ms_CrashAvertedMap.end () ; ++iter )
    {
        stream.Write ( ms_uiTickCountBase - iter->second.uiTickCount );
        stream.Write ( iter->first );
        stream.Write ( iter->second.uiUsageCount );
    }
}


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetLogInfo
//
// Static function
// Grab log info
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetLogInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write info version
    stream.Write ( 1 );

    // Write number of stats
    stream.Write ( ms_LogEventList.size () );

    // Write stats
    for ( std::list < SLogEventInfo >::iterator iter = ms_LogEventList.begin () ; iter != ms_LogEventList.end () ; ++iter )
    {
        stream.Write ( ms_uiTickCountBase - iter->uiTickCount );
        stream.WriteString ( iter->strType );
        stream.WriteString ( iter->strContext );
        stream.WriteString ( iter->strBody );
    }
}


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetDxInfo
//
// Static function
// Grab our dx datum
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetDxInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write info version
    stream.Write ( 2 );

    // video card name etc..
    SDxStatus status;
    CGraphics::GetSingleton ().GetRenderItemManager ()->GetDxStatus ( status );

    stream.Write ( status.testMode );

    stream.WriteString ( status.videoCard.strName );
    stream.Write ( status.videoCard.iInstalledMemoryKB );
    stream.WriteString ( status.videoCard.strPSVersion );

    stream.Write ( status.videoMemoryKB.iFreeForMTA );
    stream.Write ( status.videoMemoryKB.iUsedByFonts );
    stream.Write ( status.videoMemoryKB.iUsedByTextures );
    stream.Write ( status.videoMemoryKB.iUsedByRenderTargets );

    stream.Write ( status.settings.bWindowed );
    stream.Write ( status.settings.iFXQuality );
    stream.Write ( status.settings.iDrawDistance );
    stream.Write ( status.settings.bVolumetricShadows );
    stream.Write ( status.settings.iStreamingMemory );
}


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetMiscInfo
//
// Static function
// Grab various bits 'n' bobs
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetMiscInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write info version
    stream.Write ( 1 );

    // US/Euro gta_sa.exe
    unsigned char ucA = *reinterpret_cast < unsigned char* > ( 0x748ADD );
    unsigned char ucB = *reinterpret_cast < unsigned char* > ( 0x748ADE );
    stream.Write ( ucA );
    stream.Write ( ucB );
}


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetMemoryInfo
//
// Static function
// Same stuff as from showmemstat command
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetMemoryInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write info version
    stream.Write ( 1 );

    SMemStatsInfo memStatsNow;
    GetMemStats ()->SampleState ( memStatsNow );

    // GTA video memory
    static const CProxyDirect3DDevice9::SResourceMemory* const nowList[] = {    &memStatsNow.d3dMemory.StaticVertexBuffer, &memStatsNow.d3dMemory.DynamicVertexBuffer,
                                                                                &memStatsNow.d3dMemory.StaticIndexBuffer, &memStatsNow.d3dMemory.DynamicIndexBuffer,
                                                                                &memStatsNow.d3dMemory.StaticTexture, &memStatsNow.d3dMemory.DynamicTexture };
    int iNumLines = NUMELMS( nowList );
    stream.Write ( iNumLines );
    for ( int i = 0 ; i < iNumLines ; i++ )
    {
        stream.Write ( nowList[i]->iLockedCount );
        stream.Write ( nowList[i]->iCreatedCount );
        stream.Write ( nowList[i]->iDestroyedCount );
        stream.Write ( nowList[i]->iCurrentCount );
        stream.Write ( nowList[i]->iCurrentBytes );
    }

    // Game memory
    stream.Write ( 3 );
    stream.Write ( memStatsNow.iProcessMemSizeKB );
    stream.Write ( memStatsNow.iStreamingMemoryUsed );
    stream.Write ( memStatsNow.iStreamingMemoryAvailable );

    // Model usage
    iNumLines = sizeof ( memStatsNow.modelInfo ) / sizeof ( uint );
    stream.Write ( iNumLines );
    for ( int i = 0 ; i < iNumLines ; i++ )
    {
        stream.Write ( memStatsNow.modelInfo.uiArray[i] );
    }
}


///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetCrashAvertedStatsSoFar
//
// Static function
// Grab the crash averted stats
//
///////////////////////////////////////////////////////////////
SString CCrashDumpWriter::GetCrashAvertedStatsSoFar ()
{
    SString strResult;
    ms_uiTickCountBase = GetTickCount32 ();

    int iIndex = 1;
    for ( std::map < int, SCrashAvertedInfo >::iterator iter = ms_CrashAvertedMap.begin () ; iter != ms_CrashAvertedMap.end () ; ++iter )
    {
        strResult += SString ( "%d) Age:%5d Type:%2d Count:%d\n"
                                            , iIndex++
                                            , ms_uiTickCountBase - iter->second.uiTickCount
                                            , iter->first
                                            , iter->second.uiUsageCount
                                            );
    }
    return strResult;
}
