/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.DebugInfo.cpp
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
        SString     strSection;
        SString     strHead;
        SString     strData;
    };
}

///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatDebugInfoImpl : public CPerfStatDebugInfo
{
public:
    ZERO_ON_NEW

                                CPerfStatDebugInfoImpl  ( void );
    virtual                     ~CPerfStatDebugInfoImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatDebugInfo
    virtual bool                IsActive                ( const char* szSectionName = NULL );
    virtual void                AddLine                 ( const SString& strSection, const SString& strData );

    // CPerfStatDebugInfoImpl
    void                        RecordStats             ( void );

    SString                     m_strCategoryName;
    bool                        m_bActive;
    CTickCount                  m_LastGetStatsTime;
    SString                     m_LastFilter;

    std::list < SLineInfo >     m_LineList;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatDebugInfoImpl* g_pPerfStatDebugInfoImp = NULL;

CPerfStatDebugInfo* CPerfStatDebugInfo::GetSingleton ()
{
    if ( !g_pPerfStatDebugInfoImp )
        g_pPerfStatDebugInfoImp = new CPerfStatDebugInfoImpl ();
    return g_pPerfStatDebugInfoImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl::CPerfStatDebugInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatDebugInfoImpl::CPerfStatDebugInfoImpl ( void )
{
    m_strCategoryName = "Debug info";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl::CPerfStatDebugInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatDebugInfoImpl::~CPerfStatDebugInfoImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatDebugInfoImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatDebugInfoImpl::DoPulse ( void )
{
    // Check if time to auto deactivate
    if ( m_bActive )
    {
        CTickCount timeSinceLastGetStats = CTickCount::Now () - m_LastGetStatsTime;

        if ( timeSinceLastGetStats.ToLongLong () > 10000 )
            m_bActive = false;
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl::IsActive
//
// Return true if debug info is being watched.
// Set szSectionName to check that section only
//
///////////////////////////////////////////////////////////////
bool CPerfStatDebugInfoImpl::IsActive ( const char* szSectionName )
{
    return m_bActive;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl::AddLine
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatDebugInfoImpl::AddLine ( const SString& strSection, const SString& strData )
{
    if ( !IsActive ( strSection ) )
        return;

    SLineInfo info;
    info.strSection = strSection;
    info.strHead = SString ( "%s - %s"
                        , *GetLocalTimeString ( true, true )
                        , *strSection
                        );
    info.strData = strData;
    m_LineList.push_back ( info );

    while ( m_LineList.size () > 50 )
        m_LineList.pop_front ();
}


///////////////////////////////////////////////////////////////
//
// CPerfStatDebugInfoImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatDebugInfoImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    m_bActive = true;
    m_LastGetStatsTime = CTickCount::Now ();


    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );

    m_LastFilter = strFilter;

    //
    // Process help
    //
    if ( bHelp || strFilter.empty () )
    {
        pResult->AddColumn ( "Debug info help" );

        std::set < SString > sectionNames;
        for ( std::list < SLineInfo >::iterator iter = m_LineList.begin () ; iter != m_LineList.end () ; ++iter )
            MapInsert ( sectionNames, (*iter).strSection );

        pResult->AddRow ()[0] = "Option h - This help";
        pResult->AddRow ()[0] = "Filter all - View all data";
        for ( std::set < SString >::iterator iter = sectionNames.begin () ; iter != sectionNames.end () ; ++iter )
        {
            pResult->AddRow ()[0] = SString ( "Filter %s - View only this data set", **iter );
        }
        return;
    }

    // Add columns
    pResult->AddColumn ( "" );
    pResult->AddColumn ( "" );

    for ( std::list < SLineInfo >::iterator iter = m_LineList.begin () ; iter != m_LineList.end () ; ++iter )
    {
        const SLineInfo& info = *iter;

        if ( m_LastFilter.ContainsI ( "all" ) || m_LastFilter.ContainsI ( info.strSection ) )
        {
            SString* row = pResult->AddRow ();
            int c = 0;

            row[c++] = info.strHead;
            row[c++] = info.strData;
        }
    }
}
