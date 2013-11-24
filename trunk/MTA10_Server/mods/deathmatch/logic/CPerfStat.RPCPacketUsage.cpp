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


DECLARE_ENUM( eElementRPCFunctions );
IMPLEMENT_ENUM_BEGIN( eElementRPCFunctions )
    ADD_ENUM1( DONT_USE_0 )
    ADD_ENUM1( SET_TIME )
    ADD_ENUM1( SET_WEATHER )
    ADD_ENUM1( SET_WEATHER_BLENDED )
    ADD_ENUM1( SET_MINUTE_DURATION )
    ADD_ENUM1( SET_ELEMENT_PARENT )
    ADD_ENUM1( SET_ELEMENT_DATA )
    ADD_ENUM1( SET_ELEMENT_POSITION )
    ADD_ENUM1( SET_ELEMENT_VELOCITY )
    ADD_ENUM1( SET_ELEMENT_INTERIOR )
    ADD_ENUM1( SET_ELEMENT_DIMENSION )
    ADD_ENUM1( ATTACH_ELEMENTS )
    ADD_ENUM1( DETACH_ELEMENTS )
    ADD_ENUM1( SET_ELEMENT_ALPHA )
    ADD_ENUM1( SET_ELEMENT_NAME )
    ADD_ENUM1( SET_ELEMENT_HEALTH )
    ADD_ENUM1( SET_ELEMENT_MODEL )
    ADD_ENUM1( SET_ELEMENT_ATTACHED_OFFSETS )
    ADD_ENUM1( SET_PLAYER_MONEY )            
    ADD_ENUM1( SHOW_PLAYER_HUD_COMPONENT )
    ADD_ENUM1( FORCE_PLAYER_MAP )
    ADD_ENUM1( SET_PLAYER_NAMETAG_TEXT )
    ADD_ENUM1( REMOVE_PLAYER_NAMETAG_COLOR )
    ADD_ENUM1( SET_PLAYER_NAMETAG_COLOR )
    ADD_ENUM1( SET_PLAYER_NAMETAG_SHOWING )
    ADD_ENUM1( SET_PED_ARMOR )
    ADD_ENUM1( SET_PED_ROTATION )
    ADD_ENUM1( GIVE_PED_JETPACK )
    ADD_ENUM1( REMOVE_PED_JETPACK )
    ADD_ENUM1( REMOVE_PED_CLOTHES )
    ADD_ENUM1( SET_PED_GRAVITY )
    ADD_ENUM1( SET_PED_CHOKING )
    ADD_ENUM1( SET_PED_FIGHTING_STYLE )
    ADD_ENUM1( SET_PED_MOVE_ANIM )
    ADD_ENUM1( WARP_PED_INTO_VEHICLE )
    ADD_ENUM1( REMOVE_PED_FROM_VEHICLE )
    ADD_ENUM1( SET_PED_DOING_GANG_DRIVEBY )
    ADD_ENUM1( SET_PED_ANIMATION )
    ADD_ENUM1( SET_PED_ANIMATION_PROGRESS )
    ADD_ENUM1( SET_PED_ON_FIRE )
    ADD_ENUM1( SET_PED_HEADLESS )
    ADD_ENUM1( SET_PED_FROZEN )
    ADD_ENUM1( RELOAD_PED_WEAPON )
    ADD_ENUM1( DESTROY_ALL_VEHICLES )
    ADD_ENUM1( FIX_VEHICLE )
    ADD_ENUM1( BLOW_VEHICLE )
    ADD_ENUM1( SET_VEHICLE_ROTATION )
    ADD_ENUM1( SET_VEHICLE_TURNSPEED )
    ADD_ENUM1( SET_VEHICLE_COLOR )
    ADD_ENUM1( SET_VEHICLE_LOCKED )
    ADD_ENUM1( SET_VEHICLE_DOORS_UNDAMAGEABLE )
    ADD_ENUM1( SET_VEHICLE_SIRENE_ON )
    ADD_ENUM1( SET_VEHICLE_LANDING_GEAR_DOWN )
    ADD_ENUM1( SET_HELICOPTER_ROTOR_SPEED )
    ADD_ENUM1( ADD_VEHICLE_UPGRADE )
    ADD_ENUM1( ADD_ALL_VEHICLE_UPGRADES )
    ADD_ENUM1( REMOVE_VEHICLE_UPGRADE )
    ADD_ENUM1( SET_VEHICLE_DAMAGE_STATE )
    ADD_ENUM1( SET_VEHICLE_OVERRIDE_LIGHTS )
    ADD_ENUM1( SET_VEHICLE_ENGINE_STATE )
    ADD_ENUM1( SET_VEHICLE_DIRT_LEVEL )
    ADD_ENUM1( SET_VEHICLE_DAMAGE_PROOF )
    ADD_ENUM1( SET_VEHICLE_PAINTJOB )
    ADD_ENUM1( SET_VEHICLE_FUEL_TANK_EXPLODABLE )
    ADD_ENUM1( SET_VEHICLE_WHEEL_STATES )
    ADD_ENUM1( SET_VEHICLE_FROZEN )
    ADD_ENUM1( SET_TRAIN_DERAILED )
    ADD_ENUM1( SET_TRAIN_DERAILABLE )
    ADD_ENUM1( SET_TRAIN_DIRECTION )
    ADD_ENUM1( SET_TRAIN_SPEED )
    ADD_ENUM1( SET_TAXI_LIGHT_ON )
    ADD_ENUM1( SET_VEHICLE_HEADLIGHT_COLOR )
    ADD_ENUM1( SET_VEHICLE_DOOR_OPEN_RATIO )
    ADD_ENUM1( GIVE_WEAPON )
    ADD_ENUM1( TAKE_WEAPON )
    ADD_ENUM1( TAKE_ALL_WEAPONS )
    ADD_ENUM1( SET_WEAPON_AMMO )
    ADD_ENUM1( SET_WEAPON_SLOT )
    ADD_ENUM1( DESTROY_ALL_BLIPS )
    ADD_ENUM1( SET_BLIP_ICON )
    ADD_ENUM1( SET_BLIP_SIZE )
    ADD_ENUM1( SET_BLIP_COLOR )
    ADD_ENUM1( SET_BLIP_ORDERING )
    ADD_ENUM1( DESTROY_ALL_OBJECTS )
    ADD_ENUM1( SET_OBJECT_ROTATION )
    ADD_ENUM1( MOVE_OBJECT )
    ADD_ENUM1( STOP_OBJECT )
    ADD_ENUM1( DESTROY_ALL_RADAR_AREAS )
    ADD_ENUM1( SET_RADAR_AREA_SIZE )
    ADD_ENUM1( SET_RADAR_AREA_COLOR )
    ADD_ENUM1( SET_RADAR_AREA_FLASHING )
    ADD_ENUM1( DESTROY_ALL_MARKERS )
    ADD_ENUM1( SET_MARKER_TYPE )
    ADD_ENUM1( SET_MARKER_COLOR )
    ADD_ENUM1( SET_MARKER_SIZE )
    ADD_ENUM1( SET_MARKER_TARGET )
    ADD_ENUM1( SET_MARKER_ICON )
    ADD_ENUM1( DESTROY_ALL_PICKUPS )
    ADD_ENUM1( SET_PICKUP_TYPE )
    ADD_ENUM1( SET_PICKUP_VISIBLE )
    ADD_ENUM1( PLAY_SOUND )
    ADD_ENUM1( BIND_KEY )
    ADD_ENUM1( UNBIND_KEY )
    ADD_ENUM1( BIND_COMMAND )
    ADD_ENUM1( UNBIND_COMMAND )
    ADD_ENUM1( TOGGLE_CONTROL_ABILITY )
    ADD_ENUM1( TOGGLE_ALL_CONTROL_ABILITY )
    ADD_ENUM1( SET_CONTROL_STATE )
    ADD_ENUM1( FORCE_RECONNECT )
    ADD_ENUM1( SET_TEAM_NAME )
    ADD_ENUM1( SET_TEAM_COLOR )
    ADD_ENUM1( SET_PLAYER_TEAM )
    ADD_ENUM1( SET_TEAM_FRIENDLY_FIRE )
    ADD_ENUM1( SET_WANTED_LEVEL )
    ADD_ENUM1( SET_CAMERA_MATRIX )
    ADD_ENUM1( SET_CAMERA_TARGET )
    ADD_ENUM1( SET_CAMERA_INTERIOR )
    ADD_ENUM1( FADE_CAMERA )
    ADD_ENUM1( SHOW_CURSOR )
    ADD_ENUM1( SHOW_CHAT )
    ADD_ENUM1( SET_GRAVITY )
    ADD_ENUM1( SET_GAME_SPEED )
    ADD_ENUM1( SET_WAVE_HEIGHT )
    ADD_ENUM1( SET_SKY_GRADIENT )
    ADD_ENUM1( RESET_SKY_GRADIENT )
    ADD_ENUM1( SET_HEAT_HAZE )
    ADD_ENUM1( RESET_HEAT_HAZE )
    ADD_ENUM1( SET_BLUR_LEVEL )
    ADD_ENUM1( SET_FPS_LIMIT )
    ADD_ENUM1( SET_GARAGE_OPEN )
    ADD_ENUM1( RESET_MAP_INFO )
    ADD_ENUM1( SET_GLITCH_ENABLED )
    ADD_ENUM1( SET_CLOUDS_ENABLED )
    ADD_ENUM1( REMOVE_ELEMENT_DATA )
    ADD_ENUM1( SET_VEHICLE_HANDLING )
    ADD_ENUM1( SET_VEHICLE_HANDLING_PROPERTY )
    ADD_ENUM1( RESET_VEHICLE_HANDLING_PROPERTY )
    ADD_ENUM1( RESET_VEHICLE_HANDLING )
    ADD_ENUM1( TOGGLE_DEBUGGER )
    ADD_ENUM1( SET_ELEMENT_WATER_LEVEL )
    ADD_ENUM1( SET_ALL_ELEMENT_WATER_LEVEL )
    ADD_ENUM1( SET_WORLD_WATER_LEVEL )
    ADD_ENUM1( RESET_WORLD_WATER_LEVEL )
    ADD_ENUM1( SET_WATER_VERTEX_POSITION )
    ADD_ENUM1( SET_ELEMENT_DOUBLESIDED )
    ADD_ENUM1( SET_TRAFFIC_LIGHT_STATE )
    ADD_ENUM1( SET_VEHICLE_TURRET_POSITION )
    ADD_ENUM1( SET_OBJECT_SCALE )
    ADD_ENUM1( SET_ELEMENT_COLLISIONS_ENABLED )
    ADD_ENUM1( SET_JETPACK_MAXHEIGHT )
    ADD_ENUM1( SET_WATER_COLOR )
    ADD_ENUM1( RESET_WATER_COLOR )
    ADD_ENUM1( SET_ELEMENT_FROZEN )
    ADD_ENUM1( SET_LOW_LOD_ELEMENT )
    ADD_ENUM1( SET_BLIP_VISIBLE_DISTANCE )
    ADD_ENUM1( SET_INTERIOR_SOUNDS_ENABLED )
    ADD_ENUM1( SET_RAIN_LEVEL )
    ADD_ENUM1( SET_SUN_SIZE )
    ADD_ENUM1( SET_SUN_COLOR )
    ADD_ENUM1( SET_WIND_VELOCITY )
    ADD_ENUM1( SET_FAR_CLIP_DISTANCE )
    ADD_ENUM1( SET_FOG_DISTANCE )
    ADD_ENUM1( RESET_RAIN_LEVEL )
    ADD_ENUM1( RESET_SUN_SIZE )
    ADD_ENUM1( RESET_SUN_COLOR )
    ADD_ENUM1( RESET_WIND_VELOCITY )
    ADD_ENUM1( RESET_FAR_CLIP_DISTANCE )
    ADD_ENUM1( RESET_FOG_DISTANCE )
    ADD_ENUM1( SET_AIRCRAFT_MAXHEIGHT )
    ADD_ENUM1( SET_WEAPON_PROPERTY )
    ADD_ENUM1( SET_VEHICLE_VARIANT )
    ADD_ENUM1( REMOVE_WORLD_MODEL )
    ADD_ENUM1( RESTORE_WORLD_MODEL )
    ADD_ENUM1( RESTORE_ALL_WORLD_MODELS )
    ADD_ENUM1( TAKE_PLAYER_SCREEN_SHOT )
    ADD_ENUM1( SET_OCCLUSIONS_ENABLED )
    ADD_ENUM1( GIVE_VEHICLE_SIRENS )
    ADD_ENUM1( REMOVE_VEHICLE_SIRENS )
    ADD_ENUM1( SET_VEHICLE_SIRENS )
    ADD_ENUM1( SET_SYNC_INTERVALS )
    ADD_ENUM1( SET_JETPACK_WEAPON_ENABLED )
    ADD_ENUM1( FIRE_CUSTOM_WEAPON )
    ADD_ENUM1( SET_CUSTOM_WEAPON_STATE )
    ADD_ENUM1( SET_CUSTOM_WEAPON_AMMO )
    ADD_ENUM1( SET_CUSTOM_WEAPON_CLIP_AMMO )
    ADD_ENUM1( SET_CUSTOM_WEAPON_TARGET )
    ADD_ENUM1( RESET_CUSTOM_WEAPON_TARGET )
    ADD_ENUM1( SET_CUSTOM_WEAPON_FLAGS )
    ADD_ENUM1( SET_CUSTOM_WEAPON_FIRING_RATE )
    ADD_ENUM1( RESET_CUSTOM_WEAPON_FIRING_RATE )
    ADD_ENUM1( SET_WEAPON_OWNER )
    ADD_ENUM1( SET_AIRCRAFT_MAXVELOCITY )
    ADD_ENUM1( RESET_MOON_SIZE )
    ADD_ENUM1( SET_MOON_SIZE )
    ADD_ENUM1( SET_VEHICLE_PLATE_TEXT )
IMPLEMENT_ENUM_END( "eElementRPCFunctions" )

DECLARE_ENUM( CRPCFunctions::eRPCFunctions );
IMPLEMENT_ENUM_BEGIN( CRPCFunctions::eRPCFunctions )
    ADD_ENUM1( CRPCFunctions::PLAYER_INGAME_NOTICE )
    ADD_ENUM1( CRPCFunctions::INITIAL_DATA_STREAM )
    ADD_ENUM1( CRPCFunctions::PLAYER_TARGET )
    ADD_ENUM1( CRPCFunctions::PLAYER_WEAPON )
    ADD_ENUM1( CRPCFunctions::KEY_BIND )
    ADD_ENUM1( CRPCFunctions::CURSOR_EVENT )
    ADD_ENUM1( CRPCFunctions::REQUEST_STEALTH_KILL )
IMPLEMENT_ENUM_END( "eRPCFunctions" )


struct SRPCPacketStat
{
    int iCount;
    int iTotalBytes;
};

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatRPCPacketUsageImpl : public CPerfStatRPCPacketUsage
{
public:
    ZERO_ON_NEW

                                CPerfStatRPCPacketUsageImpl  ( void );
    virtual                     ~CPerfStatRPCPacketUsageImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatRPCPacketUsage
    virtual void                UpdatePacketUsageIn     ( uchar ucRpcId, uint uiSize );
    virtual void                UpdatePacketUsageOut    ( uchar ucRpcId, uint uiSize );

    // CPerfStatRPCPacketUsageImpl
    void                        MaybeRecordStats        ( void );

    int                         m_iStatsCleared;
    CElapsedTime                m_TimeSinceGetStats;
    long long                   m_llNextRecordTime;
    SString                     m_strCategoryName;
    SRPCPacketStat              m_PrevPacketStatsIn [ 256 ];
    SRPCPacketStat              m_PacketStatsIn [ 256 ];
    SRPCPacketStat              m_PacketStatsLiveIn [ 256 ];
    SFixedArray < long long, 256 > m_ShownPacketStatsIn;
    SRPCPacketStat              m_PrevPacketStatsOut [ 256 ];
    SRPCPacketStat              m_PacketStatsOut [ 256 ];
    SRPCPacketStat              m_PacketStatsLiveOut [ 256 ];
    SFixedArray < long long, 256 > m_ShownPacketStatsOut;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatRPCPacketUsageImpl* g_pPerfStatRPCPacketUsageImp = NULL;

CPerfStatRPCPacketUsage* CPerfStatRPCPacketUsage::GetSingleton ()
{
    if ( !g_pPerfStatRPCPacketUsageImp )
        g_pPerfStatRPCPacketUsageImp = new CPerfStatRPCPacketUsageImpl ();
    return g_pPerfStatRPCPacketUsageImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::CPerfStatRPCPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatRPCPacketUsageImpl::CPerfStatRPCPacketUsageImpl ( void )
{
    m_strCategoryName = "RPC Packet usage";
    assert( sizeof ( m_PacketStatsIn ) == sizeof ( m_PrevPacketStatsIn ) );
    assert( sizeof ( m_PacketStatsIn ) == sizeof ( m_PacketStatsLiveIn ) );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::CPerfStatRPCPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatRPCPacketUsageImpl::~CPerfStatRPCPacketUsageImpl ( void )
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatRPCPacketUsageImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::DoPulse ( void )
{
    MaybeRecordStats();
}


///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::UpdatePacketUsageIn
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::UpdatePacketUsageIn ( uchar ucRpcId, uint uiSize )
{
    SRPCPacketStat& stat = m_PacketStatsLiveIn[ ucRpcId ];
    stat.iCount++;
    stat.iTotalBytes += uiSize;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::UpdatePacketUsageOut
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::UpdatePacketUsageOut ( uchar ucRpcId, uint uiSize )
{
    SRPCPacketStat& stat = m_PacketStatsLiveOut[ ucRpcId ];
    stat.iCount++;
    stat.iTotalBytes += uiSize;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::MaybeRecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::MaybeRecordStats ( void )
{
    // Someone watching?
    if ( m_TimeSinceGetStats.Get () < 10000 )
    {
        // Time for record update?    // Copy and clear once every 5 seconds
        long long llTime = GetTickCount64_ ();
        if ( llTime >= m_llNextRecordTime )
        {
            m_llNextRecordTime = Max ( m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9 );

            // Save previous sample so we can calc the delta values
            memcpy ( m_PrevPacketStatsIn, m_PacketStatsIn, sizeof ( m_PacketStatsIn ) );
            memcpy ( m_PacketStatsIn, m_PacketStatsLiveIn, sizeof ( m_PacketStatsIn ) );

            memcpy ( m_PrevPacketStatsOut, m_PacketStatsOut, sizeof ( m_PacketStatsOut ) );
            memcpy ( m_PacketStatsOut, m_PacketStatsLiveOut, sizeof ( m_PacketStatsOut ) );

            if ( m_iStatsCleared == 1 )
            {
                // Prime if was zeroed
                memcpy ( m_PrevPacketStatsIn, m_PacketStatsIn, sizeof ( m_PacketStatsIn ) );
                memcpy ( m_PrevPacketStatsOut, m_PacketStatsOut, sizeof ( m_PacketStatsOut ) );
                m_iStatsCleared = 2;
            }
            else
            if ( m_iStatsCleared == 2 )
                m_iStatsCleared = 0;
        }
    }
    else
    {
        // No one watching
        if ( !m_iStatsCleared )
        {
            memset ( m_PrevPacketStatsIn, 0, sizeof ( m_PacketStatsIn ) );
            memset ( m_PacketStatsIn, 0, sizeof ( m_PacketStatsIn ) );
            memset ( m_PrevPacketStatsOut, 0, sizeof ( m_PacketStatsOut ) );
            memset ( m_PacketStatsOut, 0, sizeof ( m_PacketStatsOut ) );
            m_iStatsCleared = 1;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    m_TimeSinceGetStats.Reset ();
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
        pResult->AddColumn ( "RPC Packet usage help" );
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

    if ( m_iStatsCleared )
    {
        pResult->AddRow ()[0] ="Sampling... Please wait";
    }

    long long llTickCountNow = CTickCount::Now ().ToLongLong ();


    // Fill rows
    for ( uint i = 0 ; i < 256 ; i++ )
    {
        // Calc incoming delta values
        SPacketStat statInDelta;
        {
            const SRPCPacketStat& statInPrev = m_PrevPacketStatsIn [ i ];
            const SRPCPacketStat& statInNow = m_PacketStatsIn [ i ];
            statInDelta.iCount      = statInNow.iCount - statInPrev.iCount;
            statInDelta.iTotalBytes = statInNow.iTotalBytes - statInPrev.iTotalBytes;
            //statInDelta.totalTime   = statInNow.totalTime - statInPrev.totalTime;
        }

        if ( !statInDelta.iCount )
        {
            // Once displayed, keep a row displayed for at least 20 seconds
            if ( llTickCountNow - m_ShownPacketStatsIn[i] > 20000 )
                continue;
        }
        else
        {
            m_ShownPacketStatsIn[i] = llTickCountNow;
        }

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        // Turn "CRPCFunctions::PLAYER_WEAPON" into "64_Player_weapon"
        SString strPacketDesc = EnumToString ( (CRPCFunctions::eRPCFunctions)i ).SplitRight ( "CRPCFunctions::", NULL, -1 ).ToLower ();
        row[c++] = SString ( "%d_", i ) + strPacketDesc.Left ( 1 ).ToUpper () + strPacketDesc.SubStr ( 1 );

        if ( statInDelta.iCount )
        {
            row[c++] = SString ( "%d", ( statInDelta.iCount + 4 ) / 5 );
            row[c++] = CPerfStatManager::GetScaledByteString ( ( statInDelta.iTotalBytes + 4 ) / 5 );
            row[c++] = "n/a";
        }
        else
        {
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "-";
        }

        row[c++] = "-";
        row[c++] = "-";
        row[c++] = "-";
    }

    // Fill rows
    for ( uint i = 0 ; i < 256 ; i++ )
    {
        // Calc outgoing delta values
        SRPCPacketStat statOutDelta;
        {
            const SRPCPacketStat& statOutPrev = m_PrevPacketStatsOut [ i ];
            const SRPCPacketStat& statOutNow = m_PacketStatsOut [ i ];
            statOutDelta.iCount      = statOutNow.iCount - statOutPrev.iCount;
            statOutDelta.iTotalBytes = statOutNow.iTotalBytes - statOutPrev.iTotalBytes;
        }

        if ( !statOutDelta.iCount )
        {
            // Once displayed, keep a row displayed for at least 20 seconds
            if ( llTickCountNow - m_ShownPacketStatsOut[i] > 20000 )
                continue;
        }
        else
        {
            m_ShownPacketStatsOut[i] = llTickCountNow;
        }

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        // Turn "SET_WEAPON_OWNER" into "64_Set_weapon_owner"
        SString strPacketDesc = EnumToString ( (eElementRPCFunctions)i ).ToLower ();
        row[c++] = SString ( "%d_", i ) + strPacketDesc.Left ( 1 ).ToUpper () + strPacketDesc.SubStr ( 1 );

        row[c++] = "-";
        row[c++] = "-";
        row[c++] = "-";

        if ( statOutDelta.iCount )
        {
            row[c++] = SString ( "%d", ( statOutDelta.iCount + 4 ) / 5 );
            row[c++] = CPerfStatManager::GetScaledByteString ( ( statOutDelta.iTotalBytes + 4 ) / 5 );
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
