/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.LuaTiming.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


namespace
{
    struct CTiming
    {
        unsigned long calls;
        TIMEUS total_us;
        TIMEUS max_us;

        CTiming() : calls ( 0 ), total_us ( 0 ), max_us ( 0 ) {}
        CTiming& operator+=( const CTiming& other )
        {
            calls += other.calls;
            total_us += other.total_us;
            max_us = Max ( max_us, other.max_us );
            return *this;
        }
    };

    struct CTimingPair
    {
        CTiming acc;        // Accumulator for current period
        CTiming prev;       // Result for previous period

        void Pulse( CTimingPair* above )
        {
            if ( above )
                above->acc += prev;
            prev = acc;
            acc = CTiming ();
        }
    };

    class CTimingBlock
    {
    public:
        CTimingPair s5;     // 5 second period
        CTimingPair s60;    // 60
        CTimingPair m5;     // 300
        CTimingPair m60;    // 3600

        void Pulse1s( int flags )
        {
            if ( flags & 1 )    s5.Pulse ( &s60 );
            if ( flags & 2 )    s60.Pulse ( &m5 );
            if ( flags & 4 )    m5.Pulse ( &m60 );
            if ( flags & 8 )    m60.Pulse ( NULL );
        }  
    };


    //
    // CLuaMainTiming
    //
    typedef CFastHashMap < SString, CTimingBlock > CEventTimingMap;
    class CLuaMainTiming
    {
    public:
        CEventTimingMap EventTimingMap;
        CTimingBlock ResourceTiming;

        void Pulse1s( int flags )
        {
            ResourceTiming.Pulse1s( flags );
            for ( CEventTimingMap::iterator iter = EventTimingMap.begin () ; iter != EventTimingMap.end () ; ++iter )
            {
                CTimingBlock& EventTiming = iter->second;
                EventTiming.Pulse1s( flags );
            }
        }

    };

    typedef CFastHashMap < CLuaMain*, CLuaMainTiming > CLuaMainTimingMap;
    class CAllLuaTiming
    {
    public:
        CLuaMainTimingMap LuaMainTimingMap;

        void Pulse1s( int flags )
        {
            for ( CLuaMainTimingMap::iterator iter = LuaMainTimingMap.begin () ; iter != LuaMainTimingMap.end () ; ++iter )
            {
                CLuaMainTiming& LuaMainTiming = iter->second;
                LuaMainTiming.Pulse1s( flags );
            }
        }
    };
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatLuaTimingImpl : public CPerfStatLuaTiming
{
public:
    ZERO_ON_NEW
                                CPerfStatLuaTimingImpl  ( void );
    virtual                     ~CPerfStatLuaTimingImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatLuaTiming
    virtual void                OnLuaMainCreate         ( CLuaMain* pLuaMain );
    virtual void                OnLuaMainDestroy        ( CLuaMain* pLuaMain );
    virtual void                UpdateLuaTiming         ( CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs );

    // CPerfStatLuaTimingImpl functions
    void                        GetLuaTimingStats       ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter );
    void                        OutputTimingBlock       ( CPerfStatResult* pResult, const CTimingBlock& TimingBlock, int flags, const SString& BlockName, bool bSubBlock );

    SString                         m_strCategoryName;
    CAllLuaTiming                   AllLuaTiming;
    long long                       m_LastTickCount;
    unsigned long                   m_SecondCounter;
    CFastHashMap < CLuaMain*, int > m_LuaMainMap;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatLuaTimingImpl* g_pPerfStatLuaTimingImp = NULL;

CPerfStatLuaTiming* CPerfStatLuaTiming::GetSingleton ()
{
    if ( !g_pPerfStatLuaTimingImp )
        g_pPerfStatLuaTimingImp = new CPerfStatLuaTimingImpl ();
    return g_pPerfStatLuaTimingImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::CPerfStatLuaTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLuaTimingImpl::CPerfStatLuaTimingImpl ( void )
{
    m_strCategoryName = "Lua timing";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::CPerfStatLuaTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLuaTimingImpl::~CPerfStatLuaTimingImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatLuaTimingImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::OnLuaMainCreate
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::OnLuaMainCreate ( CLuaMain* pLuaMain )
{
    MapSet ( m_LuaMainMap, pLuaMain, 1 );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::OnLuaMainDestroy
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::OnLuaMainDestroy ( CLuaMain* pLuaMain )
{
    MapRemove ( m_LuaMainMap, pLuaMain );
    MapRemove ( AllLuaTiming.LuaMainTimingMap, pLuaMain );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::UpdateLuaTiming
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::UpdateLuaTiming ( CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs )
{
    CLuaMainTiming* pLuaMainTiming = MapFind ( AllLuaTiming.LuaMainTimingMap, pLuaMain );
    if ( !pLuaMainTiming )
    {
        MapSet ( AllLuaTiming.LuaMainTimingMap, pLuaMain, CLuaMainTiming() );
        pLuaMainTiming = MapFind ( AllLuaTiming.LuaMainTimingMap, pLuaMain );
    }

    {
        CTiming& acc = pLuaMainTiming->ResourceTiming.s5.acc;
        acc.total_us += timeUs;
    }

    CTimingBlock* pEventTiming = MapFind ( pLuaMainTiming->EventTimingMap, szEventName );
    if ( !pEventTiming )
    {
        MapSet (pLuaMainTiming->EventTimingMap, szEventName, CTimingBlock() );
        pEventTiming = MapFind ( pLuaMainTiming->EventTimingMap, szEventName );
    }

    {
        CTiming& acc = pEventTiming->s5.acc;
        acc.calls++;
        acc.total_us += timeUs;
        acc.max_us = Max ( acc.max_us, timeUs );
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::DoPulse ( void )
{
    long long llTickCount = GetTickCount64_ ();
    long long llDelta = llTickCount - m_LastTickCount;
    if ( llDelta >= 1000 )
    {
        m_LastTickCount = m_LastTickCount + 1000;
        m_LastTickCount = Max ( m_LastTickCount, llTickCount - 1500 );

        int flags = 0;
        m_SecondCounter++;

        if ( m_SecondCounter % 5 == 0 )         // 1 second
            flags |= 1;
        if ( m_SecondCounter % 60 == 0 )        // 60 seconds
            flags |= 2;
        if ( m_SecondCounter % (60*5) == 0 )    // 5 mins
            flags |= 4;
        if ( m_SecondCounter % (60*60) == 0 )   // 60 mins
            flags |= 8;

        AllLuaTiming.Pulse1s ( flags );
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& optionMap, const SString& strFilter )
{
    GetLuaTimingStats ( pResult, optionMap, strFilter );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetLuaTimingStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::GetLuaTimingStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );
    int flags = 0;
    bool bDetail = false;

    if ( MapContains ( strOptionMap, "5" ) )        flags |= 1 << 0;
    if ( MapContains ( strOptionMap, "60" ) )       flags |= 1 << 1;
    if ( MapContains ( strOptionMap, "300" ) )      flags |= 1 << 2;
    if ( MapContains ( strOptionMap, "3600" ) )     flags |= 1 << 3;
    if ( MapContains ( strOptionMap, "d" ) )        bDetail = true;

    if ( (flags & 15) == 0 )
        flags |= 1 + 2 + 4;

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Lua timings help" );
        pResult->AddRow ()[0] ="Option h - This help";
        pResult->AddRow ()[0] ="Option d - More detail";
        pResult->AddRow ()[0] ="Option 5 - Show 5 sec data";
        pResult->AddRow ()[0] ="Option 60 - Show 1 min data";
        pResult->AddRow ()[0] ="Option 300 - Show 5 min data";
        pResult->AddRow ()[0] ="Option 3600 - Show 1 hr data";
        return;
    }

    //
    // Set column names
    //
    SFixedArray < SString, 4 > PartNames = { { "5s.", "60s.", "300s.", "3600s." } };

    pResult->AddColumn ( "name" );

    for ( int i = 0 ; i < 4 ; i++ )
    {
        if ( flags & ( 1 << i ) )
        {
            pResult->AddColumn ( PartNames[i] + "cpu" );
            pResult->AddColumn ( PartNames[i] + "time" );
            pResult->AddColumn ( PartNames[i] + "calls" );
            pResult->AddColumn ( PartNames[i] + "avg" );
            pResult->AddColumn ( PartNames[i] + "max" );
        }
    }

    //
    // Set rows
    //
    for ( CLuaMainTimingMap::iterator iter = AllLuaTiming.LuaMainTimingMap.begin () ; iter != AllLuaTiming.LuaMainTimingMap.end () ; ++iter )
    {
        CLuaMainTiming& LuaMainTiming = iter->second;
        const SString& strResName = iter->first->GetScriptName ();

        // Apply filter
        if ( strFilter != "" && strResName.find ( strFilter ) == SString::npos )
            continue;

        OutputTimingBlock ( pResult, LuaMainTiming.ResourceTiming, flags, strResName, false );

        if ( bDetail )
        for ( CEventTimingMap::iterator iter = LuaMainTiming.EventTimingMap.begin () ; iter != LuaMainTiming.EventTimingMap.end () ; ++iter )
        {
            const SString& eventname = iter->first;
            const CTimingBlock& TimingBlock = iter->second;
            OutputTimingBlock ( pResult, TimingBlock, flags, std::string ( "." ) + eventname, true  );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::OutputTimingBlock
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::OutputTimingBlock ( CPerfStatResult* pResult, const CTimingBlock& TimingBlock, int flags, const SString& BlockName, bool bSubBlock )
{
    SFixedArray < const CTimingPair*, 4 >  pairList      = { { &TimingBlock.s5,  &TimingBlock.s60,   &TimingBlock.m5,    &TimingBlock.m60 } };
    SFixedArray < const TIMEUS, 4 >        threshList    = { { 5,                60,                 300,                3600 } };

    // See if any relavent data for this row
    bool bHasData = false;
    for ( int i = 0 ; i < 4 ; i++ )
    {
        if ( flags & ( 1 << i ) )
        {
            if ( pairList[i]->prev.total_us > threshList[i] * 1000 )
            {
                bHasData = true;
                break;
            }
        }
    }

    if ( !bHasData )
        return;

    // Add row
    SString* row = pResult->AddRow ();

    int c = 0;
    row[c++] = BlockName;

    for ( int i = 0 ; i < 4 ; i++ )
    {
        if ( flags & ( 1 << i ) )
        {
            const CTimingPair* p = pairList[i];

            double total_s = p->prev.total_us * ( 1/1000000.f );
            double avg_s = p->prev.calls ? p->prev.total_us / p->prev.calls * ( 1/1000000.f ) : 0;
            double max_s = p->prev.max_us * ( 1/1000000.f );

            double total_p = total_s / double ( threshList[i] ) * 100;

            row[c++] = total_p > 0.005 ? SString ( "%2.2f%%", total_p ) : "-";
            row[c++] = total_s > 0.0005 ? SString ( "%2.3f", total_s ) : "-";
            row[c++] = p->prev.calls > 0 ? SString ( "%d", p->prev.calls ) : "";
            row[c++] = avg_s > 0.0005 ? SString ( "%2.3f", avg_s ).c_str () : bSubBlock ? "-" : "";
            row[c++] = max_s > 0.0005 ? SString ( "%2.3f", max_s ).c_str () : bSubBlock ? "-" : "";
       }
    }
}
