/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.PlayerPacketUsage.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
extern CNetServer* g_pRealNetServer;

namespace
{
    #define TOP_COUNT (3)

    struct CTopValue
    {
        uint uiPktsPerSec;
        uint uiBytesInPerSec;
        uint uiBytesOutPerSec;
        SString strSerial;
        SString strName;

        CTopValue() : uiPktsPerSec( 0 ), uiBytesInPerSec( 0 ), uiBytesOutPerSec( 0 ) {}
    };

    struct CTopSet
    {
        CTopValue data[TOP_COUNT];

        void SetMax( const CTopSet& other )
        {
            if ( data[0].uiPktsPerSec < other.data[0].uiPktsPerSec )
            {
                *this = other;
            }
        }
    };

    struct CTimeSpan
    {
        CTopSet acc;        // Accumulator for current period
        CTopSet prev;       // Result for previous period

        void Pulse( CTimeSpan* above )
        {
            if ( above )
                above->acc.SetMax( prev );
            prev = acc;
            acc = CTopSet ();
        }
    };

    class CTimeSpanBlock
    {
    public:
        CTimeSpan s5;     // 5 second period
        CTimeSpan s60;    // 60
        CTimeSpan m5;     // 300
        CTimeSpan m60;    // 3600

        void Pulse1s( int flags )
        {
            if ( flags & 1 )    s5.Pulse ( &s60 );
            if ( flags & 2 )    s60.Pulse ( &m5 );
            if ( flags & 4 )    m5.Pulse ( &m60 );
            if ( flags & 8 )    m60.Pulse ( NULL );
        }  
    };

    typedef std::map < uchar, CTimeSpanBlock > CPacketUsageMap;
    class CAllPlayerPacketUsage
    {
    public:
        CPacketUsageMap PacketUsageMap;

        void Pulse1s( int flags )
        {
            for ( CPacketUsageMap::iterator iter = PacketUsageMap.begin () ; iter != PacketUsageMap.end () ; ++iter )
            {
                CTimeSpanBlock& PacketUsage = iter->second;
                PacketUsage.Pulse1s( flags );
            }
        }
    };
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatPlayerPacketUsageImpl : public CPerfStatPlayerPacketUsage
{
public:
    ZERO_ON_NEW
                                CPerfStatPlayerPacketUsageImpl  ( void );
    virtual                     ~CPerfStatPlayerPacketUsageImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatPlayerPacketUsageImpl functions
    void                        UpdatePlayerPacketUsage     ( void );
    void                        GetPlayerPacketUsageStats   ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter );
    void                        OutputTimeSpanBlock         ( CPerfStatResult* pResult, const CTimeSpanBlock& TimeSpanBlock, int iTopPos, int flags, const SString& BlockName, bool bDetail );

    SString                         m_strCategoryName;
    CAllPlayerPacketUsage           m_AllPlayerPacketUsage;
    long long                       m_LastTickCount;
    unsigned long                   m_SecondCounter;
    std::map < CLuaMain*, int >     m_LuaMainMap;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatPlayerPacketUsageImpl* g_pPerfStatPlayerPacketUsageImp = NULL;

CPerfStatPlayerPacketUsage* CPerfStatPlayerPacketUsage::GetSingleton ()
{
    if ( !g_pPerfStatPlayerPacketUsageImp )
        g_pPerfStatPlayerPacketUsageImp = new CPerfStatPlayerPacketUsageImpl ();
    return g_pPerfStatPlayerPacketUsageImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::CPerfStatPlayerPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatPlayerPacketUsageImpl::CPerfStatPlayerPacketUsageImpl ( void )
{
    m_strCategoryName = "Player packet usage";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::CPerfStatPlayerPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatPlayerPacketUsageImpl::~CPerfStatPlayerPacketUsageImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatPlayerPacketUsageImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPlayerPacketUsageImpl::DoPulse ( void )
{
    long long llTickCount = GetTickCount64_ ();
    long long llDelta = llTickCount - m_LastTickCount;
    if ( llDelta >= 1000 )
    {
        UpdatePlayerPacketUsage();

        m_LastTickCount = m_LastTickCount + 1000;
        m_LastTickCount = Max ( m_LastTickCount, llTickCount - 1500 );

        int flags = 0;
        m_SecondCounter++;

        if ( m_SecondCounter % 5 == 0 )         // 5 second
            flags |= 1;
        if ( m_SecondCounter % 60 == 0 )        // 60 seconds
            flags |= 2;
        if ( m_SecondCounter % (5*60) == 0 )    // 5 mins
            flags |= 4;
        if ( m_SecondCounter % (60*60) == 0 )   // 60 mins
            flags |= 8;

        m_AllPlayerPacketUsage.Pulse1s ( flags );
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::UpdatePlayerPacketUsage
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPlayerPacketUsageImpl::UpdatePlayerPacketUsage ( void )
{
    // Get stats from net module
    uchar packetIdList[] = { PACKET_ID_COMMAND, PACKET_ID_LUA_EVENT, PACKET_ID_CUSTOM_DATA };
    const uint uiNumPacketIds = NUMELMS( packetIdList );
    const uint uiTopCount = TOP_COUNT;

    SPlayerPacketUsage stats[ uiNumPacketIds * uiTopCount ];
    if ( !g_pRealNetServer->GetPlayerPacketUsageStats( packetIdList, uiNumPacketIds, stats, uiTopCount ) )
        return;

    uint uiPlayerCount = g_pGame->GetPlayerManager()->Count();

    // Process into AllPlayerPacketUsage
    for( uint p = 0 ; p < uiNumPacketIds ; p++ )
    {
        uchar packetId = packetIdList[p];

        CTimeSpanBlock* pPacketUsage = MapFind ( m_AllPlayerPacketUsage.PacketUsageMap, packetId );
        if ( !pPacketUsage )
        {
            MapSet ( m_AllPlayerPacketUsage.PacketUsageMap, packetId, CTimeSpanBlock() );
            pPacketUsage = MapFind ( m_AllPlayerPacketUsage.PacketUsageMap, packetId );
        }

        for( uint t = 0 ; t < uiTopCount ; t++ )
        {
            SPlayerPacketUsage* pStats = &stats[ p * uiTopCount + t ];
            CTopSet& acc = pPacketUsage->s5.acc;

            // Only update if top value is higher than current
            if ( t == 0 )
            {
                if ( pStats->uiPktsPerSec < acc.data[t].uiPktsPerSec )
                    break;
            }

            // Get player name
            SString strSerial;          
            SString strName;          
            if ( CPlayer* pPlayer = g_pGame->GetPlayerManager()->Get( pStats->playerId ) )
            {
                strSerial = pPlayer->GetSerial();
                strName = pPlayer->GetNick();
            }
            else
            if ( pStats->playerId.GetBinaryAddress() )
            {
                strName = LongToDottedIP( pStats->playerId.GetBinaryAddress() );
            }

            // Custom data packets get sent to all other players
            uint uiAmplification = ( packetId == PACKET_ID_CUSTOM_DATA ) ? uiPlayerCount : 0;

            acc.data[t].uiPktsPerSec = pStats->uiPktsPerSec;
            acc.data[t].uiBytesInPerSec = pStats->uiBytesPerSec;
            acc.data[t].uiBytesOutPerSec = pStats->uiBytesPerSec * uiAmplification;
            acc.data[t].strSerial = strSerial;
            acc.data[t].strName = strName;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPlayerPacketUsageImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& optionMap, const SString& strFilter )
{
    GetPlayerPacketUsageStats ( pResult, optionMap, strFilter );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::GetPlayerPacketUsageStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPlayerPacketUsageImpl::GetPlayerPacketUsageStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
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
        pResult->AddColumn ( "Player packet usage help" );
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

    pResult->AddColumn ( "Packet type" );

    for ( int i = 0 ; i < 4 ; i++ )
    {
        if ( flags & ( 1 << i ) )
        {
            pResult->AddColumn ( PartNames[i] + "pkt/s" );
            if ( bDetail )
            {
                pResult->AddColumn ( PartNames[i] + "bytIn/s" );
                pResult->AddColumn ( PartNames[i] + "bytOut/s" );
                pResult->AddColumn ( PartNames[i] + "serial" );
            }
            pResult->AddColumn ( PartNames[i] + "name" );
        }
    }

    //
    // Set rows
    //
    for ( CPacketUsageMap::iterator iter = m_AllPlayerPacketUsage.PacketUsageMap.begin () ; iter != m_AllPlayerPacketUsage.PacketUsageMap.end () ; ++iter )
    {
        uchar packetId = iter->first;
        CTimeSpanBlock& PacketUsage = iter->second;

        SString* row = pResult->AddRow ();
        int c = 0;

        // Turn "PACKET_ID_PED_SYNC" into "64_Ped_sync"
        SString strPacketDesc = EnumToString ( (ePacketID)packetId ).SplitRight ( "PACKET_ID", NULL, -1 ).ToLower ();
        row[c++] = SString ( "%d", packetId ) + strPacketDesc.Left ( 2 ).ToUpper () + strPacketDesc.SubStr ( 2 );

        for ( uint i = 0 ; i < TOP_COUNT ; i++ )
        {
            OutputTimeSpanBlock ( pResult, PacketUsage, i, flags, SString( ".%d", i + 1 ), bDetail );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPlayerPacketUsageImpl::OutputTimeSpanBlock
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPlayerPacketUsageImpl::OutputTimeSpanBlock ( CPerfStatResult* pResult, const CTimeSpanBlock& TimeSpanBlock, int iTopPos, int flags, const SString& BlockName, bool bDetail )
{
    SFixedArray < const CTimeSpan*, 4 >  timeSpanList = { { &TimeSpanBlock.s5, &TimeSpanBlock.s60, &TimeSpanBlock.m5, &TimeSpanBlock.m60 } };

    // Add row
    SString* row = pResult->AddRow ();

    int c = 0;
    row[c++] = BlockName;

    for ( int i = 0 ; i < 4 ; i++ )
    {
        if ( flags & ( 1 << i ) )
        {
            const CTimeSpan* p = timeSpanList[i];
            const CTopValue& value = ( p->prev.data[0].uiPktsPerSec > p->acc.data[0].uiPktsPerSec ) ? p->prev.data[iTopPos] : p->acc.data[iTopPos];

            row[c++] = value.uiPktsPerSec ? SString ( "%d", value.uiPktsPerSec ) : "-";
            if ( bDetail )
            {
                row[c++] = value.uiBytesInPerSec ? SString ( "%d", value.uiBytesInPerSec ) : "-";
                row[c++] = value.uiBytesOutPerSec ? SString ( "%d", value.uiBytesOutPerSec ) : "-";
                row[c++] = value.strSerial;
            }
            row[c++] = value.strName;
       }
    }
}
