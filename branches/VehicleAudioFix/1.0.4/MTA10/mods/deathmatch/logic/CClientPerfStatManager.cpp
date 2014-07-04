/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClientPerfStatManager.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDynamicLibrary.h"

// microseconds
#ifdef WIN32
TIMEUS GetTimeUs()
{
    static bool bInitialized = false;
    static LARGE_INTEGER lFreq, lStart;
    static LARGE_INTEGER lDivisor;
    if ( !bInitialized )
    {
        bInitialized = true;
        QueryPerformanceFrequency(&lFreq);
        QueryPerformanceCounter(&lStart);
        lDivisor.QuadPart = lFreq.QuadPart / 1000000;
    }

    LARGE_INTEGER lEnd;
    QueryPerformanceCounter(&lEnd);
	double duration = double(lEnd.QuadPart - lStart.QuadPart) / lFreq.QuadPart;
    duration *= 1000000;
    LONGLONG llDuration = static_cast < LONGLONG > ( duration );
    return llDuration & 0xffffffff;
}
#else
#include <sys/time.h>                // for gettimeofday()
using namespace std;
typedef long long LONGLONG;

TIMEUS GetTimeUs()
{
    static bool bInitialized = false;
    static timeval t1;
    if ( !bInitialized )
    {
        bInitialized = true;
        // start timer
        gettimeofday(&t1, NULL);
    }

    // stop timer
    timeval t2;
    gettimeofday(&t2, NULL);

    // compute elapsed time in us
    double elapsedTime;
    elapsedTime =  (t2.tv_sec  - t1.tv_sec) * 1000000.0;    // sec to us
    elapsedTime += (t2.tv_usec - t1.tv_usec);               // us to us

    LONGLONG llDuration = static_cast < LONGLONG > ( elapsedTime );
    return llDuration & 0xffffffff;
}
#endif


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
    typedef std::map < SString, CTimingBlock > CEventTimingMap;
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

    typedef std::map < CLuaMain*, CLuaMainTiming > CLuaMainTimingMap;
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


    //
    // CLuaMainMemory
    //
    class CLuaMainMemory
    {
    public:
        CLuaMainMemory( void  )
        {
            memset ( this, 0, sizeof ( *this ) );
        }

        int Delta;
        int Current;
        int Max;
        int OpenXMLFiles;
        int Refs;
        int TimerCount;
        int ElementCount;
        int TextDisplayCount;
        int TextItemCount;
    };

    typedef std::map < CLuaMain*, CLuaMainMemory > CLuaMainMemoryMap;
    class CAllLuaMemory
    {
    public:
        CLuaMainMemoryMap LuaMainMemoryMap;
    };


    //
    // CLibMemory
    //
    class CLibMemory
    {
    public:
        CLibMemory( void  )
        {
            memset ( this, 0, sizeof ( *this ) );
        }

        int Delta;
        int Current;
        int Max;
    };

    typedef std::map < SString, CLibMemory > CLibMemoryMap;
    class CAllLibMemory
    {
    public:
        CLibMemoryMap LibMemoryMap;
    };


    typedef unsigned long (*PFNGETALLOCSTATS) ( unsigned long*, unsigned long );

    struct CLibraryInfo
    {
        SString strName;
        CDynamicLibrary* pLibrary;
        PFNGETALLOCSTATS pfnGetAllocStats;
    };
}



///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl
//
///////////////////////////////////////////////////////////////
class CClientPerfStatManagerImpl : public CClientPerfStatManager
{
public:
    // CClientPerfStatManager interface
                        CClientPerfStatManagerImpl    ( void );
    virtual             ~CClientPerfStatManagerImpl   ( void );
    virtual void        DoPulse                 ( void );
    virtual void        OnLuaMainCreate         ( CLuaMain* pLuaMain );
    virtual void        OnLuaMainDestroy        ( CLuaMain* pLuaMain );
    virtual void        GetStats                ( CClientPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter );
    virtual void        UpdateLuaMemory         ( CLuaMain* pLuaMain, int iMemUsed );
    virtual void        UpdateLuaTiming         ( CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs );
    virtual void        UpdateLibMemory         ( const SString& strLibName, int iMemUsed, int iMemUsedMax );

    // CClientPerfStatManagerImpl functions
    void                GetLuaMemoryStats       ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter );
    void                GetLuaTimingStats       ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter );
    void                GetLibMemoryStats       ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter );
    void                OutputTimingBlock       ( CClientPerfStatResult* pResult, const CTimingBlock& TimingBlock, int flags, const SString& BlockName, bool bSubBlock );

    CAllLuaTiming                   AllLuaTiming;
    CAllLuaMemory                   AllLuaMemory;
    CAllLibMemory                   AllLibMemory;
    long long                       m_LastTickCount;
    unsigned long                   m_SecondCounter;
    std::map < CLuaMain*, int >     m_LuaMainMap;
    std::vector < CLibraryInfo >    m_LibraryList;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CClientPerfStatManagerImpl* g_pClientPerfStatManagerImp = NULL;

CClientPerfStatManager* GetClientPerfStatManager ()
{
    if ( !g_pClientPerfStatManagerImp )
        g_pClientPerfStatManagerImp = new CClientPerfStatManagerImpl ();
    return g_pClientPerfStatManagerImp;
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::CClientPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatManagerImpl::CClientPerfStatManagerImpl ( void )
{
    m_LastTickCount = 0;
    m_SecondCounter = 0;
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::CClientPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatManagerImpl::~CClientPerfStatManagerImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::OnLuaMainCreate
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::OnLuaMainCreate ( CLuaMain* pLuaMain )
{
    MapSet ( m_LuaMainMap, pLuaMain, 1 );
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::OnLuaMainDestroy
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::OnLuaMainDestroy ( CLuaMain* pLuaMain )
{
    MapRemove ( m_LuaMainMap, pLuaMain );
    MapRemove ( AllLuaTiming.LuaMainTimingMap, pLuaMain );
    MapRemove ( AllLuaMemory.LuaMainMemoryMap, pLuaMain );
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::LuaMemory
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::UpdateLuaMemory ( CLuaMain* pLuaMain, int iMemUsed )
{
    CLuaMainMemory* pLuaMainMemory = MapFind ( AllLuaMemory.LuaMainMemoryMap, pLuaMain );
    if ( !pLuaMainMemory )
    {
        MapSet ( AllLuaMemory.LuaMainMemoryMap, pLuaMain, CLuaMainMemory() );
        pLuaMainMemory = MapFind ( AllLuaMemory.LuaMainMemoryMap, pLuaMain );
    }

    pLuaMainMemory->Delta += iMemUsed - pLuaMainMemory->Current;
    pLuaMainMemory->Current = iMemUsed;
    pLuaMainMemory->Max = Max ( pLuaMainMemory->Max, pLuaMainMemory->Current );
    pLuaMainMemory->OpenXMLFiles = pLuaMain->GetXMLFileCount ();
    pLuaMainMemory->Refs = pLuaMain->m_CallbackTable.size ();
    pLuaMainMemory->TimerCount = pLuaMain->GetTimerCount ();
    pLuaMainMemory->ElementCount = pLuaMain->GetElementCount ();
/*
    pLuaMainMemory->TextDisplayCount = pLuaMain->GetTextDisplayCount ();
    pLuaMainMemory->TextItemCount = pLuaMain->GetTextItemCount ();
*/
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::UpdateLibMemory
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::UpdateLibMemory ( const SString& strLibName, int iMemUsed, int iMemUsedMax )
{
    CLibMemory* pLibMemory = MapFind ( AllLibMemory.LibMemoryMap, strLibName );
    if ( !pLibMemory )
    {
        MapSet ( AllLibMemory.LibMemoryMap, strLibName, CLibMemory() );
        pLibMemory = MapFind ( AllLibMemory.LibMemoryMap, strLibName );
    }

    pLibMemory->Delta += iMemUsed - pLibMemory->Current;
    pLibMemory->Current = iMemUsed;
    pLibMemory->Max = Max ( pLibMemory->Max, iMemUsedMax );
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::UpdateLuaTiming
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::UpdateLuaTiming ( CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs )
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
// CClientPerfStatManagerImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::DoPulse ( void )
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
// CClientPerfStatManagerImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::GetStats ( CClientPerfStatResult* pResult, const SString& strCategory, const SString& strOptions, const SString& strFilter )
{
    pResult->Clear ();

    if ( strCategory == "" )
    {
        pResult->AddColumn ( "Categories" );
        pResult->AddRow ()[0] ="Lua timings";
        pResult->AddRow ()[0] ="Lua memory";
        pResult->AddRow ()[0] ="Lib memory";
        pResult->AddRow ()[0] ="Help";
        return;
    }

    if ( strCategory == "Help" )
    {
        pResult->AddColumn ( "Help" );
        pResult->AddRow ()[0] ="Comma separate multiple options";
        pResult->AddRow ()[0] ="Type h in options and select a category to see help for that category";
        return;
    }

    // Put options in a map
    std::map < SString, int > strOptionMap;
    {
        std::vector < SString > strParts;
        strOptions.Split ( ",", strParts );
        for ( unsigned int i = 0 ; i < strParts.size (); i++ )
            MapSet ( strOptionMap, strParts[i], 1 );
    }

    if ( strCategory == "Lua timings" )
        GetLuaTimingStats ( pResult, strOptionMap, strFilter );
    else
    if ( strCategory == "Lua memory" )
        GetLuaMemoryStats ( pResult, strOptionMap, strFilter );
    else
    if ( strCategory == "Lib memory" )
        GetLibMemoryStats ( pResult, strOptionMap, strFilter );
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::GetLuaMemoryStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::GetLuaMemoryStats ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );
    bool bAcurate = MapContains ( strOptionMap, "a" );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Lua memory help" );
        pResult->AddRow ()[0] ="Option h - This help";
        pResult->AddRow ()[0] ="Option a - More accurate memory usage - Warning: Can slow client a little";
        return;
    }


    // Fetch mem stats from Lua
    {
        for ( std::map < CLuaMain*, int >::iterator iter = m_LuaMainMap.begin () ; iter != m_LuaMainMap.end () ; ++iter )
        {
            CLuaMain* pLuaMain = iter->first;
            if ( pLuaMain->GetVM() )
            {
                if ( bAcurate )
                    lua_gc(pLuaMain->GetVM(), LUA_GCCOLLECT, 0);

                int iMemUsed = lua_getgccount( pLuaMain->GetVM() );
                UpdateLuaMemory ( pLuaMain, iMemUsed );
            }
        }
    }

    pResult->AddColumn ( "name" );
    pResult->AddColumn ( "change" );
    pResult->AddColumn ( "current" );
    pResult->AddColumn ( "max" );
    pResult->AddColumn ( "XMLFiles" );
    pResult->AddColumn ( "refs" );
    pResult->AddColumn ( "Timers" );
    pResult->AddColumn ( "Elements" );
    pResult->AddColumn ( "TextDisplays" );
    pResult->AddColumn ( "TextItems" );

    // Calc totals
    if ( strFilter == "" )
    {
        int calcedCurrent = 0;
        int calcedDelta = 0;
        int calcedMax = 0;
        for ( CLuaMainMemoryMap::iterator iter = AllLuaMemory.LuaMainMemoryMap.begin () ; iter != AllLuaMemory.LuaMainMemoryMap.end () ; ++iter )
        {
            CLuaMainMemory& LuaMainMemory = iter->second;
            calcedCurrent += LuaMainMemory.Current;
            calcedDelta += LuaMainMemory.Delta;
            calcedMax += LuaMainMemory.Max;
        }

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = "Lua VM totals";

        if ( labs(calcedDelta) >= 1 )
        {
            row[c] = SString ( "%d KB", calcedDelta );
            calcedDelta = 0;
        }
        c++;

        row[c++] = SString ( "%d KB", calcedCurrent );
        row[c++] = SString ( "%d KB", calcedMax );
    }

    // For each VM
    for ( CLuaMainMemoryMap::iterator iter = AllLuaMemory.LuaMainMemoryMap.begin () ; iter != AllLuaMemory.LuaMainMemoryMap.end () ; ++iter )
    {
        CLuaMainMemory& LuaMainMemory = iter->second;
        SString resname = iter->first->GetScriptNamePointer ();

        // Apply filter
        if ( strFilter != "" && resname.find ( strFilter ) == SString::npos )
            continue;

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = resname;

        if ( labs ( LuaMainMemory.Delta ) >= 1 )
        {
            row[c] = SString ( "%d KB", LuaMainMemory.Delta );
            LuaMainMemory.Delta = 0;
        }
        c++;

        row[c++] = SString ( "%d KB", LuaMainMemory.Current );
        row[c++] = SString ( "%d KB", LuaMainMemory.Max );
        row[c++] = !LuaMainMemory.OpenXMLFiles ? "-" : SString ( "%d", LuaMainMemory.OpenXMLFiles );
        row[c++] = !LuaMainMemory.Refs ? "-" : SString ( "%d", LuaMainMemory.Refs );
        row[c++] = !LuaMainMemory.TimerCount ? "-" : SString ( "%d", LuaMainMemory.TimerCount );
        row[c++] = !LuaMainMemory.ElementCount ? "-" : SString ( "%d", LuaMainMemory.ElementCount );
        row[c++] = !LuaMainMemory.TextDisplayCount ? "-" : SString ( "%d", LuaMainMemory.TextDisplayCount );
        row[c++] = !LuaMainMemory.TextItemCount ? "-" : SString ( "%d", LuaMainMemory.TextItemCount );
    }
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::GetLuaTimingStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::GetLuaTimingStats ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
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
        pResult->AddColumn ( "cLua timings help" );
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
    //const char* PartNames[] = { " 5s", " 60s", " 300s", " 3600s" };
    SString PartNames[] = { "5s.", "60s.", "300s.", "3600s." };

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
        const SString& resname = iter->first->GetScriptNamePointer ();

        // Apply filter
        if ( strFilter != "" && resname.find ( strFilter ) == SString::npos )
            continue;

        OutputTimingBlock ( pResult, LuaMainTiming.ResourceTiming, flags, resname, false );

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
// CClientPerfStatManagerImpl::OutputTimingBlock
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::OutputTimingBlock ( CClientPerfStatResult* pResult, const CTimingBlock& TimingBlock, int flags, const SString& BlockName, bool bSubBlock )
{
    const CTimingPair*  pairList[]      = { &TimingBlock.s5,  &TimingBlock.s60,   &TimingBlock.m5,    &TimingBlock.m60 };
    const TIMEUS        threshList[]    = { 5,                60,                 300,                3600 };

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


///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::GetLibMemoryStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::GetLibMemoryStats ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );
    bool bMoreInfo = MapContains ( strOptionMap, "i" );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Lib memory help" );
        pResult->AddRow ()[0] ="Option h - This help";
        pResult->AddRow ()[0] ="Option i - More information";
        return;
    }


    // Fetch mem stats from dlls
    #if 0
    {
        if ( m_LibraryList.size () == 0 )
        {
            struct {
                bool bModDir;
                const char* szName;
            } libs [] = {
                            { false,  "core", },
                            { true,   "client", },
                            { false,  "game_sa", },
                            { false,  "multiplayer_sa", },
                            { false,  "netc", },
                            { false,  "xmll", },
                        };

            for ( unsigned int i = 0 ; i < NUMELMS ( libs ) ; i++ )
            {
                CLibraryInfo info;
                bool bModDir = libs[i].bModDir;
                info.strName = libs[i].szName;
                #if MTA_DEBUG
                    info.strName += "_d";
                #endif
                #ifdef WIN32
                    info.strName += ".dll";
                #else
                    info.strName += ".so";
                #endif
                info.pLibrary = new CDynamicLibrary();

                SString strPathFilename;
                char szBuffer [MAX_PATH];
                if ( bModDir )
                    strPathFilename = g_pCoreInterface->GetModManager ()->GetAbsolutePath ( info.strName );
                else
                    strPathFilename = g_pCoreInterface->GetAbsolutePath ( info.strName, szBuffer, MAX_PATH );

                if ( info.pLibrary->Load ( strPathFilename ) )
                {
                    info.pfnGetAllocStats = reinterpret_cast< PFNGETALLOCSTATS > ( info.pLibrary->GetProcedureAddress ( "GetAllocStats" ) );
                    if ( info.pfnGetAllocStats )
                    {
                        m_LibraryList.push_back ( info );
                        continue;
                    }
                }
                delete info.pLibrary;
            }
        }

        for ( unsigned int i = 0 ; i < m_LibraryList.size () ; i++ )
        {
            CLibraryInfo& info = m_LibraryList[i];
            unsigned long stats[9];
            unsigned long numgot = info.pfnGetAllocStats ( stats, NUMELMS ( stats ) );
            if ( numgot >= 2 )
                UpdateLibMemory ( info.strName, ( stats[0] + 1023 ) / 1024, ( stats[1] + 1023 ) / 1024 );
        }
    }
    #endif

    pResult->AddColumn ( "name" );
    pResult->AddColumn ( "change" );
    pResult->AddColumn ( "current" );
    pResult->AddColumn ( "max" );

    if ( bMoreInfo )
    {
        pResult->AddColumn ( "ActiveAllocs" );
        pResult->AddColumn ( "DupeAllocs" );
        pResult->AddColumn ( "UniqueAllocs" );
        pResult->AddColumn ( "ReAllocs" );
        pResult->AddColumn ( "Frees" );
        pResult->AddColumn ( "UnmatchedFrees" );
        pResult->AddColumn ( "DupeMem" );
    }

    // Calc totals
    if ( strFilter == "" )
    {
        int calcedCurrent = 0;
        int calcedDelta = 0;
        int calcedMax = 0;
        for ( CLibMemoryMap::iterator iter = AllLibMemory.LibMemoryMap.begin () ; iter != AllLibMemory.LibMemoryMap.end () ; ++iter )
        {
            CLibMemory& LibMemory = iter->second;
            calcedCurrent += LibMemory.Current;
            calcedDelta += LibMemory.Delta;
            calcedMax += LibMemory.Max;
        }

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = "Lib totals";

        if ( labs(calcedDelta) >= 1 )
        {
            row[c] = SString ( "%d KB", calcedDelta );
            calcedDelta = 0;
        }
        c++;

        row[c++] = SString ( "%d KB", calcedCurrent );
        row[c++] = SString ( "%d KB", calcedMax );
    }


    // For each lib
    for ( CLibMemoryMap::iterator iter = AllLibMemory.LibMemoryMap.begin () ; iter != AllLibMemory.LibMemoryMap.end () ; ++iter )
    {
        CLibMemory& LibMemory = iter->second;
        const SString& strName = iter->first;

        // Apply filter
        if ( strFilter != "" && strName.find ( strFilter ) == SString::npos )
            continue;

        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = strName;

        if ( labs(LibMemory.Delta) >= 1 )
        {
            row[c] = SString ( "%d KB", LibMemory.Delta );
            LibMemory.Delta = 0;
        }
        c++;

        row[c++] = SString ( "%d KB", LibMemory.Current );
        row[c++] = SString ( "%d KB", LibMemory.Max );

        if ( bMoreInfo )
        {
            for ( unsigned int i = 0 ; i < m_LibraryList.size () ; i++ )
            {
                CLibraryInfo& info = m_LibraryList[i];
                if ( strName == info.strName )
                {
                    unsigned long stats[9];
                    unsigned long numgot = info.pfnGetAllocStats ( stats, NUMELMS(stats) );
                    if ( numgot >= 9 )
                    {
                        row[c++] = SString ( "%d", stats[2] );
                        row[c++] = SString ( "%d", stats[3]  );
                        row[c++] = SString ( "%d", stats[4]  );
                        row[c++] = SString ( "%d", stats[5]  );
                        row[c++] = SString ( "%d", stats[6]  );
                        row[c++] = SString ( "%d", stats[7]  );
                        row[c++] = SString ( "%d KB", ( stats[8] + 1023 ) / 1024  );
                    }
                    break;
                }
            }
        }
    }
}
