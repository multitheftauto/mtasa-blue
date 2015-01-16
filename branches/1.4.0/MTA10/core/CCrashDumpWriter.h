/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

enum EDumpFileNameParts
{
    DUMP_PART_SIDE,
    DUMP_PART_VERSION,
    DUMP_PART_MODULE,
    DUMP_PART_OFFSET,
    DUMP_PART_CODE,
    DUMP_PART_PATH,
    DUMP_PART_IP,
    DUMP_PART_PORT,
    DUMP_PART_DURATION,
    DUMP_PART_ID,
    DUMP_PART_DATE,
    DUMP_PART_TIME,
};


//
// CCrashDumpWriter
//
// Handle gathering data and writing the .dmp file on fatal crash
//
class CCrashDumpWriter
{
public:
    static long WINAPI  HandleExceptionGlobal   ( _EXCEPTION_POINTERS* pException );
    static void         DumpCoreLog             ( CExceptionInformation* pExceptionInformation );
    static void         DumpMiniDump            ( _EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation );
    static void         RunErrorTool            ( CExceptionInformation* pExceptionInformation );
    static void         AppendToDumpFile        ( const SString& strPathFilename, const CBuffer& dataBuffer, DWORD dwMagicStart, DWORD dwMagicEnd );
    static void         GetPoolInfo             ( CBuffer& buffer );
    static void         GetD3DInfo              ( CBuffer& buffer );
    static void         GetCrashAvertedStats    ( CBuffer& buffer );
    static void         GetLogInfo              ( CBuffer& buffer );
    static void         GetDxInfo               ( CBuffer& buffer );
    static void         GetMemoryInfo           ( CBuffer& buffer );
    static void         GetMiscInfo             ( CBuffer& buffer );
    static void         OnCrashAverted          ( uint uiId );
    static void         OnEnterCrashZone        ( uint uiId );
    static void         LogEvent                ( const char* szType, const char* szContext, const char* szBody );
    static SString      GetCrashAvertedStatsSoFar ( void );
    static void         ReserveMemoryKBForCrashDumpProcessing ( uint uiMemoryKB );
    static void         FreeMemoryForCrashDumpProcessing ( void );
};
