/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CFunctionUseLogger.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CFunctionUseLogger.h"

//
// CFunctionUseLogger::CFunctionUseLogger
//
CFunctionUseLogger::CFunctionUseLogger( const SString& strLogFilename )
{
    m_strLogFilename = strLogFilename;
}


//
// CFunctionUseLogger::~CFunctionUseLogger
//
CFunctionUseLogger::~CFunctionUseLogger( void )
{
    MaybeFlush( true );
}


//
// CFunctionUseLogger::Pulse
//
void CFunctionUseLogger::Pulse( void )
{
    if ( !m_FuncCallRecordMap.empty() )
        MaybeFlush();
}


//
// CFunctionUseLogger::MaybeFlush
//
void CFunctionUseLogger::MaybeFlush( bool bForce )
{
    CTickCount timeNow = CTickCount::Now();
    for( std::map < SString, SFuncCallRecord >::iterator iter = m_FuncCallRecordMap.begin() ; iter != m_FuncCallRecordMap.end() ; )
    {
        const SFuncCallRecord& callRecord = iter->second;

        // Time to flush this line?
        if ( bForce || ( timeNow - callRecord.timeFirstUsed ).ToInt() > 1500 )
        {
            // Add log line
            SString strMessage( "%s - %s - %s x %d [%s]\n"
                                    , *GetLocalTimeString( true, true )
                                    , *callRecord.strResourceName
                                    , *callRecord.strFunctionName
                                    , callRecord.uiCallCount
                                    , *callRecord.strExampleArgs
                                );

            if ( !m_strLogFilename.empty() )
                FileAppend( m_strLogFilename, strMessage );

            m_FuncCallRecordMap.erase( iter++ );
        }
        else
            ++iter;
    }
}


//
// CFunctionUseLogger::OnFunctionUse
//
void CFunctionUseLogger::OnFunctionUse( lua_State* luaVM, const char* szFunctionName, const char* szArgs, uint uiArgsSize )
{
    if ( m_strLogFilename.empty() )
        return;

    CResource* pResource = g_pGame->GetResourceManager()->GetResourceFromLuaState( luaVM );
    SString strResourceName = pResource ? pResource->GetName() : "Unknown";

    SString strKey( "%s-%s", szFunctionName, *strResourceName );

    SFuncCallRecord* pItem = MapFind( m_FuncCallRecordMap, strKey );
    if ( !pItem )
    {
        // Create new entry for this resource/function combo
        MapSet( m_FuncCallRecordMap, strKey, SFuncCallRecord() );
        pItem = MapFind( m_FuncCallRecordMap, strKey );
        pItem->strFunctionName = szFunctionName;
        pItem->strResourceName = strResourceName;
        pItem->uiCallCount = 0;
        pItem->timeFirstUsed = CTickCount::Now();
    }
    pItem->uiCallCount++;

    if ( pItem->strExampleArgs.empty() )
        pItem->strExampleArgs = SStringX( szArgs ).Left( 40 );
}
