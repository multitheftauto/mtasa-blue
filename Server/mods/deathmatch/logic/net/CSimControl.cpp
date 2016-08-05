/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"

namespace
{
    bool                ms_bEnabled = false;
    bool                ms_bEnableRequest = false;
    CNetServerBuffer*   ms_pNetServerBuffer = NULL;
    CSimPlayerManager*  ms_pSimPlayerManager = NULL;
}


///////////////////////////////////////////////////////////////
//
// CSimControl::Startup
//
//
//
///////////////////////////////////////////////////////////////
void CSimControl::Startup ( void )
{
    ms_pSimPlayerManager = new CSimPlayerManager ();

    // Check packet flags are what we are expecting
    dassert( CPlayerPuresyncPacket().HasSimHandler() );
    dassert( CVehiclePuresyncPacket().HasSimHandler() );
    dassert( CKeysyncPacket().HasSimHandler() );
    dassert( CBulletsyncPacket().HasSimHandler() );
}


///////////////////////////////////////////////////////////////
//
// CSimControl::Shutdown
//
//
//
///////////////////////////////////////////////////////////////
void CSimControl::Shutdown ( void )
{
    EnableSimSystem ( false );
    SAFE_DELETE( ms_pSimPlayerManager );
}


///////////////////////////////////////////////////////////////
//
// CSimControl::EnableSimSystem
//
// Turn on and off here
// Not applied until the next pulse unless bApplyNow is set
//
///////////////////////////////////////////////////////////////
void CSimControl::EnableSimSystem ( bool bEnable, bool bApplyNow )
{
    ms_bEnableRequest = bEnable;
    if ( bApplyNow )
        DoPulse ();
}


///////////////////////////////////////////////////////////////
//
// CSimControl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CSimControl::DoPulse ( void )
{
    if ( ms_bEnableRequest == ms_bEnabled )
        return;
    ms_bEnabled = ms_bEnableRequest;

    if ( ms_bEnabled )
    {
        // Startup NetServerBuffer
        assert ( !ms_pNetServerBuffer );
        ms_pNetServerBuffer = new CNetServerBuffer ( ms_pSimPlayerManager );

        // Replace g_pNetServer
        g_pNetServer = ms_pNetServerBuffer;

        // Replace packet handler
        ms_pNetServerBuffer->RegisterPacketHandler ( CGame::StaticProcessPacket );

        // Let the pulsing begin
        ms_pNetServerBuffer->SetAutoPulseEnabled ( true );
    }
    else
    {
        // Stop the sync thread from doing anything by itself
        ms_pNetServerBuffer->SetAutoPulseEnabled ( false );

        // Restore g_pNetServer
        g_pNetServer = g_pRealNetServer;

        // Restore packet handler - This is blocking so will drain the outgoing queue
        ms_pNetServerBuffer->RegisterPacketHandler ( NULL );
        g_pRealNetServer->RegisterPacketHandler ( CGame::StaticProcessPacket );

        // Drain the incoming queue
        ms_pNetServerBuffer->ProcessIncoming ();

        // Kaboooom
        SAFE_DELETE ( ms_pNetServerBuffer );
    }
}


///////////////////////////////////////////////////////////////
//
// CSimControl::IsSimSystemEnabled
//
// Check if sim system is on
//
///////////////////////////////////////////////////////////////
bool CSimControl::IsSimSystemEnabled ( void )
{
    return ms_bEnabled;
}


///////////////////////////////////////////////////////////////
//
// CSimControl::AddSimPlayer
//
// Add a joining player
//
///////////////////////////////////////////////////////////////
void CSimControl::AddSimPlayer ( CPlayer* pPlayer )
{
    ms_pSimPlayerManager->AddSimPlayer ( pPlayer );
}


///////////////////////////////////////////////////////////////
//
// CSimControl::RemoveSimPlayer
//
// Remove a leaving player
//
///////////////////////////////////////////////////////////////
void CSimControl::RemoveSimPlayer ( CPlayer* pPlayer )
{
    ms_pSimPlayerManager->RemoveSimPlayer ( pPlayer );
}


///////////////////////////////////////////////////////////////
//
// CSimControl::UpdateSimPlayer
//
// Update a player at pure sync (and other) times
//
///////////////////////////////////////////////////////////////
void CSimControl::UpdateSimPlayer ( CPlayer* pPlayer )
{
    ms_pSimPlayerManager->UpdateSimPlayer ( pPlayer );
}
