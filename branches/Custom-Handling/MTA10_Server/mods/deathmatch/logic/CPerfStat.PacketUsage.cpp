/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.PacketUsage.cpp
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
// CPerfStatPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatPacketUsageImpl : public CPerfStatPacketUsage
{
public:
                                CPerfStatPacketUsageImpl  ( void );
    virtual                     ~CPerfStatPacketUsageImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    SString                         m_strCategoryName;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatPacketUsageImpl* g_pPerfStatPacketUsageImp = NULL;

CPerfStatPacketUsage* CPerfStatPacketUsage::GetSingleton ()
{
    if ( !g_pPerfStatPacketUsageImp )
        g_pPerfStatPacketUsageImp = new CPerfStatPacketUsageImpl ();
    return g_pPerfStatPacketUsageImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPacketUsageImpl::CPerfStatPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatPacketUsageImpl::CPerfStatPacketUsageImpl ( void )
{
    m_strCategoryName = "Packet usage";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPacketUsageImpl::CPerfStatPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatPacketUsageImpl::~CPerfStatPacketUsageImpl ( void )
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatPacketUsageImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatPacketUsageImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPacketUsageImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPacketUsageImpl::DoPulse ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPacketUsageImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPacketUsageImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
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
        pResult->AddColumn ( "Packet usage help" );
        pResult->AddRow ()[0] ="Option h - This help";
        return;
    }

    // Fetch stats
    unsigned long ulTotalBitsIn[256];
    unsigned long ulCountIn[256];
    unsigned long ulTotalBitsOut[256];
    unsigned long ulCountOut[256];
    g_pNetServer->GetNetworkUsageData ( CNetServer::STATS_INCOMING_TRAFFIC, ulTotalBitsIn, ulCountIn );
    g_pNetServer->GetNetworkUsageData ( CNetServer::STATS_OUTGOING_TRAFFIC, ulTotalBitsOut, ulCountOut );

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
