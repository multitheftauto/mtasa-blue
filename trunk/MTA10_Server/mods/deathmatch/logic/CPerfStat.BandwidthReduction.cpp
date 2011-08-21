/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.BandwidthReduction.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatBandwidthReductionImpl : public CPerfStatBandwidthReduction
{
public:
    ZERO_ON_NEW

                                CPerfStatBandwidthReductionImpl  ( void );
    virtual                     ~CPerfStatBandwidthReductionImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatBandwidthReductionImpl
    void                        RecordStats             ( void );

    long long                   m_llNextRecordTime;
    SString                     m_strCategoryName;

    SStatData                   m_PrevStats;
    SStatData                   m_Stats5Sec;
    SStatData                   m_StatsTotal;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatBandwidthReductionImpl* g_pPerfStatBandwidthReductionImp = NULL;

CPerfStatBandwidthReduction* CPerfStatBandwidthReduction::GetSingleton ()
{
    if ( !g_pPerfStatBandwidthReductionImp )
        g_pPerfStatBandwidthReductionImp = new CPerfStatBandwidthReductionImpl ();
    return g_pPerfStatBandwidthReductionImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::CPerfStatBandwidthReductionImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatBandwidthReductionImpl::CPerfStatBandwidthReductionImpl ( void )
{
    m_strCategoryName = "Bandwidth reduction";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::CPerfStatBandwidthReductionImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatBandwidthReductionImpl::~CPerfStatBandwidthReductionImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatBandwidthReductionImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthReductionImpl::DoPulse ( void )
{
    long long llTime = GetTickCount64_ ();

    // Record once every 5 seconds
    if ( llTime >= m_llNextRecordTime )
    {
        m_llNextRecordTime = Max ( m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9 );
        RecordStats ();
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::RecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthReductionImpl::RecordStats ( void )
{
    for ( uint i = 0 ; i < ZONE_MAX ; i++ )
    {
        m_Stats5Sec.puresync.uiSentPacketsByZone [ i ]    = g_pStats->puresync.uiSentPacketsByZone[i]    - m_PrevStats.puresync.uiSentPacketsByZone[i];
        m_Stats5Sec.puresync.uiSkippedPacketsByZone [ i ] = g_pStats->puresync.uiSkippedPacketsByZone[i] - m_PrevStats.puresync.uiSkippedPacketsByZone[i];
    }
    m_PrevStats = *g_pStats;
    m_StatsTotal = *g_pStats;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthReductionImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Bandwidth reduction help" );
        pResult->AddRow ()[0] ="Option h - This help";
        return;
    }

    // Add columns
    pResult->AddColumn ( "Zone" );
    pResult->AddColumn ( "Last 5 seconds.Pure sync packets sent" );
    pResult->AddColumn ( "Last 5 seconds.Pure sync packets skipped" );
    pResult->AddColumn ( "Since start.Pure sync packets sent" );
    pResult->AddColumn ( "Since start.Pure sync packets skipped" );


    int iTotals[4] = { 0, 0, 0, 0 };
    const char* szDesc[4] = { "Very near, or in FOV", "Near, just out of FOV", "Near, way out of FOV", "Far" };

    for ( uint i = 0 ; i < ZONE_MAX ; i++ )
    {
        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = SString ( "%d - %s", i, szDesc[i] );
        row[c++] = SString ( "%d", m_Stats5Sec.puresync.uiSentPacketsByZone[i] );
        row[c++] = SString ( "%d", m_Stats5Sec.puresync.uiSkippedPacketsByZone[i] );
        row[c++] = SString ( "%d", m_StatsTotal.puresync.uiSentPacketsByZone[i] );
        row[c++] = SString ( "%d", m_StatsTotal.puresync.uiSkippedPacketsByZone[i] );

        iTotals[0] += m_Stats5Sec.puresync.uiSentPacketsByZone[i];
        iTotals[1] += m_Stats5Sec.puresync.uiSkippedPacketsByZone[i];
        iTotals[2] += m_StatsTotal.puresync.uiSentPacketsByZone[i];
        iTotals[3] += m_StatsTotal.puresync.uiSkippedPacketsByZone[i];
    }

    {
        pResult->AddRow ();
        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = "Total";
        row[c++] = SString ( "%d", iTotals[0] );
        row[c++] = SString ( "%d", iTotals[1] );
        row[c++] = SString ( "%d", iTotals[2] );
        row[c++] = SString ( "%d", iTotals[3] );
    }

    {
        SString* row = pResult->AddRow ();

        int iPercent5Sec = ( iTotals[1] ) * 100 / Max ( 1, iTotals[0] + iTotals[1] );
        int iPercentAll  = ( iTotals[3] ) * 100 / Max ( 1, iTotals[2] + iTotals[3] );
        int c = 0;
        row[c++] = "Reduction percent";
        row[c++] = SString ( "%d%%", -iPercent5Sec );
        row[c++] = "";
        row[c++] = SString ( "%d%%", -iPercentAll );
        row[c++] = "";
    }
}
