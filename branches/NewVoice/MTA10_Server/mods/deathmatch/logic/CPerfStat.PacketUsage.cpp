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


DECLARE_ENUM( ePacketID );
IMPLEMENT_ENUM_BEGIN( ePacketID )
    ADD_ENUM1( PACKET_ID_SERVER_JOINEDGAME )
    ADD_ENUM1( PACKET_ID_SERVER_DISCONNECTED )
    ADD_ENUM1( PACKET_ID_RPC )
    ADD_ENUM1( PACKET_ID_PLAYER_LIST )
    ADD_ENUM1( PACKET_ID_PLAYER_KEYSYNC )
    ADD_ENUM1( PACKET_ID_PLAYER_PURESYNC )
    ADD_ENUM1( PACKET_ID_PLAYER_VEHICLE_PURESYNC )
    ADD_ENUM1( PACKET_ID_RETURN_SYNC )
    ADD_ENUM1( PACKET_ID_PLAYER_SPAWN )
    ADD_ENUM1( PACKET_ID_PLAYER_WASTED )
    ADD_ENUM1( PACKET_ID_PLAYER_CHANGE_NICK )
    ADD_ENUM1( PACKET_ID_PLAYER_STATS )
    ADD_ENUM1( PACKET_ID_PLAYER_CLOTHES )
    ADD_ENUM1( PACKET_ID_PLAYER_KEYSYNC )
    ADD_ENUM1( PACKET_ID_PLAYER_PURESYNC )
    ADD_ENUM1( PACKET_ID_PLAYER_VEHICLE_PURESYNC )
    ADD_ENUM1( PACKET_ID_RETURN_SYNC )
    ADD_ENUM1( PACKET_ID_EXPLOSION )
    ADD_ENUM1( PACKET_ID_FIRE )
    ADD_ENUM1( PACKET_ID_PROJECTILE )
    ADD_ENUM1( PACKET_ID_DETONATE_SATCHELS )
    ADD_ENUM1( PACKET_ID_COMMAND )
    ADD_ENUM1( PACKET_ID_CHAT_ECHO )
    ADD_ENUM1( PACKET_ID_CONSOLE_ECHO )
    ADD_ENUM1( PACKET_ID_DEBUG_ECHO )
    ADD_ENUM1( PACKET_ID_MAP_INFO )
    ADD_ENUM1( PACKET_ID_MAP_START )
    ADD_ENUM1( PACKET_ID_MAP_RESTART )
    ADD_ENUM1( PACKET_ID_MAP_STOP )
    ADD_ENUM1( PACKET_ID_ENTITY_ADD )
    ADD_ENUM1( PACKET_ID_ENTITY_REMOVE )
    ADD_ENUM1( PACKET_ID_PICKUP_HIDESHOW )
    ADD_ENUM1( PACKET_ID_PICKUP_HIT_CONFIRM )
    ADD_ENUM1( PACKET_ID_UNOCCUPIED_VEHICLE_STARTSYNC )
    ADD_ENUM1( PACKET_ID_UNOCCUPIED_VEHICLE_STOPSYNC )
    ADD_ENUM1( PACKET_ID_UNOCCUPIED_VEHICLE_SYNC )
    ADD_ENUM1( PACKET_ID_VEHICLE_SPAWN )
    ADD_ENUM1( PACKET_ID_VEHICLE_INOUT )
    ADD_ENUM1( PACKET_ID_VEHICLE_DAMAGE_SYNC )
    ADD_ENUM1( PACKET_ID_VEHICLE_TRAILER )
    ADD_ENUM1( PACKET_ID_PED_STARTSYNC )
    ADD_ENUM1( PACKET_ID_PED_STOPSYNC )
    ADD_ENUM1( PACKET_ID_PED_SYNC )
    ADD_ENUM1( PACKET_ID_PED_WASTED )
    ADD_ENUM1( PACKET_ID_PLAYER_RCON )
    ADD_ENUM1( PACKET_ID_PLAYER_RCON_LOGIN )
    ADD_ENUM1( PACKET_ID_PLAYER_RCON_KICK )
    ADD_ENUM1( PACKET_ID_PLAYER_RCON_BAN )
    ADD_ENUM1( PACKET_ID_PLAYER_RCON_MUTE )
    ADD_ENUM1( PACKET_ID_PLAYER_RCON_FREEZE )
    ADD_ENUM1( PACKET_ID_VOICE_DATA )
    ADD_ENUM1( PACKET_ID_CHEAT_CHALLENGEMEMORY )
    ADD_ENUM1( PACKET_ID_CHEAT_RETURN )
    ADD_ENUM1( PACKET_ID_MAP_LIST )
    ADD_ENUM1( PACKET_ID_LUA )
    ADD_ENUM1( PACKET_ID_TEXT_ITEM )
    ADD_ENUM1( PACKET_ID_SCOREBOARD )
    ADD_ENUM1( PACKET_ID_TEAMS )
    ADD_ENUM1( PACKET_ID_LUA_EVENT )
    ADD_ENUM1( PACKET_ID_RESOURCE_START )
    ADD_ENUM1( PACKET_ID_RESOURCE_STOP )
    ADD_ENUM1( PACKET_ID_CUSTOM_DATA )
    ADD_ENUM1( PACKET_ID_CAMERA_SYNC )
    ADD_ENUM1( PACKET_ID_UPDATE_INFO )
    ADD_ENUM1( PACKET_ID_DISCONNECT_MESSAGE )
    ADD_ENUM1( PACKET_ID_PLAYER_TRANSGRESSION )
    ADD_ENUM1( PACKET_ID_PLAYER_DIAGNOSTIC )
IMPLEMENT_ENUM_END( "ePacketID" )


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
    ZERO_ON_NEW

                                CPerfStatPacketUsageImpl  ( void );
    virtual                     ~CPerfStatPacketUsageImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatPacketUsageImpl
    void                        RecordStats             ( void );

    long long                   m_llNextRecordTime;
    SString                     m_strCategoryName;
    SPacketStat                 m_PrevPacketStats [ 2 ] [ 256 ];
    SPacketStat                 m_PacketStats [ 2 ] [ 256 ];
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
    // Copy and clear once every 5 seconds
    long long llTime = GetTickCount64_ ();

    if ( llTime >= m_llNextRecordTime )
    {
        m_llNextRecordTime = Max ( m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9 );
        RecordStats ();
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatPacketUsageImpl::RecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatPacketUsageImpl::RecordStats ( void )
{
    // Save previous sample so we can calc the delta values
    memcpy ( m_PrevPacketStats, m_PacketStats, sizeof ( m_PacketStats ) );
    memcpy ( m_PacketStats, g_pNetServer->GetPacketStats (), sizeof ( m_PacketStats ) );
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

    // Add columns
    pResult->AddColumn ( "Packet type" );
    pResult->AddColumn ( "Incoming.pkt/sec" );
    pResult->AddColumn ( "Incoming.bytes/sec" );
    pResult->AddColumn ( "Incoming.cpu" );
    pResult->AddColumn ( "Outgoing.pkt/sec" );
    pResult->AddColumn ( "Outgoing.bytes/sec" );
    pResult->AddColumn ( "Outgoing.cpu" );

    // Fill rows
    for ( uint i = 0 ; i < 256 ; i++ )
    {
        // Calc incoming delta values
        SPacketStat statInDelta;
        {
            const SPacketStat& statInPrev = m_PrevPacketStats [ CNetServer::STATS_INCOMING_TRAFFIC ] [ i ];
            const SPacketStat& statInNow = m_PacketStats [ CNetServer::STATS_INCOMING_TRAFFIC ] [ i ];
            statInDelta.iCount      = statInNow.iCount - statInPrev.iCount;
            statInDelta.iTotalBytes = statInNow.iTotalBytes - statInPrev.iTotalBytes;
            statInDelta.totalTime   = statInNow.totalTime - statInPrev.totalTime;
        }

        // Calc outgoing delta values
        SPacketStat statOutDelta;
        {
            const SPacketStat& statOutPrev = m_PrevPacketStats [ CNetServer::STATS_OUTGOING_TRAFFIC ] [ i ];
            const SPacketStat& statOutNow = m_PacketStats [ CNetServer::STATS_OUTGOING_TRAFFIC ] [ i ];
            statOutDelta.iCount      = statOutNow.iCount - statOutPrev.iCount;
            statOutDelta.iTotalBytes = statOutNow.iTotalBytes - statOutPrev.iTotalBytes;
            statOutDelta.totalTime   = statOutNow.totalTime - statOutPrev.totalTime;
        }

        if ( !statInDelta.iCount && !statOutDelta.iCount )
            continue;

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        // Turn "PACKET_ID_PED_SYNC" into "64_Ped_sync"
        SString strPacketDesc = EnumToString ( (ePacketID)i ).SplitRight ( "PACKET_ID", NULL, -1 ).ToLower ();
        row[c++] = SString ( "%d", i ) + strPacketDesc.Left ( 2 ).ToUpper () + strPacketDesc.SubStr ( 2 );
        if ( statInDelta.iCount )
        {
            row[c++] = SString ( "%d", statInDelta.iCount / 5 );
            row[c++] = SString ( "%d", statInDelta.iTotalBytes / 5 );
            row[c++] = SString ( "%2.2f%%", statInDelta.totalTime / 50000.f );   // Number of microseconds in sample period ( 5sec * 1000000 ) into percent ( * 100 )
        }
        else
        {
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "-";
        }

        if ( statOutDelta.iCount )
        {
            row[c++] = SString ( "%d", statOutDelta.iCount / 5 );
            row[c++] = SString ( "%d", statOutDelta.iTotalBytes / 5 );
            //row[c++] = SString ( "%2.2f%%", statOutDelta.totalTime / 50000.f );
            row[c++] = "n/a";
        }
        else
        {
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "-";
        }
    }
}
