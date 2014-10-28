/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.EventPacketUsage.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

struct SEventUsage
{
    SEventUsage( void ) : iTotal( 0 ), iEventOut( 0 ), iElementDataOut( 0 ), iElementDataRelay( 0 ) {}
    SString strName;
    int iTotal;
    int iEventOut;
    int iElementDataOut;
    int iElementDataRelay;
};

///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatEventPacketUsageImpl : public CPerfStatEventPacketUsage
{
public:
    ZERO_ON_NEW

                                CPerfStatEventPacketUsageImpl  ( void );
    virtual                     ~CPerfStatEventPacketUsageImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatEventPacketUsage
    virtual void                UpdateElementDataUsageOut       ( const char* szName, uint uiNumPlayers, uint uiSize );
    virtual void                UpdateElementDataUsageRelayed   ( const char* szName, uint uiNumPlayers, uint uiSize );
    virtual void                UpdateEventUsageOut             ( const char* szName, uint uiNumPlayers );

    // CPerfStatEventPacketUsageImpl
    void                        MaybeRecordStats        ( void );

    bool                                m_bEnabled;
    CElapsedTime                        m_TimeSinceGetStats;
    long long                           m_llNextRecordTime;
    SString                             m_strCategoryName;
    std::map < SString, SEventUsage >   m_EventUsageLiveMap;
    std::vector < SEventUsage >         m_EventUsageSortedList;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatEventPacketUsageImpl* g_pPerfStatEventPacketUsageImp = NULL;

CPerfStatEventPacketUsage* CPerfStatEventPacketUsage::GetSingleton ()
{
    if ( !g_pPerfStatEventPacketUsageImp )
        g_pPerfStatEventPacketUsageImp = new CPerfStatEventPacketUsageImpl ();
    return g_pPerfStatEventPacketUsageImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::CPerfStatEventPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatEventPacketUsageImpl::CPerfStatEventPacketUsageImpl ( void )
{
    m_strCategoryName = "Event Packet usage";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::CPerfStatEventPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatEventPacketUsageImpl::~CPerfStatEventPacketUsageImpl ( void )
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatEventPacketUsageImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatEventPacketUsageImpl::DoPulse ( void )
{
    MaybeRecordStats();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::UpdateElementDataUsageOut
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatEventPacketUsageImpl::UpdateElementDataUsageOut ( const char* szName, uint uiNumPlayers, uint uiSize )
{
    if ( !m_bEnabled )
        return;

    SEventUsage& usage = MapGet( m_EventUsageLiveMap, szName );
    usage.iTotal += uiNumPlayers;
    usage.iElementDataOut += uiNumPlayers;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::UpdateElementDataUsageRelayed
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatEventPacketUsageImpl::UpdateElementDataUsageRelayed ( const char* szName, uint uiNumPlayers, uint uiSize )
{
    if ( !m_bEnabled )
        return;

    SEventUsage& usage = MapGet( m_EventUsageLiveMap, szName );
    usage.iTotal += uiNumPlayers;
    usage.iElementDataRelay += uiNumPlayers;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::UpdateEventUsageOut
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatEventPacketUsageImpl::UpdateEventUsageOut ( const char* szName, uint uiNumPlayers )
{
    if ( !m_bEnabled )
        return;

    SEventUsage& usage = MapGet( m_EventUsageLiveMap, szName );
    usage.iTotal += uiNumPlayers;
    usage.iEventOut += uiNumPlayers;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::MaybeRecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatEventPacketUsageImpl::MaybeRecordStats ( void )
{
    // Someone watching?
    if ( m_TimeSinceGetStats.Get () < 10000 )
    {
        // Time for record update?    // Copy and clear once every 5 seconds
        long long llTime = GetTickCount64_ ();
        if ( llTime >= m_llNextRecordTime )
        {
            m_llNextRecordTime = Max ( m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9 );

            // Copy into a list and sort
            m_EventUsageSortedList.clear();
            for( std::map < SString, SEventUsage >::iterator iter = m_EventUsageLiveMap.begin() ; iter != m_EventUsageLiveMap.end() ; ++iter )
            {
                iter->second.strName = iter->first;
                m_EventUsageSortedList.push_back( iter->second );
            }
            sort_inline ( m_EventUsageSortedList.begin (), m_EventUsageSortedList.end (), ( const SEventUsage& a, const SEventUsage& b ) { return a.iTotal > b.iTotal; } );

            m_EventUsageLiveMap.clear();
        }
    }
    else
    {
        m_bEnabled = false;
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatEventPacketUsageImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatEventPacketUsageImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    m_TimeSinceGetStats.Reset ();
    m_bEnabled = true;
    MaybeRecordStats();

    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Event Packet usage help" );
        pResult->AddRow ()[0] ="Option h - This help";
        return;
    }

    // Add columns
    pResult->AddColumn ( "Type" );
    pResult->AddColumn ( "Name" );
    pResult->AddColumn ( "msgs/sec" );
    pResult->AddColumn ( "5 sec.msgs" );

    // Fill rows
    for ( uint i = 0 ; i < m_EventUsageSortedList.size() && i < 30 ; i++ )
    {
        const SEventUsage& usage = m_EventUsageSortedList[i];

        // Add row
        SString* row = pResult->AddRow ();

        SString strType;
        if ( usage.iEventOut )
            strType += "Event ";
        if ( usage.iElementDataOut )
            strType += "ElementData ";
        if ( usage.iElementDataRelay )
            strType += "ElementData(Relay) ";

        int c = 0;
        row[c++] = strType;
        row[c++] = usage.strName;
        row[c++] = SString ( "%d", ( usage.iTotal + 4 ) / 5 );
        row[c++] = SString ( "%d", usage.iTotal );
    }
}
