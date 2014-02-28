/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.FunctionTiming.cpp
*  PURPOSE:     Performance stats
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define DEFAULT_THRESH_MS 1
TIMEUS CPerfStatFunctionTiming::ms_PeakUsThresh = DEFAULT_THRESH_MS * 1000;

namespace
{
    //
    // CPerfStatFunctionTiming helpers
    //
    struct STiming
    {
        STiming( void ) :
             uiNumCalls( 0 )
            ,fTotalMs( 0 )
            ,fPeakMs( 0 )
            ,fResBiggestMs( 0 )
            ,uiTotalBytes( 0 )
            ,uiPeakBytes( 0 )
            ,uiResBiggestBytes( 0 )
        {}

        uint uiNumCalls;
        float fTotalMs;
        float fPeakMs;
        float fResBiggestMs;
        SString strResBiggestMsName;

        uint uiTotalBytes;
        uint uiPeakBytes;
        uint uiResBiggestBytes;
        SString strResBiggestBytesName;
    };

    struct SFunctionTimingInfo
    {
        SFunctionTimingInfo( void ) : iPrevIndex( 0 ) {}
        STiming now5s;
        STiming prev60s;
        int iPrevIndex;
        SFixedArray < STiming, 12 > history;
    };


    //
    // Keep track of a set of values over a period of time
    //
    class CValueHistory
    {
    public:
        std::map < int, CTickCount > historyMap;

        void AddValue ( int iValue )
        {
            MapSet ( historyMap, iValue, CTickCount::Now () );
        }

        void RemoveOlderThan ( int iValue )
        {
            CTickCount now = CTickCount::Now ();
            for ( std::map < int, CTickCount >::iterator iter = historyMap.begin () ; iter != historyMap.end () ; )
            {
                if ( ( now - iter->second ).ToLongLong () > iValue )
                    historyMap.erase ( iter++ );
                else
                    ++iter;
            }
        }

        int GetLowestValue ( int iDefault )
        {
            if ( historyMap.empty () )
                return iDefault;
            return historyMap.begin()->first;
        }
    };

}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatFunctionTimingImpl : public CPerfStatFunctionTiming
{
public:
    ZERO_ON_NEW
                                CPerfStatFunctionTimingImpl  ( void );
    virtual                     ~CPerfStatFunctionTimingImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatFunctionTiming
    virtual void                UpdateTiming            ( const SString& strResourceName, const char* szFunctionName, TIMEUS timeUs, uint uiDeltaBytes );

    // CPerfStatFunctionTimingImpl functions
    void                        SetActive               ( bool bActive );

    SString                                     m_strCategoryName;
    CElapsedTime                                m_TimeSinceLastViewed;
    bool                                        m_bIsActive;
    CValueHistory                               m_PeakUsRequiredHistory;

    CElapsedTime                                m_TimeSinceUpdate;
    std::map < SString, SFunctionTimingInfo >   m_TimingMap;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatFunctionTimingImpl* g_pPerfStatFunctionTimingImp = NULL;

CPerfStatFunctionTiming* CPerfStatFunctionTiming::GetSingleton ()
{
    if ( !g_pPerfStatFunctionTimingImp )
        g_pPerfStatFunctionTimingImp = new CPerfStatFunctionTimingImpl ();
    return g_pPerfStatFunctionTimingImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl::CPerfStatFunctionTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatFunctionTimingImpl::CPerfStatFunctionTimingImpl ( void )
{
    m_strCategoryName = "Function stats";
    ms_PeakUsThresh = DEFAULT_THRESH_MS * 1000;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl::CPerfStatFunctionTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatFunctionTimingImpl::~CPerfStatFunctionTimingImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatFunctionTimingImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatFunctionTimingImpl::DoPulse ( void )
{
    // Maybe turn off stats gathering if nobody is watching
    if ( m_bIsActive && m_TimeSinceLastViewed.Get () > 15000 )
        SetActive ( false );

    // Do nothing if not active
    if ( !m_bIsActive )
    {
        m_TimingMap.clear ();
        return;
    }

    // Check if time to cycle the stats
    if ( m_TimeSinceUpdate.Get () >= 10000 )
    {
        m_TimeSinceUpdate.Reset ();

        // For each timed function
        for ( std::map < SString, SFunctionTimingInfo >::iterator iter = m_TimingMap.begin () ; iter != m_TimingMap.end () ; )
        {
            SFunctionTimingInfo& item = iter->second;
            // Update history
            item.iPrevIndex = ( item.iPrevIndex + 1 ) % NUMELMS( item.history );
            item.history[ item.iPrevIndex ] = item.now5s;

            // Reset accumulator
            item.now5s.uiNumCalls = 0;

            item.now5s.fTotalMs = 0;
            item.now5s.fPeakMs = 0;
            item.now5s.fResBiggestMs = 0;
            item.now5s.strResBiggestMsName.clear();

            item.now5s.uiTotalBytes = 0;
            item.now5s.uiPeakBytes = 0;
            item.now5s.uiResBiggestBytes = 0;
            item.now5s.strResBiggestBytesName.clear();

            // Recalculate last 60 second stats
            item.prev60s.uiNumCalls = 0;

            item.prev60s.fTotalMs = 0;
            item.prev60s.fPeakMs = 0;
            item.prev60s.fResBiggestMs = 0;
            item.prev60s.strResBiggestMsName.clear();

            item.prev60s.uiTotalBytes = 0;
            item.prev60s.uiPeakBytes = 0;
            item.prev60s.uiResBiggestBytes = 0;
            item.prev60s.strResBiggestBytesName.clear();

            for ( uint i = 0 ; i < NUMELMS( item.history ) ; i++ )
            {
                const STiming& slot = item.history[i];
                item.prev60s.uiNumCalls += slot.uiNumCalls;

                item.prev60s.fTotalMs += slot.fTotalMs;
                item.prev60s.fPeakMs = Max ( item.prev60s.fPeakMs, slot.fPeakMs );
                if ( item.prev60s.fResBiggestMs < slot.fTotalMs )
                {
                    item.prev60s.fResBiggestMs = slot.fTotalMs;
                    item.prev60s.strResBiggestMsName = slot.strResBiggestMsName;
                }

                item.prev60s.uiTotalBytes += slot.uiTotalBytes;
                item.prev60s.uiPeakBytes = Max ( item.prev60s.uiPeakBytes, slot.uiPeakBytes );
                if ( item.prev60s.uiResBiggestBytes < slot.uiTotalBytes )
                {
                    item.prev60s.uiResBiggestBytes = slot.uiTotalBytes;
                    item.prev60s.strResBiggestBytesName = slot.strResBiggestBytesName;
                }
            }

            // Remove from map if no calls in the last 60s
            if ( item.prev60s.uiNumCalls == 0 )
                m_TimingMap.erase ( iter++ );
            else
                ++iter;
        }
    }

    //
    // Update PeakUs threshold
    //
    m_PeakUsRequiredHistory.RemoveOlderThan ( 10000 );
    ms_PeakUsThresh = m_PeakUsRequiredHistory.GetLowestValue ( DEFAULT_THRESH_MS * 1000 );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl::SetActive
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatFunctionTimingImpl::SetActive ( bool bActive )
{
    if ( bActive != m_bIsActive )
    {
        m_bIsActive = bActive;
        g_pStats->bFunctionTimingActive = m_bIsActive;
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl::UpdateTiming
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatFunctionTimingImpl::UpdateTiming ( const SString& strResourceName, const char* szFunctionName, TIMEUS timeUs, uint uiDeltaBytes )
{
    if ( !m_bIsActive )
        return;

    // Ignore any single calls under lowest threshold from any viewer
    if ( timeUs < ms_PeakUsThresh )
        return;

    float fTimeMs = timeUs * ( 1 / 1000.f );

    // Record the timing
    SFunctionTimingInfo& item = MapGet ( m_TimingMap, szFunctionName );
    item.now5s.uiNumCalls++;

    item.now5s.fTotalMs += fTimeMs;
    item.now5s.fPeakMs = Max ( item.now5s.fPeakMs, fTimeMs );
    if ( item.now5s.fResBiggestMs < fTimeMs )
    {
        item.now5s.fResBiggestMs = fTimeMs;
        item.now5s.strResBiggestMsName = strResourceName;
    }

    item.now5s.uiTotalBytes += uiDeltaBytes;
    item.now5s.uiPeakBytes = Max ( item.now5s.uiPeakBytes, uiDeltaBytes );
    if ( item.now5s.uiResBiggestBytes < uiDeltaBytes )
    {
        item.now5s.uiResBiggestBytes = uiDeltaBytes;
        item.now5s.strResBiggestBytesName = strResourceName;
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatFunctionTimingImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatFunctionTimingImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& optionMap, const SString& strFilter )
{
    m_TimeSinceLastViewed.Reset ();
    SetActive ( true );

    //
    // Set option flags
    //
    bool bHelp = MapContains ( optionMap, "h" );
    uint uiPeakBytesThresh = 1000;
    int iPeakMsThresh = optionMap.empty () ? -1 : atoi ( optionMap.begin ()->first );
    if ( iPeakMsThresh < 0 )
        iPeakMsThresh = DEFAULT_THRESH_MS;
    m_PeakUsRequiredHistory.AddValue ( iPeakMsThresh * 1000 );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Function timings help" );
        pResult->AddRow ()[0] = "Option h - This help";
        pResult->AddRow ()[0] = "0-50 - Peak Ms threshold (defaults to 1)";
        return;
    }

    //
    // Set column names
    //

    pResult->AddColumn ( " " );
    pResult->AddColumn ( "10 sec.calls" );
    pResult->AddColumn ( "10 sec.cpu total" );
    pResult->AddColumn ( "10 sec.cpu peak" );
    pResult->AddColumn ( "10 sec.cpu biggest call" );

    pResult->AddColumn ( "10 sec.BW" );
    //pResult->AddColumn ( "10 sec.BW peak" );
    pResult->AddColumn ( "10 sec.BW biggest call" );

    pResult->AddColumn ( "120 sec.calls" );
    pResult->AddColumn ( "120 sec.cpu total" );
    pResult->AddColumn ( "120 sec.cpu peak" );
    pResult->AddColumn ( "120 sec.cpu biggest call" );

    pResult->AddColumn ( "120 sec.BW" );
    //pResult->AddColumn ( "120 sec.BW peak" );
    pResult->AddColumn ( "120 sec.BW biggest call" );

    //
    // Set rows
    //

    for ( std::map < SString, SFunctionTimingInfo > :: const_iterator iter = m_TimingMap.begin () ; iter != m_TimingMap.end () ; iter++ )
    {
        const SString& strFunctionName  = iter->first;
        const SFunctionTimingInfo& item = iter->second;

        const STiming& prev5s = item.history[ item.iPrevIndex ];
        const STiming& prev60s = item.prev60s;

        bool bHas5s = prev5s.uiNumCalls > 0;
        bool bHas60s = prev60s.uiNumCalls > 0;

        if ( !bHas5s && !bHas60s )
            continue;

        // Filter peak threshold for this viewer
        if ( prev5s.fPeakMs < iPeakMsThresh && prev60s.fPeakMs < iPeakMsThresh &&
             prev5s.uiPeakBytes < uiPeakBytesThresh && prev60s.uiPeakBytes < uiPeakBytesThresh )
            continue;

        // Apply filter
        if ( strFilter != "" && strFunctionName.find ( strFilter ) == SString::npos )
            continue;

        // Add row
        SString* row = pResult->AddRow ();
        int c = 0;
        row[c++] = strFunctionName;

        if ( !bHas5s )
        {
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "";
            //row[c++] = "";
            row[c++] = "";
        }
        else
        {
            row[c++] = SString ( "%u", prev5s.uiNumCalls );

            row[c++] = SString ( "%2.0f ms", prev5s.fTotalMs );
            row[c++] = SString ( "%2.0f ms", prev5s.fPeakMs );
            row[c++] = SString ( "%2.0f ms (%s)", prev5s.fResBiggestMs, *prev5s.strResBiggestMsName );

            row[c++] = prev5s.uiTotalBytes < 10 ? "" : SString ( "%s", *CPerfStatManager::GetScaledByteString( prev5s.uiTotalBytes ) );
            //row[c++] = prev5s.uiPeakBytes < 10 ? "" : SString ( "%s ", *CPerfStatManager::GetScaledByteString( prev5s.uiPeakBytes ) );
            row[c++] = prev5s.uiResBiggestBytes < 10 ? "" : SString ( "%s (%s)", *CPerfStatManager::GetScaledByteString( prev5s.uiResBiggestBytes ), *prev5s.strResBiggestBytesName );
        }

        row[c++] = SString ( "%u", prev60s.uiNumCalls );

        row[c++] = SString ( "%2.0f ms", prev60s.fTotalMs );
        row[c++] = SString ( "%2.0f ms", prev60s.fPeakMs );
        row[c++] = SString ( "%2.0f ms (%s)", prev60s.fResBiggestMs, *prev60s.strResBiggestMsName );

        row[c++] = prev60s.uiTotalBytes < 10 ? "" : SString ( "%s ", *CPerfStatManager::GetScaledByteString( prev60s.uiTotalBytes ) );
        //row[c++] = prev60s.uiPeakBytes < 10 ? "" : SString ( "%s ", *CPerfStatManager::GetScaledByteString( prev60s.uiPeakBytes ) );
        row[c++] = prev60s.uiResBiggestBytes < 10 ? "" : SString ( "%s (%s)", *CPerfStatManager::GetScaledByteString( prev60s.uiResBiggestBytes ), *prev60s.strResBiggestBytesName );
    }
}
