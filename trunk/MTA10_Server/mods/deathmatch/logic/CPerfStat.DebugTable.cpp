/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.DebugTable.cpp
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    struct SLineInfo
    {
        std::vector < SString > strCellList;
        CTickCount endTickCount;
        bool bHasEndTime;
    };
}

///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatDebugTableImpl : public CPerfStatDebugTable
{
public:
    ZERO_ON_NEW

                                CPerfStatDebugTableImpl  ( void );
    virtual                     ~CPerfStatDebugTableImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatDebugTable
    virtual void                RemoveLines             ( const SString& strKeyMatch );
    virtual void                UpdateLine              ( const SString& strKey, int iLifeTimeMs, ... );

    SString                             m_strCategoryName;
    CElapsedTime                        m_TimeSinceRemoveOld;
    std::map < SString, SLineInfo >     m_LineMap;
    CCriticalSection                    m_CS;           // Required as some methods are called from the database thread
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatDebugTableImpl* g_pPerfStatDebugTableImp = NULL;

CPerfStatDebugTable* CPerfStatDebugTable::GetSingleton ()
{
    if ( !g_pPerfStatDebugTableImp )
        g_pPerfStatDebugTableImp = new CPerfStatDebugTableImpl ();
    return g_pPerfStatDebugTableImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl::CPerfStatDebugTableImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatDebugTableImpl::CPerfStatDebugTableImpl ( void )
    : m_TimeSinceRemoveOld ( 500, true )
{
    m_strCategoryName = "Debug table";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl::CPerfStatDebugTableImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatDebugTableImpl::~CPerfStatDebugTableImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatDebugTableImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatDebugTableImpl::DoPulse ( void )
{
    // Do remove old once every second
    if ( m_TimeSinceRemoveOld.Get () < 1000 )
        return;
    m_TimeSinceRemoveOld.Reset ();

    LOCK_SCOPE ( m_CS );

    CTickCount nowTickCount = CTickCount::Now ( true );

    // Remove old
    for ( std::map < SString, SLineInfo >::iterator iter = m_LineMap.begin () ; iter != m_LineMap.end () ; )
    {
        SLineInfo& info = iter->second;
        if ( info.bHasEndTime && info.endTickCount < nowTickCount )
            m_LineMap.erase ( iter++ );
        else
            ++iter;
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl::RemoveLines
//
// Remove one or several lines. Use wildcard match for multiple removes
//
///////////////////////////////////////////////////////////////
void CPerfStatDebugTableImpl::RemoveLines ( const SString& strKeyMatch )
{
    LOCK_SCOPE ( m_CS );

    if ( strKeyMatch.Contains ( "*" ) || strKeyMatch.Contains ( "?" ) )
    {
        // Wildcard remove
        for ( std::map < SString, SLineInfo >::iterator iter = m_LineMap.begin () ; iter != m_LineMap.end () ; )
        {
            // Find each row match
            if ( WildcardMatch ( strKeyMatch, iter->first ) )
                m_LineMap.erase ( iter++ );
            else
                ++iter;
        }
    }
    else
    {
        // Standard remove
        MapRemove ( m_LineMap, strKeyMatch );
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl::UpdateLine
//
// Add/update a line by a string key.
//
///////////////////////////////////////////////////////////////
void CPerfStatDebugTableImpl::UpdateLine ( const SString& strKey, int iLifeTimeMs, ... )
{
    LOCK_SCOPE ( m_CS );

    SLineInfo& info = MapGet( m_LineMap, strKey );
    info.strCellList.clear ();

    // Get cells
    va_list vl;
    va_start ( vl, iLifeTimeMs );

    while ( true )
    {
        char* szText = va_arg ( vl, char* );
        if ( !szText )
            break;
        info.strCellList.push_back ( szText );
    }
    va_end ( vl );

    if ( info.strCellList.empty () )
        info.strCellList.push_back ( "" );

    // Update end time
    info.bHasEndTime = ( iLifeTimeMs > 0 );
    if ( info.bHasEndTime )
        info.endTickCount = CTickCount::Now ( true ) + CTickCount ( (long long)iLifeTimeMs );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugTableImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatDebugTableImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    LOCK_SCOPE ( m_CS );

    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Debug table help" );
        pResult->AddRow ()[0] = "Option h - This help";
        return;
    }

    // Add columns
    const uint uiNumColumns = 4;
    for ( uint i = 0 ; i < uiNumColumns ; i++ )
        pResult->AddColumn ( "" );

    for ( std::map < SString, SLineInfo >::iterator iter = m_LineMap.begin () ; iter != m_LineMap.end () ; ++iter )
    {
        const SString& strKey = iter->first;
        const SLineInfo& info = iter->second;

        // Apply filter
        if ( !strFilter.empty () && !info.strCellList[0].ContainsI ( strFilter ) )
            continue;

        SString* row = pResult->AddRow ();
        int c = 0;

        // Add cells
        for ( uint i = 0 ; i < info.strCellList.size () && c < uiNumColumns ; i++ )
            row[c++] = info.strCellList[i];
    }
}
