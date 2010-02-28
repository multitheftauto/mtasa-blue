/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayer.cpp
*  PURPOSE:     Player ped entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;   

CPlayer::CPlayer ( CPlayerManager* pPlayerManager, class CScriptDebugging* pScriptDebugging, const NetServerPlayerID& PlayerSocket ) : CPed ( NULL, NULL, NULL, 0 )
{
    // Init
    m_pPlayerManager = pPlayerManager;
    m_pScriptDebugging = pScriptDebugging;
    m_PlayerSocket = PlayerSocket;

    m_iType = CElement::PLAYER;
    SetTypeName ( "player" );
    m_bIsPlayer = true;
    m_bDoNotSendEntities = false;
    m_szNick [0] = 0;
    m_iGameVersion = 0;
    m_usMTAVersion = 0;
    m_usBitStreamVersion = 0;
    m_bIsMuted = false;
    m_lMoney = 0;
    m_bNametagColorOverridden = false;

    m_iStatus = STATUS_CONNECTED;

    m_fRotation = 0.0f;
    m_fAimDirection = 0.0f;
    m_ucDriveByDirection = 0;
	m_bAkimboArmUp = false;    
    
    m_uiScriptDebugLevel = 0;

    m_ulTimeConnected = GetTime ();

    m_tNickChange = 0;

    m_ucLoginAttempts = 0;

    m_pPlayerTextManager = new CPlayerTextManager ( this );	

    m_bCamFadedIn = false;
    SetCamFadeColor ( 0, 0, 0 );

    m_PlayerAttackerID = INVALID_ELEMENT_ID;
    m_ucAttackWeapon = 0xFF;
    m_ucAttackBodyPart = 0xFF;
    m_llSetDamageInfoTime = 0;

    m_pTeam = NULL;

    m_pPad = new CPad ( this );

    m_bDebuggerVisible = false;

    m_uiWantedLevel = 0;

    m_bForcedScoreboard = false;
    m_bForcedMap = false;   

    m_pCamera = new CPlayerCamera ( this );

    m_pKeyBinds = new CKeyBinds ( this );

    m_bCursorShowing = false;

    m_szNametagText = NULL;
    m_ucNametagR = 0xFF;
    m_ucNametagG = 0xFF;
    m_ucNametagB = 0xFF;
    m_bNametagShowing = true;        

    m_ucBlurLevel = 36; // Default

    // Sync stuff
    m_bSyncingVelocity = false;
    m_uiPuresyncPackets = 0;

    m_ulLastReceivedSyncTime = 0;

    // Add us to the manager
    pPlayerManager->AddToList ( this );
}


CPlayer::~CPlayer ( void )
{
    // Clear our sync time list and make sure no other player references us.
    ClearSyncTimes ();
    m_pPlayerManager->ClearSyncTime ( *this );

    // Make sure the script debugger doesn't reference us
    SetScriptDebugLevel ( 0 );    

    if ( m_pCamera )
    {
        // Remove the camera from its targets FollowingCameras list
        if ( m_pCamera->GetTarget () )
            m_pCamera->GetTarget ()->m_FollowingCameras.remove ( m_pCamera );
        delete m_pCamera;
        m_pCamera = NULL;
    }    

    // Make sure nobody's syncing us
    RemoveAllSyncingVehicles ();
    RemoveAllSyncingPeds ();

    // Delete the player text manager
    delete m_pPlayerTextManager;

    // Destroy our nick
    if ( m_szNametagText )
    {
        delete [] m_szNametagText;
        m_szNametagText = NULL;
    }

    SetTeam ( NULL, true );

    delete m_pPad;

    delete m_pKeyBinds;

    // Unlink from manager
    Unlink ();

    // Unparent us (CElement's unparenting will crash because of the incomplete vtable at that point)
    m_bDoNotSendEntities = true;
    SetParentObject ( NULL );
}


void CPlayer::DoPulse ( void )
{
    if ( GetStatus () == STATUS_JOINED )
    {
        m_pPlayerTextManager->Process ();
    }
}


void CPlayer::Unlink ( void )
{
    // Remove us from the player manager
    m_pPlayerManager->RemoveFromList ( this );
}


void CPlayer::SetNick ( const char* szNick )
{
    if ( strlen ( m_szNick ) > 0 && strcmp ( m_szNick, szNick ) != 0 )
    {
        // If changing, add the new name to the whowas list
        char szIP [22];
        g_pGame->GetConsole ()->GetWhoWas ()->Add ( szNick, inet_addr ( GetSourceIP( szIP ) ), GetSerial () );
    }

    assert ( sizeof ( m_szNick ) == MAX_NICK_LENGTH + 1 );
    // Copy the nick to us
    STRNCPY ( m_szNick, szNick, MAX_NICK_LENGTH + 1 );
}


char* CPlayer::GetSourceIP ( char* pBuffer )
{
    // Grab the player IP
    char szIP [22];
    unsigned short usPort;
    g_pNetServer->GetPlayerIP ( m_PlayerSocket, szIP, &usPort );

    // Copy the buffer and return a pointer to it
    strcpy ( pBuffer, szIP );
    return pBuffer;
}

// TODO [28-Feb-2009] packetOrdering is currently always PACKET_ORDERING_GAME
void CPlayer::Send ( const CPacket& Packet, NetServerPacketOrdering packetOrdering )
{
    // Use the flags to determine how to send it
    NetServerPacketReliability Reliability;
    unsigned long ulFlags = Packet.GetFlags ();
    if ( ulFlags & PACKET_RELIABLE )
    {
        if ( ulFlags & PACKET_SEQUENCED )
        {
            Reliability = PACKET_RELIABILITY_RELIABLE_ORDERED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_RELIABLE;
        }
    }
    else
    {
        if ( ulFlags & PACKET_SEQUENCED )
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE_SEQUENCED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE;
        }
    }
    NetServerPacketPriority packetPriority = PACKET_PRIORITY_MEDIUM;
    if ( ulFlags & PACKET_HIGH_PRIORITY )
    {
        packetPriority = PACKET_PRIORITY_HIGH;
    }
    else if ( ulFlags & PACKET_LOW_PRIORITY )
    {
        packetPriority = PACKET_PRIORITY_LOW;
    }

    // Allocate a bitstream for it
    NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream ( GetBitStreamVersion () );
    if ( pBitStream )
    {
        // Write the content to it and send it
        if ( Packet.Write ( *pBitStream ) )
        {
            g_pNetServer->SendPacket ( Packet.GetPacketID (), m_PlayerSocket, pBitStream, FALSE, packetPriority, Reliability, packetOrdering );
        }

        // Destroy the bitstream
        g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
}


void CPlayer::SendEcho ( const char* szEcho )
{
    Send ( CChatEchoPacket ( szEcho, CHATCOLOR_MESSAGE ) );
}


void CPlayer::SendConsole ( const char* szEcho )
{
    Send ( CConsoleEchoPacket ( szEcho ) );
}


void CPlayer::AddSyncingVehicle ( CVehicle* pVehicle )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pVehicle->SetSyncer ( this );
        bAlreadyIn = false;

        // Add it to our list
        m_SyncingVehicles.push_back ( pVehicle );
    }
}


void CPlayer::RemoveSyncingVehicle ( CVehicle* pVehicle )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pVehicle->SetSyncer ( NULL );
        bAlreadyIn = false;

        // Remove it from our list
        if ( !m_SyncingVehicles.empty() ) m_SyncingVehicles.remove ( pVehicle );
    }
}


void CPlayer::RemoveAllSyncingVehicles ( void )
{
    // Unreference us from all
    list < CVehicle* > ::const_iterator iter = m_SyncingVehicles.begin ();
    for ( ; iter != m_SyncingVehicles.end (); iter++ )
    {
        (*iter)->m_pSyncer = NULL;
    }
}


void CPlayer::AddSyncingPed ( CPed* pPed )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pPed->SetSyncer ( this );
        bAlreadyIn = false;

        // Add it to our list
        m_SyncingPeds.push_back ( pPed );
    }
}


void CPlayer::RemoveSyncingPed ( CPed* pPed )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pPed->SetSyncer ( NULL );
        bAlreadyIn = false;

        // Remove it from our list
        if ( !m_SyncingPeds.empty() ) m_SyncingPeds.remove ( pPed );
    }
}


void CPlayer::RemoveAllSyncingPeds ( void )
{
    // Unreference us from all
    list < CPed* > ::const_iterator iter = m_SyncingPeds.begin ();
    for ( ; iter != m_SyncingPeds.end (); iter++ )
    {
        (*iter)->m_pSyncer = NULL;
    }
}


bool CPlayer::SetScriptDebugLevel ( unsigned int uiLevel )
{
    return m_pScriptDebugging->AddPlayer ( *this, uiLevel );
}


void CPlayer::SetCamFadeColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    #define COLOR_ARGB(a,r,g,b) \
        (((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
    #define COLOR_RGBA(r,g,b,a) COLOR_ARGB(a,r,g,b)

    m_ulCamFadeColor = COLOR_ARGB ( 255, ucRed, ucGreen, ucBlue );

	#undef COLOR_ARGB
	#undef COLOR_RGBA
}


void CPlayer::SetDamageInfo ( ElementID ElementID, unsigned char ucWeapon, unsigned char ucBodyPart )
{
    m_PlayerAttackerID = ElementID;
    m_ucAttackWeapon = ucWeapon;
    m_ucAttackBodyPart = ucBodyPart;
    m_llSetDamageInfoTime = GetTickCount64_ ();
}


void CPlayer::ValidateDamageInfo ( void )
{
    if ( m_llSetDamageInfoTime + 100 < GetTickCount64_ () )
    {
        // Reset if data is too old
        m_PlayerAttackerID = INVALID_ELEMENT_ID;
        m_ucAttackWeapon = 0xFF;
        m_ucAttackBodyPart = 0xFF;
    }
}


ElementID CPlayer::GetPlayerAttacker ( void )
{
    ValidateDamageInfo ();
    return m_PlayerAttackerID;
}


unsigned char CPlayer::GetAttackWeapon ( void )
{
    ValidateDamageInfo ();
    return m_ucAttackWeapon;
}


unsigned char CPlayer::GetAttackBodyPart ( void )
{
    ValidateDamageInfo ();
    return m_ucAttackBodyPart;
}


void CPlayer::SetTeam ( CTeam* pTeam, bool bChangeTeam )
{
    if ( pTeam == m_pTeam ) return;

    if ( m_pTeam && bChangeTeam )
        m_pTeam->RemovePlayer ( this, false );

    m_pTeam = pTeam;
    if ( m_pTeam && bChangeTeam )
        m_pTeam->AddPlayer ( this, false );
}


void CPlayer::Reset ( void )
{
    //Called when resetMapInfo is called to reset per player information that is reset in the clientside implimentation of resetMapInfo. This stops our functions clientside and serverside possibly returning different results.
    memset ( m_fStats, 0, sizeof ( m_fStats ) );
    m_fStats [ 24 ] = 569.0f;           // default max_health
    m_pClothes->DefaultClothes ();    
    m_bHasJetPack = false;

    // Removed
    //m_bForcedScoreboard = false;
    m_bForcedMap = false;
    m_ucInterior = 0;
    m_usDimension = 0;
    m_bCamFadedIn = true;
    SetCamFadeColor ( 0, 0, 0 );
    //m_pKeyBinds->Clear ();
    m_bCursorShowing = false;

    // Added
    m_ucFightingStyle = 15; // STYLE_GRAB_KICK

    SetNametagText ( NULL );
    m_ucAlpha = 255;

    // Make sure the nametag color is no longer overridden
    RemoveNametagOverrideColor ();

    m_bNametagShowing = true;
    m_usModel = 0;
}


void CPlayer::SetNametagText ( const char* szText )
{
    if ( m_szNametagText )
    {
        delete [] m_szNametagText;
        m_szNametagText = NULL;
    }
    if ( szText )
    {
        m_szNametagText = new char [ strlen ( szText ) + 1 ];
        strcpy ( m_szNametagText, szText );
    }
}


void CPlayer::GetNametagColor ( unsigned char& ucR, unsigned char& ucG, unsigned char& ucB )
{
    // Is the nametag color overridden?
    if ( m_bNametagColorOverridden )
    {
        ucR = m_ucNametagR;
        ucG = m_ucNametagG;
        ucB = m_ucNametagB;
    }
    else
    {
        // Is the player on a team?
        if ( m_pTeam )
        {
            // Use the team color
            m_pTeam->GetColor ( ucR, ucG, ucB );
        }
        else
        {
            // Default white color
            ucR = 255;
            ucG = 255;
            ucB = 255;
        }
    }
}


void CPlayer::SetNametagOverrideColor ( unsigned char ucR, unsigned char ucG, unsigned char ucB )
{
    m_ucNametagR = ucR;
    m_ucNametagG = ucG;
    m_ucNametagB = ucB;
    m_bNametagColorOverridden = true;
}


void CPlayer::RemoveNametagOverrideColor ( void )
{
    m_ucNametagR = 255;
    m_ucNametagG = 255;
    m_ucNametagB = 255;
    m_bNametagColorOverridden = false;
}


bool CPlayer::IsTimeToSendSyncFrom ( CPlayer& Player, unsigned long ulTimeNow )
{
    #define SLOW_SYNCRATE 1000
    #define DISTANCE_FOR_SLOW_SYNCRATE 320.0f

    // Loop through the sync stuff
    sPlayerSyncData* pData;
    list < sPlayerSyncData* > ::iterator iter = m_SyncTimes.begin ();
    for ( ; iter != m_SyncTimes.end (); iter++ )
    {
        pData = *iter;

        // Matching player?
        if ( &Player == pData->pPlayer )
        {
            // Is a slow sync rate required?
            bool bReqSlowSync = false;
            CVector vecCameraPosition;
            m_pCamera->GetPosition ( vecCameraPosition );
            const CVector& vecRemotePlayerPos = Player.GetPosition ();
            const CVector& vecLocalPlayerPos = GetPosition ();

            if ( ( DistanceBetweenPoints3D ( vecLocalPlayerPos, vecRemotePlayerPos ) >= DISTANCE_FOR_SLOW_SYNCRATE ) &&
                 ( DistanceBetweenPoints3D ( vecCameraPosition, vecRemotePlayerPos ) >= DISTANCE_FOR_SLOW_SYNCRATE ) )
            {
                // Allow 5 fast syncs when switching from fast to slow sync rate to ensure big moves away from the viewer are updated in a timely manner
                if ( pData->ulSwitchingToSlowSyncRate < 5 )
                    pData->ulSwitchingToSlowSyncRate++;
                else
                    bReqSlowSync = true;
            }
            else
            {
                pData->ulSwitchingToSlowSyncRate = 0;
            }

            // Skip if slow syncing and previous sync was less than SLOW_SYNCRATE ticks ago
            if ( bReqSlowSync && ulTimeNow - pData->ulLastSent < SLOW_SYNCRATE )
            {
                // Don't send
                return false;
            }

            // Send a sync now since we're close to him or it's time to do so
            // Remember now as the time we've done that.
            pData->ulLastSent = ulTimeNow;
            return true;
        }
    }

    // There are no matching entries for this player. This means we need to create one for it.
    pData = new sPlayerSyncData;
    pData->pPlayer = &Player;
    pData->ulLastSent = ulTimeNow;
    pData->ulSwitchingToSlowSyncRate = 0;
    m_SyncTimes.push_back ( pData );

    // And do the sync
    return true;
}


void CPlayer::ClearSyncTime ( CPlayer& Player )
{
    // Loop through our list of synctime entries
    sPlayerSyncData* pData;
    list < sPlayerSyncData* > ::iterator iter = m_SyncTimes.begin ();
    for ( ; iter != m_SyncTimes.end (); iter++ )
    {
        pData = *iter;

        // Is this our player?
        if ( pData->pPlayer == &Player )
        {
            // Delete the data in it
            delete pData;

            // Delete this entry and we're done. We won't exist twice in this list.
            m_SyncTimes.erase ( iter );
            return;
        }
    }
}


void CPlayer::ClearSyncTimes ( void )
{
    // Delete all our data
    list < sPlayerSyncData* > ::iterator iter = m_SyncTimes.begin ();
    for ( ; iter != m_SyncTimes.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list so we won't try accessing bad data later
    m_SyncTimes.clear ();
}
