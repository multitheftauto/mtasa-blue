/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClientPerfStat.PacketUsage.cpp
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
// CClientPerfStatPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
class CClientPerfStatPacketUsageImpl : public CClientPerfStatPacketUsage
{
public:
                                CClientPerfStatPacketUsageImpl  ( void );
    virtual                     ~CClientPerfStatPacketUsageImpl ( void );

    // CClientPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CClientPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    SString                         m_strCategoryName;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CClientPerfStatPacketUsageImpl* g_pClientPerfStatPacketUsageImp = NULL;

CClientPerfStatPacketUsage* CClientPerfStatPacketUsage::GetSingleton ()
{
    if ( !g_pClientPerfStatPacketUsageImp )
        g_pClientPerfStatPacketUsageImp = new CClientPerfStatPacketUsageImpl ();
    return g_pClientPerfStatPacketUsageImp;
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatPacketUsageImpl::CClientPerfStatPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatPacketUsageImpl::CClientPerfStatPacketUsageImpl ( void )
{
    m_strCategoryName = "Packet usage";
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatPacketUsageImpl::CClientPerfStatPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatPacketUsageImpl::~CClientPerfStatPacketUsageImpl ( void )
{
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatPacketUsageImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CClientPerfStatPacketUsageImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatPacketUsageImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatPacketUsageImpl::DoPulse ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CClientPerfStatPacketUsageImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatPacketUsageImpl::GetStats ( CClientPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
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
        pResult->AddColumn ( "Packet usage help" );
        pResult->AddRow ()[0] ="Option h - This help";
        return;
    }

    // Fetch stats
    unsigned long ulTotalBitsIn[256];
    unsigned long ulCountIn[256];
    unsigned long ulTotalBitsOut[256];
    unsigned long ulCountOut[256];
    g_pNet->GetNetworkUsageData ( CNet::STATS_INCOMING_TRAFFIC, ulTotalBitsIn, ulCountIn );
    g_pNet->GetNetworkUsageData ( CNet::STATS_OUTGOING_TRAFFIC, ulTotalBitsOut, ulCountOut );

    // Add columns
    pResult->AddColumn ( "Packet type" );
    pResult->AddColumn ( "In bytes" );
    pResult->AddColumn ( "Out bytes" );
    pResult->AddColumn ( "In packets" );
    pResult->AddColumn ( "Out packets" );

    // Fill rows
    for ( uint i = 0 ; i < NUMELMS( ulTotalBitsIn ) ; i++ )
    {
        unsigned long ulBytesIn  = ulTotalBitsIn[i] / 8;
        unsigned long ulIn       = ulCountIn[i];
        unsigned long ulBytesOut = ulTotalBitsOut[i] / 8;
        unsigned long ulOut      = ulCountOut[i];

        if ( !ulIn && !ulOut )
            continue;

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = SString ( "%d", i );
        row[c++] = SString ( "%d", ulBytesIn );
        row[c++] = SString ( "%d", ulBytesOut );
        row[c++] = SString ( "%d", ulIn );
        row[c++] = SString ( "%d", ulOut );
    }
}
