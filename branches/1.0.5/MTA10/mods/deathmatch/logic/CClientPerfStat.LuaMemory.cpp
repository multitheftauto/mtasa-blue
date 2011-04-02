/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClientPerfStat.LuaMemory.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
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
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
class CClientPerfStatLuaMemoryImpl : public CClientPerfStatLuaMemory
{
public:
                                CClientPerfStatLuaMemoryImpl  ( void );
    virtual                     ~CClientPerfStatLuaMemoryImpl ( void );

    // CClientPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CClientPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CClientPerfStatLuaMemory
    virtual void                OnLuaMainCreate         ( CLuaMain* pLuaMain );
    virtual void                OnLuaMainDestroy        ( CLuaMain* pLuaMain );

    // CClientPerfStatLuaMemoryImpl
    void                        GetLuaMemoryStats       ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter );
    void                        UpdateLuaMemory         ( CLuaMain* pLuaMain, int iMemUsed );

    SString                         m_strCategoryName;
    CAllLuaMemory                   AllLuaMemory;
    std::map < CLuaMain*, int >     m_LuaMainMap;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CClientPerfStatLuaMemoryImpl* g_pClientPerfStatLuaMemoryImp = NULL;

CClientPerfStatLuaMemory* CClientPerfStatLuaMemory::GetSingleton ()
{
    if ( !g_pClientPerfStatLuaMemoryImp )
        g_pClientPerfStatLuaMemoryImp = new CClientPerfStatLuaMemoryImpl ();
    return g_pClientPerfStatLuaMemoryImp;
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::CClientPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatLuaMemoryImpl::CClientPerfStatLuaMemoryImpl ( void )
{
    m_strCategoryName = "Lua memory";
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::CClientPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatLuaMemoryImpl::~CClientPerfStatLuaMemoryImpl ( void )
{
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CClientPerfStatLuaMemoryImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::OnLuaMainCreate
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::OnLuaMainCreate ( CLuaMain* pLuaMain )
{
    MapSet ( m_LuaMainMap, pLuaMain, 1 );
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::OnLuaMainDestroy
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::OnLuaMainDestroy ( CLuaMain* pLuaMain )
{
    MapRemove ( m_LuaMainMap, pLuaMain );
    MapRemove ( AllLuaMemory.LuaMainMemoryMap, pLuaMain );
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::LuaMemory
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::UpdateLuaMemory ( CLuaMain* pLuaMain, int iMemUsed )
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
    //pLuaMainMemory->TextDisplayCount = pLuaMain->GetTextDisplayCount ();
    //pLuaMainMemory->TextItemCount = pLuaMain->GetTextItemCount ();
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::DoPulse ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::GetStats ( CClientPerfStatResult* pResult, const std::map < SString, int >& optionMap, const SString& strFilter )
{
    pResult->Clear ();
    GetLuaMemoryStats ( pResult, optionMap, strFilter );
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::GetLuaMemoryStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::GetLuaMemoryStats ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
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
        pResult->AddRow ()[0] ="Option a - More accurate memory usage - Warning: Can slow server a little";
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
