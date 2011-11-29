/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Misc.hpp
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*               Cecill Etheredge <ijsf@gmx-topmail.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifdef MTA_DEBUG

namespace SharedUtil
{
    //
    // Add tags here to hide permanently
    //
    void InitDebugTags ( void )
    {
        SetDebugTagHidden ( "Updater" );
        SetDebugTagHidden ( "Shader" );
        SetDebugTagHidden ( "Browser" );
        SetDebugTagHidden ( "RPC" );
        SetDebugTagHidden ( "Bass" );
        SetDebugTagHidden ( "Database" );
        SetDebugTagHidden ( "AsyncLoading" );
        SetDebugTagHidden ( "ClientSound" );
        SetDebugTagHidden ( "Lua" );
        SetDebugTagHidden ( "Sync" );
        SetDebugTagHidden ( "DEBUG EVENT" );
        SetDebugTagHidden ( "ReportLog" );
        SetDebugTagHidden ( "InstanceCount" );

        #ifdef Has_InitDebugTagsLocal
            InitDebugTagsLocal ();
        #endif
    }

    static std::set < SString > ms_debugTagInvisibleMap;
};


//
// Set hiddeness of a debug tag
//
void SharedUtil::SetDebugTagHidden ( const SString& strTag, bool bHidden  )
{
    if ( bHidden )
        MapInsert ( ms_debugTagInvisibleMap, strTag.ToLower () );
    else
        MapRemove ( ms_debugTagInvisibleMap, strTag.ToLower () );
}

//
// Check is a debug tag is hidden
//
bool SharedUtil::IsDebugTagHidden ( const SString& strTag )
{
    static bool bDoneInitTags = false;
    if ( !bDoneInitTags )
    {
        bDoneInitTags = true;
        InitDebugTags ();
    }
    return MapContains ( ms_debugTagInvisibleMap, strTag.ToLower () );
}

//
// Output timestamped line into the debugger
//
void SharedUtil::OutputDebugLine ( const char* szMessage )
{
    if ( szMessage[0] == '[' )
    {
        // Filter test
        const char* szEnd = strchr ( szMessage, ']' );
        if ( szEnd && IsDebugTagHidden ( std::string ( szMessage + 1, szEnd - szMessage - 1 ) ) )
            return;
    }

    SString strMessage = GetLocalTimeString ( false, true ) + " - " + szMessage;
    if ( strMessage.length () > 0 && strMessage[ strMessage.length () - 1 ] != '\n' )
        strMessage += "\n";
#ifdef _WIN32
    OutputDebugString ( strMessage );
#else
    // Other platforms here
#endif
}


namespace SharedUtil
{
    struct SDebugCountInfo
    {
        SDebugCountInfo ( void ) : iCount ( 0 ), iHigh ( 0 ) {}
        int iCount;
        int iHigh;
    };
    static std::map < SString, SDebugCountInfo > ms_DebugCountMap;

    void DebugCreateCount ( const SString& strName )
    {
        SDebugCountInfo& info = MapGet ( ms_DebugCountMap, strName );
        info.iCount++;
        if ( info.iCount > info.iHigh )
        {
            info.iHigh = info.iCount;
            OutputDebugLine ( SString ( "[InstanceCount] New high of %d for %s", info.iCount, *strName ) );
        }
    }

    void DebugDestroyCount ( const SString& strName )
    {
        SDebugCountInfo& info = MapGet ( ms_DebugCountMap, strName );
        info.iCount--;
        if ( info.iCount < 0 )
            OutputDebugLine ( SString ( "[InstanceCount] Count is negative (%d) for %s", info.iCount, *strName ) );
    }
}

#endif  // MTA_DEBUG
