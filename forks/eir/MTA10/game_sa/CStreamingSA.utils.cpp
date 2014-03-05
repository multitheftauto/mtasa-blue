/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.utils.cpp
*  PURPOSE:     Data streamer utilities
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

#include "CStreamingSA.utils.hxx"

extern CBaseModelInfoSAInterface **ppModelInfo;

/*=========================================================
    Streaming::StreamPedsAndVehicles

    Arguments:
        reqPos - position to stream entities around
    Purpose:
        Streams vehicle and ped models around the player.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040B650
    Note:
        This function is not used by MTA.
=========================================================*/
inline float GetEntityBasePlaneSpeed( const CPhysicalSAInterface *entity )
{
    const CVector& speed = entity->m_vecLinearVelocity;

    return CVector2D( speed[0], speed[1] ).Length();
}

inline ModelIdContainer& GetVehicleModelIdContainer( void )
{
    return *(ModelIdContainer*)0x00C0E9F8;
}

struct zoneInfo
{
    BYTE                m_pad[15];      // 0
    unsigned char       m_unk;          // 15
};

void __cdecl Streaming::StreamPedsAndVehicles( const CVector& reqPos )
{
    // NOTE: this function is not used by MTA.
#ifdef ENABLE_GAME_TRAFFIC
    if ( CVehicleSAInterface *playerVehicle = GetPlayerVehicle( -1, false ) )
    {
        switch( playerVehicle->m_unk39 )
        {
        case 3:
            if ( GetEntityBasePlaneSpeed( playerVehicle ) <= 0.1f )
                break;
        case 4:
            return;
        }
    }

    if ( GetVehicleModelIdContainer().Count() < 3 )
    {
        // TODO: analyze this.
        ((void (__cdecl*)( void ))0x0040B4F0)();
    }

    // Appears to stream peds by zones.
    ((void (__cdecl*)( void ))0x0040AA10)();

    if ( zoneInfo *info = *(zoneInfo**)0x00C0BC68 )
    {
        if ( info->m_unk != *(unsigned int*)0x0096552C )
        {
            ((void (__cdecl*)( void ))0x0040AFA0)();

            *(unsigned int*)0x0096552C = info->m_unk;
        }
    }
#endif //ENABLE_GAME_TRAFFIC
}

/*=========================================================
    Streaming::IsStreamingBusy

    Purpose:
        Returns whether the ingame loader is busy with
        loading models. If true, it is not recommended to
        request more models.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004076A0
=========================================================*/
bool __cdecl Streaming::IsStreamingBusy( void )
{
    return *(bool*)0x00B76850 || *(unsigned int*)0x008E4CB8 > 5;
}

/*=========================================================
    Streaming::IsCurrentHourPeriod

    Arguments:
        fromHour - the hour marking the beginning of the period
        toHour - the hour marking the end of it
    Purpose:
        Returns a boolean whether the given time period is
        reached. The current time is displayed at the HUD clock.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0052CEE0
=========================================================*/
bool __cdecl Streaming::IsCurrentHourPeriod( unsigned char fromHour, unsigned char toHour )
{
    unsigned char currentHour = *(unsigned char*)VAR_TimeHours;

    return
        ( fromHour <= toHour ) ?
        ( currentHour >= fromHour && currentHour < toHour ) :
        ( currentHour >= fromHour || currentHour < toHour );
}

void StreamingUtils_Init( void )
{
    // Hook important routines.
    HookInstall( 0x0052CEE0, (DWORD)Streaming::IsCurrentHourPeriod, 5 );
    HookInstall( 0x004076A0, (DWORD)Streaming::IsStreamingBusy, 5 );

    // Initialize sub modules.
    StreamingGC_Init();
}

void StreamingUtils_Shutdown( void )
{
    // Destroy sub modules.
    StreamingGC_Shutdown();
}